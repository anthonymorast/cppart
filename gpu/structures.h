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

struct mpi_send {
	int column;
	int nrows;
};

struct mpi_resp {
	int column;
	double sse;
	double improve;
    double leftSS;
    double rightSS;
};

struct cpTable {
    double cp = 0;
    double risk = 0;
    double xrisk = 0;
    double xstd = 0;
    int nsplit = 0;

    struct cpTable *forward = NULL;
    struct cpTable *back = NULL;
};
