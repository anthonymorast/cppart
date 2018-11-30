#include <metric.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <assert.h>
#include <float.h>
#include <map>
using namespace std;

#define LEFT  (-1) 
#define RIGHT  1
#define MISSING 0


// The statisticalMetric class provides statistical functions to be
// used on DataTable objects.

// calculate the mean of one of the columns
double statisticalMetric::mean(DataTable *data,int column)
{
  double tmp=0.0;
  for(int i = 0; i<data->numRows(); i++)
    tmp += (*data)[i][column];
  return tmp/data->numRows();
}
  
// calculate the sum squared difference of one of the columns, given
// the mean
double statisticalMetric::sumSquares(DataTable *data,int column,double colMean)
{
  double tmp;
  double ss = 0.0;
  for(int i = 0; i<data->numRows(); i++)
    {
      tmp = (*data)[i][0] - colMean;
      ss += tmp * tmp;
    }
  return ss;
}

// calculate statistical variance of one of the columns
double statisticalMetric::variance(DataTable *data,int column)
{
  return sumSquares(data,column,mean(data,column))/(data->numRows() - 1);
}

// calculate standard deviation of one of the columns
double statisticalMetric::stdDev(DataTable *data,int column)
{
  return sqrt(variance(data,column));
}
  

void statisticalMetric::findSplit(DataTable *data,int col,
				  int &bestpos,int &direction,
				  double &splitValue,
				  double &improve,int minNode)
{
  cerr<<"Error! findSplit called for statisticalMetric"<<endl;
  exit(2);
}
				  


// Find the best place to split the data.  Assume data is already
// sorted on the specified column.  The goal is to find the split
// point that minimizes v_l + v_r, where v_l is the variance on the
// left lide of the split, and v_r is the variance on the right side
// of the split.
void anovaMetric::findSplit(DataTable *data,int col,
			    int &bestpos, int &direction,
			    double &splitValue,
			    double &improve, int minNode)
{
  double tmp;
  double left_sum, right_sum;
  // double left_wt, right_wt;  // why have extra vars
  int left_n, right_n;
  double grandmean, bestval;
  double originalSumSquares;
  
  assert(minNode > 1);

  direction = LEFT;
  
  // calculate initial mean of all response values
  grandmean = mean(data,0);
  
  // get sum of squares for response variable
  originalSumSquares = sumSquares(data,0,grandmean);

  // Initialize variables.  Assume that the best split is to put
  // everything on the right side of the split
  right_n = data->numRows();
  left_sum =0.0;
  left_n = 0;
  right_sum = 0.0;

  bestval = 0.0;
  bestpos = 0;

  // Move one item at a time to the left and calculate how good that
  // split would be. Track the best split location.  Stop when we get
  // fewer than minNode items on the right.

  for (int i = 0; right_n > minNode; i++)
    {
      // move split point right
      left_n++;
      right_n--;

      // adjust weighted sums
      tmp = (*data)[i][0] - grandmean;
      left_sum += tmp;
      right_sum -= tmp;
     
      if ( // can never happen so don't check i<(data->numRows() - 1) &&
	   (*data)[i+1][col] != (*data)[i][col] &&
	   left_n >= minNode)
	{
	  // calculate the sum of the variances v_l + v_r.  Note:
	  // left_n and right_n must both be greater than 1, which is
	  // guaranteed as long as minNode is greater than one.
	  tmp = (left_sum * left_sum) / left_n +
	        (right_sum * right_sum) / right_n;
	  if (tmp > bestval)
	    {
	      bestval = tmp;
	      bestpos = i;
	      if (left_sum < right_sum)
			direction = LEFT;
	      else
			direction = RIGHT;
	    }
	}
    }
  improve = originalSumSquares == 0.0 ? 0 : bestval / originalSumSquares;
  
  splitValue = (data->numRows() > bestpos + 1) ?
    (((*data)[bestpos][col] + (*data)[bestpos + 1][col]) / 2) :
    (*data)[bestpos][col];
}


void statisticalMetric::getSplitCriteria(DataTable *data,double *ave, double *cp)
{
  cerr<<"Warning statisticalMetric::getSplitCritia was called."<<endl;
  *ave = mean(data,0);
  *cp = sumSquares(data,0,*ave);
}


// formerly known as anovaSS
void anovaMetric::getSplitCriteria(DataTable *data,double *ave, double *cp)
{
  *ave = mean(data,0);
  *cp = sumSquares(data,0,*ave);
}



// Find the best place to split the data.  Assume data is already
// sorted on the specified column.  The goal is to find the split
// point that minimizes v_l + v_r, where v_l is the variance on the
// left lide of the split, and v_r is the variance on the right side
// of the split.
void giniMetric::findSplit(DataTable *data,int col,
			    int &bestpos, int &direction,
			    double &splitValue,
			    double &improve, int minNode)
{
    // int i, j, k;
    // double lwt = 0, rwt = 0;
    // int rtot = 0, ltot = 0;
    // direction = LEFT; 
    // which = 0; // which = where
    // double total_ss, best, temp, pp;
    // double lmean, rmean;

    // // initialize count of class instances left and right of split (all start in right)
    // map<float, int> rightMap, leftMap;
    // for(int i = 0; i < n; i++) {
    //     if(rightMap.count(y[i])) { // if == 1, key is in map
    //     } else {
    //         rightMap.insert(pair<float, int>(y[i], 0));
    //         leftMap.insert(pair<float, int>(y[i], 0));
    //     }
    // }

    // for(i = 0; i < n; i++) {
    //     rwt++; // += aprior[j] * wt[i] => 1*1
    //     rightMap.at(y[i])++; // += wt[i] 
    //     rtot++;
    // }

    // total_ss = 0;
    // for(auto it = rightMap.cbegin(); it != rightMap.cend(); it++) {
    //     temp = 1 * it->second / rwt;
    //     total_ss += rwt * impure(temp);
    // }
    // best = total_ss;

    // for(i = 0; rtot > p->minNode; i++) {
    //     j = y[i];
    //     rwt--; // -= aprior[j] * wt[i]
    //     lwt++; // += aprior[j] * wt[i]
    //     rtot--;
    //     ltot++;
    //     rightMap.at(j)--; // -= wt[i]
    //     leftMap.at(j)++; // += wt[i]
    //     if (ltot >= p->minNode && x[i+1] != x[i]) {
    //         temp = 0;
    //         lmean = 0;
    //         rmean = 0;
    //         j = 0;
    //         for(auto right = rightMap.cbegin(), left = leftMap.cbegin(); 
    //                 right != rightMap.cend() && left != leftMap.cend();  right++, left++) {
    //             // key order should always be the same for std::map structures
    //             pp = 1 * left->second / lwt; // aprior[j] * left[j] / lwt
    //             temp += lwt * impure(pp);
    //             lmean += pp * j; 
    //             pp = 1 * right->second / rwt; // aprior[j] * right[j] / rwt
    //             temp += rwt * impure(pp);
    //             rmean += pp * j;
    //             j++;
    //         }
    //         if (temp < best) {
    //             best = temp;
    //             which = i;
    //             direction = lmean < rmean ? LEFT : RIGHT;
    //         }
    //     }
    // }

    // improve = total_ss - best;
    // splitPoint = (n > which + 1) ? ((x[which] + x[which + 1]) / 2) : x[which];
}



// formerly known as giniCalc
void giniMetric::getSplitCriteria(DataTable *data,double *cls, double *cp)
{

    double gini = 0;

    map<double, int> freqs; // map class label to occurrence frequency
    for(int i = 0; i < data->numRows(); i++) {
      if(freqs.count((*data)[i][0])) { // if == 1, key is in map
            freqs.at((*data)[i][0])++; 
        } else {
            freqs.insert(pair<double, int>((*data)[i][0], 1));
        }
    }

    double temp;
    for(auto it = freqs.cbegin(); it != freqs.cend(); it++) {
        //gini += ((it->second/(double)n) * (it->second/(double)n)); // gini = 1 - sum(freq_j / total obs);
      temp = it->second / (double)data->numRows();
      gini += (double)data->numRows() * impure(temp);
    }
    *cls = 0.0;
    *cp = gini;
}
  

