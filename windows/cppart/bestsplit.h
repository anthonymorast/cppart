#pragma once
#include "globals.h"
#include "structures.h"
#include "data_handler.h"
#include "anova.h"
#include <cfloat>

#include <string>

void bestsplit(node *n, params *p, string response, int &numLeft, int &numRight);
void mergeSort(float **x, int low, int high, int varIdx, int colCount, int respCol);
void merge(float **x, int low, int high, int mid, int varIdx, int colCount, int respCol);

void getSplitCounts(float **data, int splitVar, float splitPoint, int direction, int numObs, int &leftCount, int &rightCount);
void splitData(int direction, float splitPoint, int splitVar, int colCount, int numObs, float **left, float **right, float **data);
