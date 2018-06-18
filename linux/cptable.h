#pragma once
#include <vector>
#include <algorithm>

#include "structures.h"

void makeCpList(node *n, double &parentCp, vector<double> &cpList, int &uniqueCp, params *p);
cpTable* makeCpTable(node *n, double parentCp, int nSplit, cpTable *cpTail);
