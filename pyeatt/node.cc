
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
int Node::maxNodes=MAXINT;
double Node::alpha=1750;
int Node::verbose=0;

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
  //int rows = data->numRows();
  //double yBar, deviance;
  
  double bestSS = DBL_MAX;
  // double baseSS = bestSS;
  //double bestImprove = 0;

  DataTable *ltab,*rtab;
  
  Node *lftChild,*rgtChild;

  // check to make sure we are not past max nodes, max depth, min
  // alpha, and return if any of these stopping critera are met.
  
    cout << nodeId << "/" << maxNodes<<" "<<
      data->numRows() << "/" << minObs <<" "<<
      cp << "/" << alpha << " -- " << endl;;
    

  if (nodeId > maxNodes || data->numRows() < minObs || cp <= alpha)
    {
      cout<<"not splitting"<<endl;
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

      //totalSS = metric->getSplitCriteria(data);
      //      if(p->verbose > 1)
      //cout << "totalSS is "<<totalSS<<endl;
      
      // save original improvement measure
      //compImprove = improve;

      // I would have swapped rtab and ltab, but... whatever.
      ltab = data->subSet(0,where);
      rtab = data->subSet(where+1,data->numRows()-1);
      
      metric->getSplitCriteria(ltab,&leftMean,&leftSS);
      metric->getSplitCriteria(rtab,&rightMean,&rightSS);

      totalSS = leftSS + rightSS;

      //      cout << "The SS values are :" << leftSS << " " <<rightSS<<" "<<totalSS<<endl;
      
      //if (improve > 0 && trunc(1000000.*totalSS) < trunc(1000000.*bestSS))
      if ((improve>0) && (totalSS<bestSS) && (leftSS>alpha) && (rightSS>alpha))
	{
	  lftChild = new Node(this,ltab,leftMean,leftSS,depth+1);
	  rgtChild = new Node(this,rtab,rightMean,rightSS,depth+1);
	  bestSS = totalSS;
	  direction = dir;
	  // bestImprove = compImprove;
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

  if(left != NULL)
    left->setId();
  if(right != NULL)
    right->setId();

  if((left != NULL)&&(left->data->numRows()>minObs))
    left->split(level+1);

  if((right != NULL)&&(left->data->numRows()>minObs))
	right->split(level+1);

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


// double getSplitCriteria(methods m, int totalObs, int n, double y[])
// {
//   double value = 0;

//   if(m == ANOVA) {
//     double mean = 0;
//     anovaSS(y, n, mean, value);
//   } else if(m == GINI) {
//     value = giniCalc(n, y);
//   }
//   return value;
// }

// void mergeSort(double *x, double *y, int low, int high, int curCol, int cols) {
//   int mid;
//   if (low < high) {
//     mid = (low + high) / 2;
//     mergeSort(x, y, low, mid, curCol, cols);
//     mergeSort(x, y, mid + 1, high, curCol, cols);

//     merge(x, y, low, high, mid, curCol, cols, respCol);
//   }
// }

// void merge(double *x, double *y, int low, int high, int mid, int curCol, int cols) {
//   int i = low, j = mid + 1, k = 0;
//   const int size = high - low + 1;

//   double *tempx = new double[high - low + 1];
//   double *tempy = new double[high - low + 1];

//   while (i <= mid && j <= high) {
//     if (x[i] < x[j]) {
//       tempx[k] = x[i];
//       tempy[k] = y[i];
//       k++;
//       i++;
//     }
//     else if (x[i] >= x[j]) {
//       tempx[k] = x[j];
//       tempy[k] = y[j];
//       k++;
//       j++;
//     }
//   }
//   while (i <= mid) {
//     tempx[k] = x[i];
//     tempy[k] = y[i];
//     k++;
//     i++;
//   }

//   while (j <= high) {
//     tempx[k] = x[j];
//     tempy[k] = y[j];
//     k++;
//     j++;
//   }
//   for (k = 0, i = low; i <= high; ++i, ++k) {
//     x[i] = tempx[k];
//     y[i] = tempy[k];
//   }

//   free1DData(tempx);
//   free1DData(tempy);
// }

// void getSplitCounts(double ** data, int splitVar, double splitPoint, int direction, int rows, int & leftCount, int & rightCount)
// {
//   if (direction < 0) {
//     for (int i = 0; i < rows; i++) {
//       if (data[i][splitVar] < splitPoint) {
// 	leftCount++;
//       }
//       else {
// 	rightCount++;
//       }
//     }
//   }
//   else {
//     for (int i = 0; i < rows; i++) {
//       if (data[i][splitVar] <= splitPoint) {
// 	rightCount++;
//       }
//       else {
// 	leftCount++;
//       }
//     }
//   }
// }

// void splitData(int direction, double splitPoint, int splitVar, int cols, int rows, double ** left, double ** right, double ** data)
// {
//   int leftCnt = 0, rightCnt = 0;
//   for (int i = 0; i < rows; i++) {
//     if (direction < 0) {
//       if (data[i][splitVar] < splitPoint) {
// 	for (int j = 0; j < cols; j++) {
// 	  left[leftCnt][j] = data[i][j];
// 	}
// 	leftCnt++;
//       }
//       else {
// 	for (int j = 0; j < cols; j++) {
// 	  right[rightCnt][j] = data[i][j];
// 	}
// 	rightCnt++;
//       }
//     }
//     else {
//       if (data[i][splitVar] <= splitPoint) {
// 	for (int j = 0; j < cols; j++) {
// 	  right[rightCnt][j] = data[i][j];
// 	}
// 	rightCnt++;
//       }
//       else {
// 	for (int j = 0; j < cols; j++) {
// 	  left[leftCnt][j] = data[i][j];
// 	}
// 	leftCnt++;
//       }
//     }
//   }
// }
