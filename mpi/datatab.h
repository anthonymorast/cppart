#ifndef DATATAB_H
#define DATATAB_H

#include <time.h>
#include <stdlib.h>

#include <string>
using namespace std;

// This is a class to hold the data for inducing the tree. The
// response variable must always be column zero. The actual data is
// stored in the first table to be created, and then subsets are
// created, using lists of indices.

class DataTable{
  string *names;        // Name for each column
  double **data;       // Pointers to row of data
  int cols;            // Number of columns in data set
  int rows;                   // Number of rows in this subset
  // double **index;             // Indices of the rows in this subset

  void quickSortBy(int col,int left,int right);

 public:
  
  // This constructor is for building the original table
  DataTable(string *column_names,double **rowdata,int nrows, int ncols);
  
  // This constructor is for building subsets by giving a list of indices
  DataTable(DataTable *orig,double **newindex, int nrows);

  ~DataTable();

  /* static void setNames(string *n){names = n;} */
  /* static void setData(double **d){data=d;} */
  /* static void setCols(int numColumns){cols=numColumns;} */

  // sort rows by the values in the specified column
  void getColumnData(int col, double *ndata);
  void sortBy(int col);
  DataTable* subSet(int first,int last);
  DataTable* subSample(int size);

  int numCols(){return cols;} 
  int numRows(){return rows;}

  // get a pointer to one row of data
  double* operator[](int row){return data[row];}
  
  string getName(int col){return names[col];}

  void dump();
  
};

#endif

