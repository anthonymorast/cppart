#pragma once
#include "structures.h"
#include "globals.h"
#include "data_handler.h"

#include <iostream>
#include <math.h>
#include <map>

int giniPredict(double y, double yHat);
double giniCalc(int n, double y[]); 
double impure(double p);
void giniDev(double y[], int numValues, double &classification, double &deviance);
void giniSplit(double *x, double *y, params *p, int &which, 
				int &direction, double &splitPoint, double &improve, int numValues);
