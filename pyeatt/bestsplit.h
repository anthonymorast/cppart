#pragma once
#include "globals.h"
#include "structures.h"
#include "data_handler.h"
#include "methods.h"
#include <cdouble>
#include <omp.h>

#include <string>

void bestsplit(node *n, params *p, string response, int &numLeft, int &numRight);
void mergeSort(double *x, double *y, int low, int high, int varIdx, int colCount, int respCol);
void merge(double *x, double *y, int low, int high, int mid, int varIdx, int colCount, int respCol);

void getSplitCounts(double **data, int splitVar, double splitPoint, int direction, int numObs, int &leftCount, int &rightCount);
void splitData(int direction, double splitPoint, int splitVar, int colCount, int numObs, double **left, double **right, double **data);

double getSplitCriteria(methods m, int total, int n, double y[]);
