#pragma once
#include <vector>

#include "structures.h"

void makeCpList(node *n, double &parentCp, vector<double> &cpList, int &uniqueCp);
cpTable* makeCpTable(node *n, double parentCp, int nSplit, cpTable *cpTail);