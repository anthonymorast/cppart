#pragma once
#include <string>

using namespace std;

/*
 *	Struct and class definitions that are not used in any particular functionality.
 */
enum methods { GINI = 0, ANOVA = 1 };

struct params {
    unsigned int maxNodes;
    unsigned int minObs;
    string response;
    string headers;
    unsigned int minNode;	// minimum number of obs for terminal node
    unsigned int maxDepth;
    bool delayed;
    bool runXVals;
    bool splitdata;
    string testDataFilename;
    unsigned int numXval;
    float iscale;
    int* where;
    float complexity = 0.01;
    float alpha = 0.00;
    float** trainData;	// 1 row in the dataframe
    float** testData;
    float** data;
    int testSize;
    int trainSize;
    int uniqueCp;
    int dataLineCount;
    string filename;
    string *varNames;
    int verbose = 0;

    int numclasses = 0;
    methods method = ANOVA;
};

struct node {
    struct node *rightNode = NULL;
    struct node *leftNode = NULL;

    unsigned int nodeId = 0;
    float splitPoint;
    int index;
    int direction;
    int varIndex = 99999;
    string varName;

    float cp;
    int numObs;
    float dev;
    float yval;
    float improvement = 99999;

    float** data = NULL;
    string response;
};

struct cpTable {
    float cp = 0;
    float risk = 0;
    float xrisk = 0;
    float xstd = 0;
    int nsplit = 0;

    struct cpTable *forward = NULL;
    struct cpTable *back = NULL;
};
