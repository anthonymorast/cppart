
#include <node.h>
#include <metric.h>
#include <datatab.h>
#include <string>
#include <float.h>
#include <values.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stddef.h>

#include <fstream>
using namespace std;

// declare and initialize static data members
statisticalMetric *Node::metric=NULL; // Statistical metric being used
int Node::minObs;        // minimum amount of data required in a leaf node. AM: rpart makes the distinction between minNode and minObs
// I think minObs=min obs required to consider a split and minNodes=min number of obs in a leaf node.
// min Obs is used as stopping criteria (column-wise) while min node is used as row-wise stopping criteria
int Node::minNode;
int Node::nextId=1;
double Node::alpha=1750;
int Node::verbose=0;
int Node::maxDepth;
int Node::delays=0;

Node::Node(Node *Parent, DataTable *d,double mean,double Cp,int level)
{
    data=d;
    depth = level;
    splitIndex=-1;
    direction=0;
    splitValue=0.0; 
    left=right=NULL;
    nodeId = -1;
    parent = Parent;
    yval = mean;
    dev = cp = Cp;
}

Node::~Node()
{
    delete data;
    if(left != NULL)
        delete left;
    if(right != NULL)
        delete right;
}

void Node::dsplit(DataTable *temp, DataTable *&l, DataTable *&r)
{

    int cols = temp->numCols();
    double bestSS = DBL_MAX;
    DataTable *ltab,*rtab;

    for (int curCol = 1; curCol < cols; curCol++)
    {
        // sort by current column
        temp->sortBy(curCol);

        int where, dir;
        double splitPoint, improve;
        double leftMean,rightMean;
        double leftSS, rightSS, totalSS;

        metric->findSplit(temp, curCol, where, dir, splitPoint, improve, minNode);
        // I would have swapped rtab and ltab, but... whatever.
        ltab = temp->subSet(0,where);
        rtab = temp->subSet(where+1,temp->numRows()-1);

        metric->getSplitCriteria(ltab,&leftMean,&leftSS);
        metric->getSplitCriteria(rtab,&rightMean,&rightSS);

        totalSS = leftSS + rightSS;

        if ((improve>0) && (totalSS<bestSS))
        {
            bestSS = totalSS;
            r = rtab;
            l = ltab;
        }
    }
}

// formerly known as partition() and bestsplit()
void Node::split(int level)
{

    int cols = data->numCols();
    double bestSS = DBL_MAX;
    DataTable *ltab,*rtab;
    Node *lftChild,*rgtChild;

    if (level >= maxDepth || data->numRows() < minObs || cp <= alpha)
    {
        // nothing
        // "Create_Leaf_Node()
        return;
    }

	// setup MPI
	const int send_struct_tag = 1;	// tell slave nodes they have data
	const int send_x_tag = 4;		// send the x data (should combine with struct)
	const int send_y_tag = 5;		// send the y data (should combine with struct)
	const int results_tag = 2;		// tell master node we have results
	const int done_tag = 3;			// tell all nodes we're done
	const int data_tag = 6;			// data is coming

	int procs, rank, size;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// if there are more processors than there are columns
	if(cols < (size-1))
		procs = cols;
	else 
		procs = size-1; // remove master
	
	// create MPI types for structs
	const int nitems_send = 2;
	const int nitems_resp = 3;

	int blocklengths[2] = {1, 1}; // probbaly used to send array (set block legnth = array length [not sure though])
	MPI_Datatype types[2] = {MPI_INT, MPI_INT};
	MPI_Datatype send_data;
	MPI_Aint offsets[nitems_send];
	offsets[0] = offsetof(mpi_send, column);
	offsets[1] = offsetof(mpi_send, nrows);
	MPI_Type_create_struct(nitems_send, blocklengths, offsets, types, &send_data);

	int bl[3] = {1, 1, 1};
	MPI_Datatype t[3] = {MPI_INT, MPI_DOUBLE, MPI_DOUBLE};
	MPI_Datatype resp_data;
	MPI_Aint os[nitems_resp];
	os[0] = offsetof(mpi_resp, column);
	os[1] = offsetof(mpi_resp, sse);
	os[2] = offsetof(mpi_resp, improve);
	MPI_Type_create_struct(nitems_resp, bl, os, t, &resp_data);

	int best_col = INT_MAX;
	if(rank == 0)
	{
		int col_count = 0;
		// while there are columns left
		while(col_count < cols)
		{	
			// for each node that's not master
			for(int i = 1; i <= procs; i++) 
			{
				struct mpi_send snd;
				snd.column = col_count;
				snd.nrows = data->numRows();

				double *x = new double[data->numRows()];
				double *y = new double[data->numRows()];
				data->getColumnData(col_count, x);
				data->getColumnData(0, y);

				MPI_Send(NULL, 0, MPI_INT, i, data_tag, MPI_COMM_WORLD);
				MPI_Send(&snd, 1, send_data, i, send_struct_tag, MPI_COMM_WORLD);
				MPI_Send(&x, snd.nrows, MPI_DOUBLE, i, send_x_tag, MPI_COMM_WORLD);
				MPI_Send(&y, snd.nrows, MPI_DOUBLE, i, send_y_tag, MPI_COMM_WORLD);

				col_count++;
			}

			for(int i = 0; i < procs; i++)
			{
				struct mpi_resp resp;
				MPI_Status status;
				int n = data->numRows();

				MPI_Recv(&resp, 1, resp_data, MPI_ANY_SOURCE, results_tag, MPI_COMM_WORLD, &status);
				if(resp.improve > 0 && resp.sse < bestSS)
				{
					bestSS = resp.sse;
					best_col = resp.column;
				}
			}
		}
	}
	else 
	{
		MPI_Status status;
		MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		while(status.MPI_TAG == data_tag) {
			double *x, *y;
			struct mpi_send snd;
			MPI_Recv(&snd, 1, send_data, 0, send_struct_tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&x, snd.nrows, MPI_DOUBLE, 0, send_x_tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&y, snd.nrows, MPI_DOUBLE, 0, send_y_tag, MPI_COMM_WORLD, &status);
	
			// do the split
			// combine the x and y columns and create data table s.t. col0 = y and col1 = x;
	        data->sortBy(1);

	        // call function to find split point
    	    int where, dir;
        	double splitPoint, improve;
     	   	double leftMean,rightMean;
        	double leftSS, rightSS, totalSS = 0;
      	 	queue<double*> q;

	        metric->findSplitMPI(x, y, where, dir, splitPoint, improve, minNode, snd.nrows);
			/* NEED TO IMPLEMENT THESE FOR ONE COLUMN "DO_SPLIT" in old logic should work
			 * could we just create a new datatable here with "our data" and use the old functions???
        	if (dir < 0) {
            	ltab = data->subSet(0,where);
            	rtab = data->subSet(where+1,data->numRows()-1);
	        } else {
    	        ltab = data->subSet(where+1, data->numRows()-1);
       	     	rtab = data->subSet(0, where);
        	}*/

	        q.push(ltab); q.push(rtab);
      	  	long unsigned int stopSize = pow(2, delays+1);
        	while(q.size() < stopSize)
        	{
            	DataTable *temp = q.front(), *l = NULL, *r = NULL;

	            dsplit(temp, l, r);
   	         	if(l == NULL && r == NULL)
            	{
                	// if there isn't enough data to split on, just use the SSE of the smallest possible partitions 
                	stopSize--;
                	q.push(q.front());
            	}
            	else 
            	{
                	q.push(l); q.push(r);
            	}
            	q.pop();
        	}
        	if(q.size() != stopSize) 
        	{
            	cout << "Error splitting node, queue does not contain the right amount of partitions.";
            	cout << endl;
            	exit(0);
        	}
       	 	while(!q.empty())
        	{
            	// sum up SSE for each partition
            	double *temp = q.front();
            	q.pop();
            	metric->getSplitCriteria(temp, &leftMean, &leftSS);
            	totalSS += leftSS;
        	}

	        metric->getSplitCriteria(ltab,&leftMean,&leftSS);
    	    metric->getSplitCriteria(rtab,&rightMean,&rightSS);

			double improve=0, totalSS=0;
			struct mpi_resp resp;
			resp.column = snd.column;
			resp.improve = improve;
			resp.sse = totalSS;
			MPI_Send(&resp, 1, resp_data, 0, results_tag, MPI_COMM_WORLD);

			// keep receiving data until we get the done_tag value, at which point we'll exit
			MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
	}

	// only do post-processing on master thread
	if(rank == 0)
	{
		// send to each process to stop
		for(int i = 1; i < size; i++)
		{
			MPI_Send(NULL, 0, MPI_INT, i, done_tag, MPI_COMM_WORLD);
		}
		
	}


    for (int curCol = 1; curCol < cols; curCol++)
    {

        if ((improve>0) && (totalSS<bestSS) && (leftSS>alpha) && (rightSS>alpha))
        {
            lftChild = new Node(this,ltab,leftMean,leftSS,depth+1);
            rgtChild = new Node(this,rtab,rightMean,rightSS,depth+1);
            bestSS = totalSS;
            direction = dir;
            splitValue = splitPoint;
            splitIndex = where;
            varIndex = curCol;
            varName = data->getName(curCol);
            if(right != NULL)
                delete right;
            right = rgtChild;
            if(left != NULL)
                delete left;
            left = lftChild;
        }
        else
        {
            delete ltab;
            delete rtab;
        }
    }

    if(left!=NULL)
        left->setId();  // setId needs to be thread safe
    if(right!=NULL)
        right->setId();

    if((left != NULL)&&(left->data->numRows()>minObs)) {
        left->split(level+1);
    }

    if((right != NULL)&&(right->data->numRows()>minObs)) {
        right->split(level+1);
    }

	MPI_Finalize();
}

float Node::predict(double *sample)
{
    float pred = 0;
    if (left == NULL && right == NULL)
        return yval;

    int dir = direction;
    if(sample[varIndex] > splitValue)
        dir *= -1;

    if(dir < 0)
    {
        if(left == NULL)
            return yval;
        else 
            pred = left->predict(sample);
    } 
    else 
    {
        if(right == NULL)
            return yval;
        else 
            pred = right->predict(sample);
    }
    return pred;
}

void Node::print(ofstream &fout, bool isRight)
{
    string directionStr = "";
    string tabString = "";
    string terminalStr = "";

    if(parent == NULL)
    {
        fout << "node) split, number observations, deviance, yval\n"
            "* denotes a terminal node\n\n";
        fout << "1) root " << data->numRows() << " " <<
            dev << " " << yval << endl;
    }
    else
    {

        for (int i = 0; i < depth; i++) {
            tabString += "\t";
        }

        int dir = parent->direction;
        if (isRight) 
            dir *= -1;

        if (dir <= 0) {
            directionStr = "<";
        }
        else {
            directionStr = ">";
        }

        if (isRight) {
            directionStr += "=";
        }
        else {
            directionStr += " ";
        }

        if (left == NULL && right == NULL) {
            terminalStr += "*";
        }
        fout << tabString << nodeId << ") " <<
            parent->varName << directionStr << parent->splitValue << " " <<
            data->numRows() << "  " << dev << " " <<
            yval << " " << terminalStr << "  " << dir << endl;
    }

    if(left != NULL)
        left->print(fout, false);
    if(right != NULL)
        right->print(fout, true);

}

void Node::getImpurity(float &imp)
{
    if(left == NULL && right == NULL) // leaf node
        imp += dev;

    if(left != NULL)
        left->getImpurity(imp);
    if(right != NULL)
        right->getImpurity(imp);
}
