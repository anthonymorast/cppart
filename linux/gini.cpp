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

    // get risk (stolen again from rpart)
    deviance = 0;
    int numclasses = classFreq.size();
    for(int i = 0; i < numclasses; i++) {
        int temp = 0;
        for(int j = 0; j < numclasses; j++) {
            temp += classFreq.at(j);
        }
        if(i == 0 || temp < deviance) {
            deviance = temp;
        }
    }
}

// ripped off directly from rpart
void giniSplit(float *x, float *y, params *p, int &which, 
				int &direction, float &splitPoint, float &improve, int n) {
    // in rpart prior[i] = aprior[i] = 0 for all i
    int i, j, k;
    double lwt = 0, rwt = 0;
    int rtot = 0, ltot = 0;
    direction = LEFT; // which = where
    double total_ss, best, temp, pp;
    double lmean, rmean;

    double *left = new double[p->numclasses], *right = new double[p->numclasses]; 

    for(i = 0; i < p->numclasses; i++) {
        left[i] = 0;
        right[i] = 0;
    }

    for(i = 0; i < i < n; i++) {
        j = (int) y[i] - 1;
        rwt++; // += aprior[j] * wt[i] => 1*1
        right[j]++; // += wt[i]
        rtot++;
    }

    total_ss = 0;
    for(i = 0; i < p->numclasses; i++) {
        temp = 1 * right[i] / rwt; // = aprior[i] * right[i] / rwt
        total_ss += rwt * impure(temp);
    }
    best = total_ss;

    for(i = 0; rtot > p->minNode; i++) {
        j = (int) y[i] - 1;
        rwt--; // -= aprior[j] * wt[i]
        lwt++; // += aprior[j] * wt[i]
        rtot--;
        ltot++;
        right[j]--; // -= wt[i]
        left[j]++; // += wt[i]
        if (ltot >= p->minNode && x[i+1] != x[i]) {
            temp = 0;
            lmean = 0;
            rmean = 0;
            for(j = 0; j < p->numclasses; j++) {
                pp = 1 * left[j] / lwt; // aprior[j] * left[j] / lwt
                temp += lwt * impure(pp);
                lmean += pp * j;
                pp = 1 * right[j] / rwt; // aprior[j] * right[j] / rwt
                temp += rwt * impure(pp);
                rmean += pp * j;
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

    delete[] left;
    delete[] right;
    right = NULL;
    left = NULL; 
}
