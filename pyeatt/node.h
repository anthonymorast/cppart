#ifndef NODE_H
#define NODE_H

#include <metric.h>
#include <datatab.h>
#include <string>

#include <iostream>
using namespace std;

class Node{
  static statisticalMetric *metric; // Statistical metric being used
  static int minObs;        // minimum amount of data required in a leaf node.
  static int nextId;
  static int maxNodes;
  static int verbose;
  static double alpha;
  DataTable *data;           // Training samples covered by this node.
  int splitIndex;            // Set to -1 for leaf node.
  int direction;
  double splitValue;         // Contains response value for leaf nodes.
  double dev,yval,cp;
  int depth;
  int nodeId;
  string varName;
  Node *left, *right, *parent;        // Both NULL for leaf node.
public:
  Node(Node *Parent, DataTable *d,double mean,double Cp,int level);
  ~Node();

  void setId(){nodeId = nextId++;}

  void split(int level);
  
  static void setMetric(statisticalMetric *m){metric = m;}
  static void setMinNodeData(int min){minObs = min;}
  static void setMaxNodes(int max){maxNodes = max;}
  static void setVerbose(int vlevel){verbose = vlevel;}
  static void setAlpha(double a){alpha = a;}
  
  void print(ofstream &fout);

};

class InteriorNode:public Node{
  
  
};

class LeafNode:public Node{
  // returns 0 if the split did not occur.
  
};


#endif