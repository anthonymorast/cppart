/*
* Utilities shared by cppart and runmany.
*
* Author: Anthony Morast
*/

#include "utils.h"
#include <fstream>

// returns the number of observations for the first node.
int parseParameters(char * argv[], params *p)
{
	string filename = argv[1];
	string response = argv[2];

	int delayed = 0;
	try {
		delayed = stoi(argv[3]);
	} catch(exception e) {
		cout << "Could not parse integer value from delayed parameter \" " << argv[3] << "\"." << endl;
		printUsage();
		exit(0);
	}

	ifstream fin;
	fin.open(filename);
	if (!fin.is_open()) {
		cout << "File \"" << filename << "\" does not exist." << endl;
		exit(0);
	}

	string headers;
	bool responseFound = false;
	getline(fin, headers);
	string value;
	istringstream ss(headers);
    int varCount = 0;
	while (getline(ss, value, ',')) {
        if(!responseFound) {
	    	responseFound = !strcmp(value.c_str(), response.c_str());
        }
        varCount += 1;
	}
    string *headerValues = new string[varCount];
    varCount = 0;
    istringstream ss2(headers);
	while (getline(ss2, value, ',')) {
        headerValues[varCount] = value;
        varCount += 1;
	}

	if (!responseFound) {
		cout << "Response column not found in dataset, does your data have headers?" << endl;
		exit(0);
	}
	fin.close();

	int lineCount = getLineCount(filename);
	int colCount = getColumnCount(headers);
	p->dataLineCount = lineCount;

	float **data = new float*[lineCount - 1];
	for (int i = 0; i < lineCount; i++) {
		data[i] = new float[colCount];
	}

	getData(filename, response, headers, data);

	p->response = response;
	p->maxDepth = 30;	// only used to set maxNodes
	p->maxNodes = (int)pow(2, (p->maxDepth + 1)) - 1;
	p->minObs = 20;
	p->minNode = 7;
	p->numXval = 10;
	p->iscale = 0;		// this may not be used
	p->delayed = delayed == 1;
	p->data = data;
	p->where = new int[lineCount];
	p->headers = headers;
	p->filename = filename;
    p->varNames = headerValues;

	for (int i = 0; i < lineCount; i++) {
		p->where[i] = 0;	
	}

	return lineCount - 1;
}

node buildTree(params * p, int numObs, int &numNodes)
{
	node tree;
	tree.numObs = numObs;
	tree.data = p->data;

	double risk;
	partition(p, &tree, 1, risk);

	return tree;
}

void fixTree(node * n, float cpScale, int nodeId, int &nodeCount, vector<int> &iNode)
{
	n->cp *= cpScale;
	n->nodeId = nodeId;
	nodeCount += 1;
	iNode.push_back(nodeId);

	if (n->leftNode != NULL) {
		fixTree(n->leftNode, cpScale, 2 * nodeId, nodeCount, iNode);
	}
	if (n->rightNode != NULL) {
		fixTree(n->rightNode, cpScale, (2 * nodeId) + 1, nodeCount, iNode);
	}
}

cpTable *buildCpTable(node *root, params *p)
{
	cpTable *cpTableHead = new cpTable(), *currCpTable, *tempCpTable, *prevCpTable;
	cpTable *cpTail = new cpTable();
	
	cout << "Building CP Table..." << endl;
	vector<double> cpList;
	cpList.push_back(root->cp);
	double parentCp = root->cp;
	int uniqueCp = 2;

	makeCpList(root, parentCp, cpList, uniqueCp);
	sort(cpList.begin(), cpList.end());
	reverse(cpList.begin(), cpList.end());
	p->uniqueCp = cpList.size();

	// make linked list
	cpTableHead->back = new cpTable();
	currCpTable = cpTableHead;
	prevCpTable = cpTableHead->back;
	vector<double>::iterator it;
	for (it = cpList.begin(); it != cpList.end(); it++) {
		double cp = *it;
		currCpTable->cp = (float) cp;
		if (cp != cpList.at(cpList.size() - 1)) {
			currCpTable->forward = new cpTable;
		}
		if (&prevCpTable != NULL) {
			currCpTable->back = prevCpTable;
		}
		prevCpTable = currCpTable;
		currCpTable = currCpTable->forward;
	}
	cpTail = prevCpTable;

	makeCpTable(root, parentCp, 0, cpTail);
	tempCpTable = cpTableHead;

	// cross validations
	if (p->numXval) {
		int *xGrps = new int[root->numObs];

		srand(time(NULL));
		// may need to do some things here where we determine the number of uniqie 
		// xval values in xGrps so we don't get bad things moving forward. 
		for (int i = 0; i < root->numObs; i++) {
			xGrps[i] = rand() % p->numXval;
		}

		xval(cpTableHead, xGrps, *p);
	}

	tempCpTable = cpTableHead;
	float scale = 1 / root->dev;
	while (tempCpTable != NULL) {
		tempCpTable->cp *= scale;
		tempCpTable->risk *= scale;
		tempCpTable->xstd *= scale;
		tempCpTable->xrisk *= scale;

		tempCpTable = tempCpTable->forward;
	}
	cpTableHead->risk = 1.0;

	return cpTableHead;
}
