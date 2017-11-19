#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>

#include "data_handler.h"

using namespace std;

/*
*	Global definitions
*/
#define LEFT  (-1) 
#define RIGHT  1
#define MISSING 0


/*
*	Struct and class definitions that are not used in any particular functionality.
*/
struct params {
	unsigned int maxNodes;
	unsigned int minObs;
	string response;
	unsigned int minNode;	// minimum number of obs for terminal node
	unsigned int maxDepth;
	bool delayed;
	unsigned int numXval;
	float iscale;
	int* where;

	// x[*][i] should correspond with y[i] at all times
	float* y;	// 1 value per response (regression only)
	float** x;	// 1 row in the dataframe
};

struct node {
	struct node *rightNode;
	struct node *leftNode;

	unsigned int nodeId = 0;
	float splitPoint;
	int index;
	int direction;
	int varIndex;

	float cp;
	int numObs;
	float dev;
	float yval;
	float improvement;

	float** nodeData;
	string response;
};

struct cpTable {
	float cp;
	float risk;
	float xrisk;
	float xstd;
	int nsplit;

	struct cpTable *forward;
	struct cpTable *back;
};


/*
*	Function Definition
*/
void printUsage();
int parseParameters(char* argv[], params *p);
node buildTree(params *p);
void fixTree(node *root, float cpScale, int nodeId, int nodeCount, int iNode);
cpTable buildCpTable(node *root, params *p);
