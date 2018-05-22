/*
* Runs the cross-validations many times after building the tree
* so that an average R^2 value can be found. 
*
* Author: Anthony Morast
*/

// Includes
#include "utils.h" 		// parse params, build tree, xvals, tons of other goodies
#include <pthread.h>	// run many cross-validations at once.

// Function Definitions
node* copyTree(node *root);


void printUsage() {
	cout << "Usage:" << endl;
	cout << "\t./runmany <data filename> <response> <delayed {0,1}> <number iterations>" << endl;
}

int main(int argc, char *argv[]) {
	clock_t start, end;
	start = clock(); // may need to change for threading

	if(argc < 5) {
		printUsage();
		return 0;
	}
	params p;
	int numObs = parseParameters(argv, &p);
	int numNodes = 0;
	node root = buildTree(&p, numObs, numNodes);

	float avgRSquared = 0;
	int iters = stoi(argv[4]);

	cout << root.index << endl;
	node test = root;
	test.index = 100000;
	cout << root.index << " " << test.index<< endl;

	for(int i = 0; i < iters; i++) {
		
		cpTable *cpTableHead = buildCpTable(&root, &p);
		
	}
	
	return 0;
}
