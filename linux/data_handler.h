#pragma once
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>

using namespace std;

/*
*	Fucntion Headers
*/
void getData(string filename, string response, string headers, float** x);
float* getResponseData(string response, string headers, float** data, int numObs);
float *getExplanatoryDataCol(string response, string headers, float **data, int numObs, int col);
float getMean(float **data, string response, string headers, int numObs);
int getResponseColumnNumber(string response, string headers);
int getLineCount(string filename);
int getColumnCount(string headers);
