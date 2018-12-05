#include "gini.h"

double impure(double p) {
    return p * (1.0 - p);
}

int giniPredict(float y, float yHat) {
    cout << "Not implemented..." << endl;
    exit(0);
}

void giniDev(float y[], int numValues, double &classification, double &deviance) {
    // majority vote (rpart does some stuff with loss and priors, may need updating)
    map<float, int> classFreq; // map class label to occurrence frequency
    for(int i = 0; i < numValues; i++) {
        if(classFreq.count(y[i])) { // if == 1, key is in map
            classFreq.at(y[i])++; 
        } else {
            classFreq.insert(pair<float, int>(y[i], 1));
        }
    }
    int max = 0;
    for(auto it = classFreq.cbegin(); it != classFreq.cend(); it++) {
        if(it->second > max) {
            classification = it->first;
            max = it->second;
        }
    }

    // get risk (stolen again from rpart, for the most part)
    // loss = hollow matrix with 1's upper and lower triangles of size numclass x numclass
    deviance = 0;
    int numclasses = classFreq.size();
    for (int i = 0; i < numclasses; i++) {
        int temp = 0;
        int j = 0;
        for(auto it = classFreq.cbegin(); it != classFreq.cend(); it++) {
            int loss_mult = j == i ? 0 : 1; // on diagonal = 0, otherwise 1
            temp += (it->second * loss_mult);
            j++;
        }
        if(i == 0 || temp < deviance) {
            max = i;
            deviance = temp;
        }
    }
}

double giniCalc(int n, float y[]) {
    double gini = 0;

    map<float, int> freqs; // map class label to occurrence frequency
    for(int i = 0; i < n; i++) {
        if(freqs.count(y[i])) { // if == 1, key is in map
            freqs.at(y[i])++; 
        } else {
            freqs.insert(pair<float, int>(y[i], 1));
        }
    }

    double temp;
    for(auto it = freqs.cbegin(); it != freqs.cend(); it++) {
        //gini += ((it->second/(double)n) * (it->second/(double)n)); // gini = 1 - sum(freq_j / total obs);
        temp = it->second / (double)n;
        gini += (double)n * impure(temp);
    }
    return gini;
}

// ripped off directly from rpart
void giniSplit(float *x, float *y, params *p, int &which, 
				int &direction, float &splitPoint, float &improve, int n) {
    // in rpart prior[i] = aprior[i] = 0 for all i
    int i, j, k;
    double lwt = 0, rwt = 0;
    int rtot = 0, ltot = 0;
    direction = LEFT; 
    which = 0; // which = where
    double total_ss, best, temp, pp;
    double lmean, rmean;

    // initialize count of class instances left and right of split (all start in right)
    map<float, int> rightMap, leftMap;
    for(int i = 0; i < n; i++) {
        if(rightMap.count(y[i])) { // if == 1, key is in map
        } else {
            rightMap.insert(pair<float, int>(y[i], 0));
            leftMap.insert(pair<float, int>(y[i], 0));
        }
    }

    for(i = 0; i < n; i++) {
        rwt++; // += aprior[j] * wt[i] => 1*1
        rightMap.at(y[i])++; // += wt[i] 
        rtot++;
    }

    total_ss = 0;
    for(auto it = rightMap.cbegin(); it != rightMap.cend(); it++) {
        temp = 1 * it->second / rwt;
        total_ss += rwt * impure(temp);
    }
    best = total_ss;

    for(i = 0; rtot > p->minNode; i++) {
        j = y[i];
        rwt--; // -= aprior[j] * wt[i]
        lwt++; // += aprior[j] * wt[i]
        rtot--;
        ltot++;
        rightMap.at(j)--; // -= wt[i]
        leftMap.at(j)++; // += wt[i]
        if (ltot >= p->minNode && x[i+1] != x[i]) {
            temp = 0;
            lmean = 0;
            rmean = 0;
            j = 0;
            for(auto right = rightMap.cbegin(), left = leftMap.cbegin(); 
                    right != rightMap.cend() && left != leftMap.cend();  right++, left++) {
                // key order should always be the same for std::map structures
                pp = 1 * left->second / lwt; // aprior[j] * left[j] / lwt
                temp += lwt * impure(pp);
                lmean += pp * j; 
                pp = 1 * right->second / rwt; // aprior[j] * right[j] / rwt
                temp += rwt * impure(pp);
                rmean += pp * j;
                j++;
            }
            if (temp < best) {
                best = temp;
                which = i;
                direction = lmean < rmean ? LEFT : RIGHT;
            }
        }
    }

    improve = total_ss - best;
    splitPoint = (n > which + 1) ? ((x[which] + x[which + 1]) / 2) : x[which];
}
