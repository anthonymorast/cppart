#pragma once
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>

#include <structures.h>

using namespace std;

/*
*	Fucntion Headers
*/
void getData(string filename, double** x);

double* getResponseData(string response, string headers, double** data, int numObs);
double *getExplanatoryDataCol(string response, string headers, double **data, int numObs, int col);
double getMean(double **data, string response, string headers, int numObs);
int getResponseColumnNumber(string response, string headers);
int getLineCount(string filename);
int getColumnCount(string headers);

//void free1DData(double* y);
//void free2DData(double **data, int numObs);

//void freeTreeData(node *root);

double** deepCopyData(double** data, int numObs, int colCount);
