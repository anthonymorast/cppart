#pragma once
#include <random>
#include <string>
#include <vector>
#include <iostream>
#include <time.h>

#include "structures.h"
#include "anova.h"
#include "data_handler.h"
#include "partition.h"

using namespace std;

void xval(cpTable *tableHead, int xGroups[], params p);
void rundown(node *tree, float row[], float cpList[], float xtemp[], float xpred[], int uniqueCp, int responseCol);
node *branch(node *tree, float row[]);
void fixCp(node *n, float parentCp);

int getXGroupSize(int xGrps[], int group, int numObs);
void getGroupData(float **data, float **groupData, float **holdout, int xGrps[], int group, int colCount, int numObs);
