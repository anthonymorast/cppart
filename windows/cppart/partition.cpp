/*
* Partitions the dataset by finding the bestsplit and doing 
* a some operations and error checking with the cp value
* and bad splits.
*
* Author: Anthony Morast
*/


#include "partition.h"

/*
*	Function Implementations
*/
int partition(params *p, node* n, int nodeNum, double &sumrisk) {
	float **data = n->data;
	float *y = getResponseData(p->response, p->headers, data, n->numObs);

	double tempcp, mean;
	anovaSS(y, n->numObs, mean, tempcp);
	n->dev = tempcp;
        free1DData(y);
	
	if (nodeNum == 1) {
		n->cp = n->dev;
	}
	if (nodeNum > 1 && tempcp > n->cp) {
		tempcp = n->cp;
	}

	// can we stop?
	if (nodeNum > p->maxNodes || n->numObs < p->minObs || tempcp <= 0) {
		n->leftNode = NULL;
		n->rightNode = NULL;
		n->yval = mean;
		n->dev = tempcp;
		n->cp = 0; 
		return 0;
	}

	int numleft = 0, numright = 0;
	bestsplit(n, p, n->response, numleft, numright);


	// update where 
	// TODO: probably don't need where since we do R^2 stuff ourselves, hold off til end

	double leftRisk, rightRisk;
	int leftSplits, rightSplits;

	n->leftNode->numObs = numleft;
	n->leftNode->response = p->response;
	n->leftNode->nodeId = nodeNum * 2;
	n->leftNode->cp = tempcp;
	leftSplits = partition(p, n->leftNode, (2 * nodeNum), leftRisk);

	// update cp
	tempcp = (n->dev - leftRisk) / (leftSplits + 1);
	double tempcp2 = (n->dev - n->leftNode->dev);
	if (tempcp < tempcp2) {
		tempcp = tempcp2;
	}
	if (tempcp > n->cp) {
		tempcp = n->cp;
	}

	n->rightNode->numObs = numright;
	n->rightNode->response = p->response;
	n->rightNode->nodeId = (nodeNum * 2) + 1;
	n->rightNode->cp = tempcp;
	rightSplits = partition(p, n->rightNode, (2 * nodeNum) + 1, rightRisk);

	tempcp = (n->dev - (leftRisk + rightRisk)) / (leftSplits + rightSplits + 1);
	if (n->rightNode->cp > n->leftNode->cp) {
		if (tempcp > n->leftNode->cp) {
			leftRisk = n->leftNode->dev;
			leftSplits = 0;
		}
		
		tempcp = (n->dev - (leftRisk + rightRisk)) / (leftSplits + rightSplits + 1);
		if (tempcp > n->rightNode->cp) {
			rightRisk = n->rightNode->dev;
			rightSplits = 0;
		}
	}
	else if (tempcp > n->rightNode->cp) {
		rightSplits = 0;
		rightRisk = n->rightNode->dev;
		tempcp = (n->dev - (leftRisk + rightRisk)) / (leftSplits + rightSplits + 1);
		if (tempcp > n->leftNode->cp) {
			leftRisk = n->leftNode->dev;
			leftSplits = 0;
		}
	}

	// cp and node ids for python is REALLY close, i assum straglers are from the mergesort
	n->cp = (n->dev - (leftRisk + rightRisk)) / (leftSplits + rightSplits + 1);
	sumrisk = leftRisk + rightRisk;

	return leftSplits + rightSplits + 1;
}
