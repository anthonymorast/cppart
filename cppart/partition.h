#pragma once
#include "structures.h"
#include "anova.h"
#include "bestsplit.h"
#include "data_handler.h"

#include <iostream>
using namespace std;

/*
*	Function Definitions
*/
int partition(params *p, node* root, int nodeNum, double &sumrisk);