#include <datatab.h>
#include <cstring>
#include <iostream>

using namespace std;


// declare and initialize static data members
// int DataTable::cols=0;            // Number of columns in data set
// string *DataTable::names=NULL;        // Name for each column
// double **DataTable::data=NULL;       // Pointers to row of data


// This constructor is for building the original table
DataTable::DataTable(string *column_names,double **rowdata,int nrows, int ncols)
  {
    names = column_names;
    data = rowdata;
    cols = ncols;
    rows = nrows;
    //   index = new double*[nrows];
    // for(int i=0;i<nrows;i++)
    // index[i]=data[i];
  }
  
  
  // This constructor is for building subsets by giving a list of indices
DataTable::DataTable(DataTable *orig,double **newindex, int nrows)
  {
    cols = orig->cols;
    names = orig->names;
    rows = nrows;
    data = newindex;
  }

DataTable::~DataTable()
  {
    delete[] data;
  }

// sort rows by the values in the specified column
void DataTable::sortBy(int col)
{
  if(rows>0)
    quickSortBy(col,0,rows-1);
}



// sort rows by the values in the specified column
void DataTable::quickSortBy(int col,int left,int right)
{
  int first = left;
  int last = right;
  int pivotloc = (left+right)/2;
  double pivotval = data[pivotloc][col];
  double *tmp;
  while(left <= right)
    {
      while((left <= right) && (data[left][col] < pivotval))
	left++;
      while((left <= right) && (data[right][col] > pivotval))
	right--;
      if(left <= right)
	{
	  tmp = data[left];
	  data[left] = data[right];
	  data[right] = tmp;
	  left++;
	  right--;
	}
    }
  if(first<right)
    quickSortBy(col,first,right);
  if(left<last)
    quickSortBy(col,left,last);
}

DataTable*  DataTable::subSet(int first,int last)
{
  //cout<< "getting subset from "<<first<<" to "<<last<<endl;
  int nrows = last-first+1;
  double **newindex = new double*[nrows];
  memcpy(newindex,&(data[first]),nrows*sizeof(double*));
  return new DataTable(this,newindex,nrows);
}



void DataTable::dump()
{
  int i,j;
  for(i=0;i<cols;i++)
    cout <<names[i]<<" ";
  cout <<endl;
  for(j=0;j<rows;j++)
    {
      for(i=0;i<cols;i++)
	cout <<data[j][i]<<" ";
      cout <<endl;
    }

}
