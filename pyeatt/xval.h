#pragma once
#include <random>
#include <string>
#include <vector>
#include <iostream>
#include <time.h>

#include "structures.h"
#include "methods.h"
#include "data_handler.h"
#include "partition.h"

using namespace std;

void xval(cpTable *tableHead, int xGroups[], params p);
void rundown(node *tree, double row[], double cpList[], double xtemp[], double xpred[], int uniqueCp, int responseCol);
node *branch(node *tree, double row[]);
void fixCp(node *n, double parentCp);

int getXGroupSize(int xGrps[], int group, int numObs);
void getGroupData(double **data, double **groupData, double **holdout, int xGrps[], int group, int colCount, int numObs);
