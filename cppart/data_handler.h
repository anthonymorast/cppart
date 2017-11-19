#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

/*
*	Fucntion Headers
*/
void getXData(string filename, string response, string headers, float** x);
void getYData(string filename, string response, string headers, float y[]);
int getResponseColumnNumber(string response, string headers);
int getLineCount(string filename);
int getColumnCount(string headers);