#pragma once
#include "structures.h"
#include "methods.h"
#include "bestsplit.h"
#include "data_handler.h"

#include <iostream>
using namespace std;

/*
*	Function Definitions
*/
int partition(params *p, node* root, long nodeNum, double &sumrisk);
