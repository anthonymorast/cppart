#include <iostream>  
#include <string> 
#include <fstream> 
#include <sstream> 
#include <math.h> 
#include <vector> 
#include <algorithm> 
#include <time.h> 
#include <stdlib.h>
#include <algorithm>

#include "structures.h"
/* #include "data_handler.h" */
/* #include "partition.h" */
/* #include "cptable.h" */
/* #include "xval.h" */
/* #include "print_tree.h" */

using namespace std;

// Parses the command line parameters and read data
void parseParameters(char* argv[], int argc, params *p);


void printUsage(char** argv, int argc);

/* node buildTree(params *p, int, int &numNodes);  */
/* void fixTree(node *root, double cpScale, int nodeId, int &nodeCount, vector<int> &iNode);  */
/* cpTable *buildCpTable(node *root, params *p); */
/* double getPrediction(node* tree, double row[], int responseCol); */
