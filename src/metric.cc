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
    int i, j, n = data->numRows();
    double rtot = 0, ltot = 0;
    direction = LEFT; 
    bestpos = 0; // which = where
    double total_ss, best, temp, pp;
    double lmean, rmean;

    // initialize count of class instances left and right of split (all start in right)
    map<float, int> rightMap, leftMap;
    for(i = 0; i < n; i++) {
		double y = (*data)[i][0];
        if(rightMap.count(y)) { // if == 1, key is in map
        } else {
            rightMap.insert(pair<float, int>(y, 0));
            leftMap.insert(pair<float, int>(y, 0));
        }
    }

    for(i = 0; i < n; i++) {
		double y = (*data)[i][0];
        rightMap.at(y)++; // += wt[i] 
        rtot++;
    }

    total_ss = 0;
    for(auto it = rightMap.cbegin(); it != rightMap.cend(); it++) {
        temp = 1 * it->second / rtot;
        total_ss += rtot * impure(temp);
    }
    best = total_ss;
	for(i = 0; rtot > minNode; i++) {
		double y = (*data)[i][0];
		double x = (*data)[i][col];
		double x2 = (*data)[i+1][col];

        rtot--;
        ltot++;
        rightMap.at(y)--; // -= wt[i]
        leftMap.at(y)++; // += wt[i]
        if (ltot >= minNode && x2 != x) {
            temp = 0;
            lmean = 0;
            rmean = 0;
            j = 0;
            for(auto right = rightMap.cbegin(), left = leftMap.cbegin(); 
                    right != rightMap.cend() && left != leftMap.cend();  right++, left++) {
                // key order should always be the same for std::map structures
                pp = 1 * left->second / ltot; // aprior[j] * left[j] / lwt
                temp += ltot * impure(pp);
                lmean += pp * j; 
                pp = 1 * right->second / rtot; // aprior[j] * right[j] / rwt
                temp += rtot * impure(pp);
                rmean += pp * j;
                j++;
            }
            if (temp < best) {
                best = temp;
                bestpos = i;
                direction = lmean < rmean ? LEFT : RIGHT;
            }
        }
    }

    improve = total_ss - best;
    splitValue = (n > bestpos + 1) ? (((*data)[bestpos][col] + (*data)[bestpos+1][col]) / 2) : (*data)[bestpos][col];
}



// formerly known as giniCalc and giniDev
void giniMetric::getSplitCriteria(DataTable *data,double *cls, double *cp)
{
	// get majority vote
	map<float, int> classFreq;
	for(int i = 0; i < data->numRows(); i++)
	{
		double y = (*data)[i][0];
		if(classFreq.count(y)) 
			classFreq.at(y)++;
		else
			classFreq.insert(pair<float, int>(y, 1));
	}

	// find majority vote and deviance for splitting
	int max = 0;
	double temp;
	double n = (double)data->numRows();
	*cp = 0;
	for(auto it = classFreq.cbegin(); it != classFreq.cend(); it++)
	{
		// if most votes, choose it
		if(it->second > max)
		{
			*cls = it->first;
			max = it->second;
		}
		
		temp = it->second / n;
		*cp += n * impure(temp);
	}
}

/*
void giniMetric::giniDev(DataTable *data, double *cls, double *cp)
{
	// majority vote for class (cls)
	map<float, int> classFreq;
	for(int i = 0; i < data->numRows(); i++)
	{
		double y = (*data)[i][0];
		if(classFreq.count(y)) 
			classFreq.at(y)++;
		else
			classFreq.insert(pair<float, int>(y, 1));
	}

	int max = 0;
	for(auto it = classFreq.cbegin(); it != classFreq.cend(); it++)
	{
		if(it->second > max)
		{
			*cls = it->first;
			max = it->second;
		}
	}

	*cp = 0;
	int numClasses = classFreq.size();
	if (data->numRows() == 1280)
	cout << numClasses << endl;
	for(int i = 0; i < numClasses; i++)
	{
		int temp = 0;
		int j = 0;
		for(auto it = classFreq.cbegin(); it != classFreq.cend(); it++)
		{
			int loss_mult = j == i ? 0 : 1;
			temp += (it->second * loss_mult);
			j++;
		}
		if(i == 0 || temp < *cp) 
		{
			max = i;
			*cp = temp;
		}
	}
}
*/
