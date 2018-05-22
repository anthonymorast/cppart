#include <iostream>  
#include <string> 
#include <fstream> 
#include <sstream> 
#include <math.h> 
#include <vector> 
#include <algorithm> 
#include <time.h> 

#include "structures.h"
#include "data_handler.h"
#include "partition.h"
#include "cptable.h"
#include "xval.h"
#include "print_tree.h"

using namespace std;

int parseParameters(char* argv[], params *p); 
node buildTree(params *p, int, int &numNodes); 
void fixTree(node *root, float cpScale, int nodeId, int &nodeCount, vector<int> &iNode); 
cpTable *buildCpTable(node *root, params *p);
void printUsage(); 
