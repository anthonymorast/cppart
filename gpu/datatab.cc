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
    // #pragma omp parallel shared(data,col)
    // #pragma omp single nowait
    quickSortBy(col,0,rows-1);
}

// Sort rows by the values in the specified column.
//
// Choosing pivot by median of three is slower on average in this
// application, so just use middle value to avoid worst-case. -- Larry.
//
// This sort is about 5% faster than the old sort commented out below.
//
// Support OpenMP. Quicksort is ``embarassingly parallel.'' -- Larry




void DataTable::quickSortBy(int col,int first,int last)
{
  // Single or zero item case. Using this is 0.5% to 1% slower
  // on average in this application - Larry
  // 
  // if(first>=last) return;
  
  // Two item case. This gives us a speedup of about 1% to 2% - Larry
  if(last==first+1) 
    {
      if(data[first][col] > data[last][col])
	{
	  double *tmp = data[first];
	  data[first]=data[last];
	  data[last]=tmp;
	}
      return;
    }
  
  // // Use Shell or insertion sort for short arrays -- not
  // // implemented. I am guessing that a good value for quickSortMin is:
  // // 10 <= quickSortMin <= 1000 ... have to experiment with some
  // // values. -- Larry
  // if(last-first < quickSortMin)
  // {
  //   shellSortBy(int col,int first,int last);
  //   return;
  // }
  
  int left,right,pivotloc;
  double pivotval,*tmp;
  
  pivotloc = (first+last)/2;
  pivotval= data[pivotloc][col];

  // move pivot to first
  if(first!=pivotloc)
  {
    tmp = data[first];
    data[first]=data[pivotloc];
    data[pivotloc]=tmp;
  }
  
  // partition remainder
  left=first+1;
  right=last;
  while(left <= right)
    {
      while((left <= right) && (data[left][col] <= pivotval))
	left++;
      while((left <= right) && (data[right][col] > pivotval))
	right--;
      if(left < right)
	{
	  tmp = data[left];
	  data[left]=data[right];
	  data[right]=tmp;
	  left++;
	  right--;
	}
    }
  // put pivot where it belongs
  tmp = data[first];
  data[first]=data[right];
  data[right]=tmp;

  // recurse

  if(first<right-1)
    quickSortBy(col,first,right-1);  
  if(left<last)
    quickSortBy(col,left,last);

  // the following code could be used if you have LOTS of data
  // #define PARALLEL_THRESHOLD 5000
  // int leftsize = right-first-1;
  // int rightsize = last-left;
  //  if(( last - first) > PARALLEL_THRESHOLD)
  // #pragma omp parallel sections
  //    {
  // #pragma omp section
  // 	quickSortBy(col,first,right-1);
  // #pragma omp section
  // 	quickSortBy(col,left,last);
  //    }
  //  else 
  //    {
  // 	quickSortBy(col,first,right-1);
  // 	quickSortBy(col,left,last);
  //    }

}



// void DataTable::quickSortBy(int col,int first,int last)
// {
//   // Single or zero item case. Using this is 0.5% to 1% slower
//   // on average in this application - Larry
//   // 
//   // if(first>=last) return;
  
//   // two item case
//   if(last==first+1) 
//     {
//       if(data[first][col] > data[last][col])
// 	{
// 	  double *tmp = data[first];
// 	  data[first]=data[last];
// 	  data[last]=tmp;
// 	}
//       return;
//     }

//   // Use Shell or insertion sort for short arrays -- not implemented
//   // if(last-first < quickSortMin) // Guess: 10 <= quickSortMin <= 1000
//   // {
//   //   shellSortBy(int col,int first,int last);
//   //   return;
//   // }

//   int left = first;
//   int right = last;
//   int pivotloc = (left+right)/2;
//   double pivotval = data[pivotloc][col];
//   double *tmp;
//   while(left <= right)
//     {
//       while((left <= right) && (data[left][col] < pivotval))
// 	left++;
//       while((left <= right) && (data[right][col] > pivotval))
// 	right--;
//       if(left <= right)
// 	{
// 	  tmp = data[left];
// 	  data[left]=data[right];
// 	  data[right]=tmp;
// 	  left++;
// 	  right--;
// 	}
//     }
//   if(first<right)
//     quickSortBy(col,first,right);
//   if(left<last)
//     quickSortBy(col,left,last);
// }

DataTable* DataTable::subSet(int first,int last)
{
    //cout<< "getting subset from "<<first<<" to "<<last<<endl;
    int nrows = last-first+1;
    double **newindex = new double*[nrows];
    memcpy(newindex,&(data[first]),nrows*sizeof(double*));
    return new DataTable(this,newindex,nrows);
}

DataTable* DataTable::subSample(int size) 
{
	if(size > numRows())
	{
		cout << "Too few observations to sub-sample " << size << " rows." << endl;
		exit(0);
	}

	double **rows = new double*[size];
	for(int i = 0; i < size; i++)
	{
		int idx = rand() % numRows(); // generate random idx with replacement
		rows[i] = data[idx];
	}
	return new DataTable(names, rows, size, cols);
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
