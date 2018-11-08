
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


	// Find the best variable (and location) for splitting the current
	// node. Go through each colum, and track the best.
	// This was the bestsplit() function in previous implementation
	for (int curCol = 1; curCol < cols; curCol++)
	{

		// sort by current column
		data->sortBy(curCol);

		// call function to find split point
		int where, dir;
		double splitPoint, improve;
		double leftMean,rightMean;
		double leftSS, rightSS, totalSS;

		// Make findSplit recursive with lookahead
		metric->findSplit(data, curCol, where, dir, splitPoint, improve, minNode);

		// I would have swapped rtab and ltab, but... whatever.
		ltab = data->subSet(0,where);
		rtab = data->subSet(where+1,data->numRows()-1);

		metric->getSplitCriteria(ltab,&leftMean,&leftSS);
		metric->getSplitCriteria(rtab,&rightMean,&rightSS);

		totalSS = leftSS + rightSS;

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

void Node::build(int level)
{
	if (delays == 0) 
	{
		split(level);
	} 
	else 
	{
		Node *lftChild,*rgtChild;
		int cols = data->numCols();
		double bestSS = DBL_MAX;	
		for (int curCol = 1; curCol < cols; curCol++)
		{
			Node *subTree = new Node(NULL,data,yval,dev,0);
			subTree->setDelays(0);
			subTree->setMaxDepth(delays + 1);
			subTree->setId(); 
			subTree->build(0);

			double thisSS = 0;
			subTree->sumLeaves(thisSS);

			if(thisSS < bestSS) 
			{
				double leftMean, leftSS, rightMean, rightSS;
				DataTable *ltab,*rtab;

				ltab = data->subSet(0,subTree->splitIndex);
				rtab = data->subSet(subTree->splitIndex+1,data->numRows()-1);

				metric->getSplitCriteria(ltab,&leftMean,&leftSS);
				metric->getSplitCriteria(rtab,&rightMean,&rightSS);

				lftChild = new Node(this,ltab,leftMean,leftSS,depth+1);
				rgtChild = new Node(this,rtab,rightMean,rightSS,depth+1);
				bestSS = thisSS;
				direction = subTree->direction;
				splitValue = subTree->splitValue;
				splitIndex = subTree->splitIndex;
				varName = data->getName(curCol);
				if(right != NULL)
					delete right;
				right = rgtChild;
				if(left != NULL)
					delete left;
				left = lftChild;
			}
			// delete subTree
		}

		if(left != NULL)
			left->setId();
		if(right != NULL)
			right->setId();

		if((left != NULL)&&(left->data->numRows()>minObs)) {
			left->build(level+1);
		}

		if((right != NULL)&&(right->data->numRows()>minObs)) {
			right->build(level+1);
		}
	}
}

void Node::sumLeaves(double &sum)
{
	if(left == NULL && right == NULL) 
	{
		sum += dev; 
		return;
	}
	left->sumLeaves(sum);
	right->sumLeaves(sum);
}

void Node::print(ofstream &fout)
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

		if (direction < 0) {
			directionStr = "<";
		}
		else {
			directionStr = ">";
		}

		if (right) {
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
			yval << " " << terminalStr << endl;
	}

	if(left != NULL)
		left->print(fout);
	if(right != NULL)
		right->print(fout);

}
