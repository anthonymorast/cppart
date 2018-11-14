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
    int delayed;
    bool runXVals;
    bool splitdata;
    string testDataFilename;
    unsigned int numXval;
    double iscale;
    int* where;
    double complexity = 0.01;
  // double alpha = 0.00;
    double** trainData;	// 1 row in the dataframe
    double** testData;
    double** data;
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

/* struct node { */
/*     struct node *rightNode = NULL; */
/*     struct node *leftNode = NULL; */

/*     unsigned int nodeId = 0; */
/*     double splitPoint; */
/*     int index; */
/*     int direction; */
/*     int varIndex = 99999; */
/*     string varName; */

/*     double cp; */
/*     int numObs; */
/*     double dev; */
/*     double yval; */
/*     double improvement = 99999; */

/*     double** data = NULL; */
/*     string response; */
/* }; */

struct cpTable {
    double cp = 0;
    double risk = 0;
    double xrisk = 0;
    double xstd = 0;
    int nsplit = 0;

    struct cpTable *forward = NULL;
    struct cpTable *back = NULL;
};
