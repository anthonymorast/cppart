/*
* Entry point of the program. Parses parameters, does some preprocessing,
* creates the tree, runs cross-validations, creates tree files, etc.
*
* Author: Anthony Morast
*/


#include "cppart.h"

#include <fstream>

int main(int argc, char* argv[]) {
	clock_t start, end;

	start = clock();

	if (argc < 4) {
		printUsage();
		exit(0);
	}

	params p;
	int numObs = parseParameters(argv, &p);
	int numNodes = 0;
        cout << "Building tree..." << endl;
	node root = buildTree(&p, numObs, numNodes);
        cpTable *cpTableHead = buildCpTable(&root, &p);
	
	vector<int> iNode;
	int count = 0;
	fixTree(&root, (1 / root.dev), 1, count, iNode);

	int idx = p.filename.find(".");
	string treeFileName = p.filename.substr(0, idx);
	if(p.delayed) {
		treeFileName += ".delayed";
	}
	treeFileName += ".tree";
	printTree(&root, treeFileName);

	end = clock();
	cout << "Time elapsed " << ((float)(end - start)) / CLOCKS_PER_SEC << endl;

	return 0;
}

void printUsage()
{
	cout << "Usage:" << endl;
	cout << "\tcppart.exe <data filename> <response> <delayed>" << endl;
}
