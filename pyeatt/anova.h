#pragma once
#include "structures.h"
#include "globals.h"

#include <math.h>

double anovaPredict(double y, double yHat);
void anovaSS(double y[], int numValues, double &mean, double &ss);
void anovaSplit(double *x, double *y, params *p, int &which, 
				int &direction, double &splitPoint, double &improve, int numValues);
