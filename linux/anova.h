#pragma once
#include "structures.h"
#include "globals.h"

#include <math.h>

float anovaPredict(float y, float yHat);
void anovaSS(float y[], int numValues, double &mean, double &ss);
void anovaSplit(float *x, float *y, params *p, int varIdx, int &which, 
				int &direction, float &splitPoint, float &improve, int numValues);