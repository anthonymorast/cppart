#pragma once
#include "globals.h"
#include "structures.h"
#include "data_handler.h"
#include "anova.h"

#include <string>

void bestsplit(node *n, params *p, string response, int &numLeft, int &numRight);
void mergeSort(float **x, int low, int high, int varIdx, int colCount, int respCol);
void merge(float **x, int low, int high, int mid, int varIdx, int colCount, int respCol);