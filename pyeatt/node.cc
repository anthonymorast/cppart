
#include <node.h>
#include <metric.h>
#include <datatab.h>
#include <string>
#include <float.h>
#include <values.h>
#include <math.h>

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

		metric->findSplit(temp, curCol, where, dir, splitPoint, improve, 2);

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

	// check to make sure we are not past max nodes, max depth, min
	// alpha, and return if any of these stopping critera are met.
	if (level >= maxDepth || data->numRows() < minObs || cp <= alpha)
	{
		// nothing
		// "Create_Leaf_Node()
		return;
	}

	
	// great place for multi-threading
	for (int curCol = 1; curCol < cols; curCol++)
	{
		// sort by current column
		data->sortBy(curCol);

		// call function to find split point
		int where, dir;
		double splitPoint, improve;
		double leftMean,rightMean;
		double leftSS, rightSS, totalSS = 0;
		queue<DataTable*> q;

		metric->findSplit(data, curCol, where, dir, splitPoint, improve, minNode);

		// I would have swapped rtab and ltab, but... whatever.
		ltab = data->subSet(0,where);
		rtab = data->subSet(where+1,data->numRows()-1);

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
			DataTable *temp = q.front();
			q.pop();
			metric->getSplitCriteria(temp, &leftMean, &leftSS);
			totalSS += leftSS;
		}

		metric->getSplitCriteria(ltab,&leftMean,&leftSS);
		metric->getSplitCriteria(rtab,&rightMean,&rightSS);

		// do everything up to this point the same for parallel
		// remove everything down from here
		// in the parallel function return a struct containing all the information from here on
		// compare the bestSS and totalSS and only keep a copy of the struct relating to the bestSS
		// once we do all the columns use the information from the struct to split the node.
		// 
		// if (struct->thisSS < bestSS && struct->improve > 0) 
		// 	  bestStruct = struct
		// 	  bestSS = thisSS
		//
		// exit loop. When all columns are done create the left/right nodes and datatables

		if ((improve>0) && (totalSS<bestSS) && (leftSS>alpha) && (rightSS>alpha))
		{
			lftChild = new Node(this,ltab,leftMean,leftSS,depth+1);
			rgtChild = new Node(this,rtab,rightMean,rightSS,depth+1);
			bestSS = totalSS;
			direction = dir;
			splitValue = splitPoint;
			splitIndex = where;
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

	//cout << "split node at level "<<level<<" on variable "<<varName<<endl;

	//cout << cp << "  " << alpha << endl;
	if(left!=NULL)
		left->setId();
	if(right!=NULL)
		right->setId();

	if((left != NULL)&&(left->data->numRows()>minObs)) {
		left->split(level+1);
	}

	if((right != NULL)&&(right->data->numRows()>minObs)) {
		right->split(level+1);
	}

}

float Node::predict(double *sample)
{
	float pred = 0;
	if (left == NULL && right == NULL)
		return yval;

	if(direction < 0)
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
