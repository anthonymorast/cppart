#pragma once
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

#include "structures.h"

void printTree(node *root, string filename);
void printSubTree(node *n, int nodeId, int myDepth, node *parent, bool right, ofstream &fout);
void printNode(node *n, int nodeId, int myDepth, node *parent, bool right, ofstream &fout);