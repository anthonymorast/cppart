#pragma once
#include "structures.h"
#include "globals.h"
#include "data_handler.h"

#include <iostream>
#include <math.h>
#include <map>

int giniPredict(float y, float yHat);
double giniCalc(int n, float y[]); 
double impure(double p);
void giniDev(float y[], int numValues, double &classification, double &deviance);
void giniSplit(float *x, float *y, params *p, int &which, 
				int &direction, float &splitPoint, float &improve, int numValues);
