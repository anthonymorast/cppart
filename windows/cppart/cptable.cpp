/*
* Creates the CP Table used for pruning/calculating R^2.
*
* Author: Anthony Morast
*/


#include "cptable.h"

#include <iostream>

void makeCpList(node *n, double &parentCp, vector<double> &cpList, int & uniqueCp)
{
	if (n->cp > parentCp) {
		n->cp = parentCp;
	}

	double myCp = n->cp;
	if (myCp < 0) {
		myCp = 0;
	}

	if (n->leftNode != NULL) {
		makeCpList(n->leftNode, myCp, cpList, uniqueCp);
		makeCpList(n->rightNode, myCp, cpList, uniqueCp);
	}

	if (myCp < parentCp) {
		if (find(cpList.begin(), cpList.end(), myCp) != cpList.end()) {
			return;
		}
		cpList.push_back(myCp);
		uniqueCp++;
	}
}

cpTable *makeCpTable(node *n, double parentCp, int nSplit, cpTable *cpTail)
{
	cpTable *table;
	if (n->leftNode != NULL) {
		makeCpTable(n->leftNode, n->cp, 0, cpTail);
		table = makeCpTable(n->rightNode, n->cp, nSplit + 1, cpTail);
	}
	else {
		table = cpTail;
	}

	while (table->cp < parentCp) {
		table->risk += n->dev;
		table->nsplit += nSplit;
		table = table->back;
	}

	return table;
}
