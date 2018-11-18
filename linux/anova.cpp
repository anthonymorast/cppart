/*
 * Anova operations, calculate SS find best split.
 *
 * Author: Anthony Morast
 */

#include "anova.h"

#include <iostream>

float anovaPredict(float y, float yHat)
{
    float prediction = y - yHat;
    return prediction * prediction;
}

void anovaSS(float y[], int numValues, double &mean, double &ss)
{
    double temp = 0.0;

    // calculate mean (unweighted)
    for (int i = 0; i < numValues; i++) {
        temp += y[i];
    }
    mean = temp / numValues;

    // calculate sum of squares
    ss = 0;
    for (int i = 0; i < numValues; i++) {
        temp = y[i] - mean;
        ss += temp * temp;
    }
}

/*
 *	Returns variables pertaining to where to split the node.
 *
 *	which = where, direction, splitpoint, and improve are passed by reference
 *	so that multiple values can be returned. 
 */
void anovaSplit(float *x, float * y, params *p, int &which,
        int &direction, float &splitPoint, float &improve, int numValues)
{
    double temp;
    double left_sum, right_sum;
    double left_wt, right_wt;
    int left_n, right_n;
    double grandmean, best;
    direction = LEFT;

    double mean, myrisk;
    anovaSS(y, numValues, mean, myrisk); // myrisk == sum_squares 

    right_wt = numValues;
    right_n = numValues;
    right_sum = 0;
    for (int i = 0; i < numValues; i++) {
        right_sum += y[i];
    }

    grandmean = right_sum / right_wt;
    left_sum = 0;
    left_wt = 0;
    left_n = 0;
    right_sum = 0;
    best = 0;
    which = 0;
    for (int i = 0; right_n > p->minNode; i++) {
        left_wt += 1;  // wt[i];
        right_wt -= 1; // wt[i];
        left_n++;
        right_n--;
        temp = (y[i] - grandmean) * 1; // wt[i];
        left_sum += temp;
        right_sum -= temp;
        if (x[i + 1] != x[i] && left_n >= p->minNode) {
            temp = left_sum * left_sum / left_wt +
                right_sum * right_sum / right_wt;
            if (temp > best) {
                best = temp;
                which = i;
                if (left_sum < right_sum)
                    direction = LEFT;
                else
                    direction = RIGHT;
            }
        }
    }
    improve = myrisk == 0 ? 0 : best / myrisk;
    splitPoint = (numValues > which + 1) ? ((x[which] + x[which + 1]) / 2) : x[which];
}
