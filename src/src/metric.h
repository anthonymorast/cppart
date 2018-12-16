#ifndef METRIC_H
#define METRIC_H

#include <datatab.h>

// The statisticalMetric class provides statistical functions to be
// used on DataTable objects.

class statisticalMetric{
 public:
    
  // calculate the mean of one of the columns
  double mean(DataTable *data,int column);
  
  // calculate the sum squared difference of one of the columns, given
  // the mean
  double sumSquares(DataTable *data,int column,double colMean);

  // calculate statistical variance of one of the columns
  double variance(DataTable *data,int column);
 
  // calculate stndard deviation of one of the columns
  double stdDev(DataTable *data,int column);

  // Find the best place to split the data.  Assume data is already
  // sorted on the specified column.
  virtual void findSplit(DataTable *data,int col,
			 int &which,int &direction, 
			 double &splitPoint, 
			 double &improve,int minNode);

  virtual void getSplitCriteria(DataTable *data,double *ave, double *cp);

};


class anovaMetric:public statisticalMetric{
 public:

  // Find the best place to split the data.  Assume data is already
  // sorted on the specified column.
  virtual void findSplit(DataTable *data,int col,
			 int &which,int &direction, 
			 double &splitPoint, 
			 double &improve,int minNode);


  virtual void getSplitCriteria(DataTable *data,double *ave, double *cp);

};

class giniMetric:public statisticalMetric{

 double impure(double p){return p * (1.0 - p);}

 public:

  // Find the best place to split the data.  Assume data is already
  // sorted on the specified column.
  virtual void findSplit(DataTable *data,int col,
			 int &which,int &direction, 
			 double &splitPoint, 
			 double &improve,int minNode);


  virtual void getSplitCriteria(DataTable *data,double *ave, double *cp);

};

#endif
