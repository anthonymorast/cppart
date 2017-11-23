#include "bestsplit.h"

void bestsplit(node *n, params *p, string response, int & numleft, int & numright)
{
	float **data = n->data;
	n->rightNode = new node;
	n->leftNode = new node;

	int colCount = getColumnCount(p->headers);
	int respCol = getResponseColumnNumber(p->response, p->headers);
	int numObs = n->numObs;
	double bestSS = DBL_MAX;
	double baseSS = bestSS;
	double yBar, deviance;
	anovaSS(getResponseData(p->response, p->headers, data, numObs), numObs, yBar, deviance);

	// OMP
	for (int varIdx = 0; varIdx < colCount; varIdx++) {
		if (varIdx == respCol) {
			continue;
		}
		// sort on x[varidx]
		mergeSort(data, 0, numObs-1, varIdx, colCount, respCol); // -1 since we go until <= high

		// call anova split point
		int where, direction, split, improvement;
		float splitPoint, improve;
		float *x = getExplanatoryDataCol(p->response, p->headers, data, numObs, varIdx);
		float *y = getResponseData(p->response, p->headers, data, numObs);
		anovaSplit(x, y, p, varIdx, where, direction, splitPoint, improve, numObs);

		// split data (set left, right, numLeft, numRight)
		int numLeft = 0, numRight = 0;
		if (direction < 0) {
			for (int i = 0; i < numObs; i++) {
				if (data[i][varIdx] < splitPoint) {
					numLeft++;
				}
				else {
					numRight++;
				}
			}
		}
		else {
			for (int i = 0; i < numObs; i++) {
				if (data[i][varIdx] <= splitPoint) {
					numRight++;
				}
				else {
					numLeft++;
				}
			}
		}
		float **left = new float*[numLeft];
		float **right = new float*[numRight];
		for (int i = 0; i < numLeft; i++) {
			left[i] = new float[colCount];
		}for (int i = 0; i < numRight; i++) {
			right[i] = new float[colCount];
		}

		// split the data
		int leftCnt = 0, rightCnt = 0;
		for (int i = 0; i < numObs; i++) {
			if (direction < 0) {
				if (data[i][varIdx] < splitPoint) {
					for (int j = 0; j < colCount; j++) {
						left[leftCnt][j] = data[i][j];
					}
					leftCnt++;
				}
				else {
					for (int j = 0; j < colCount; j++) {
						right[rightCnt][j] = data[i][j];
					}
					rightCnt++;
				}
			}
			else {
				if (data[i][varIdx] <= splitPoint) {
					for (int j = 0; j < colCount; j++) {
						right[rightCnt][j] = data[i][j];
					}
					rightCnt++;
				}
				else {
					for (int j = 0; j < colCount; j++) {
						left[leftCnt][j] = data[i][j];
					}
					leftCnt++;
				}
			}
		}


		// calc SS
		double leftSS, rightSS, mean;
		anovaSS(getResponseData(p->response, p->headers, left, numLeft), numLeft, mean, leftSS);
		anovaSS(getResponseData(p->response, p->headers, right, numRight), numRight, mean, rightSS);
		double thisSS = leftSS + rightSS;

		if (thisSS < bestSS && improve > 0) {
			bestSS = thisSS;
			n->splitPoint = splitPoint;
			n->direction = direction;
			n->index = where;
			n->varIndex = varIdx;
			n->yval = yBar;
			n->dev = deviance;
			n->improvement = improve;

			n->rightNode->data = right;
			n->leftNode->data = left;

			numleft = numLeft;
			numright = numRight;
		}
	}
	// cout << n->varIndex << "  " << bestSS << "   " << n->splitPoint << "   " << n->index << endl;
}

void mergeSort(float **x, int low, int high, int varIdx, int colCount, int respCol) {
	int mid;
	if (low < high) {
		mid = (low + high) / 2;
		mergeSort(x, low, mid, varIdx, colCount, respCol);
		mergeSort(x, mid + 1, high, varIdx, colCount, respCol);

		merge(x, low, high, mid, varIdx, colCount, respCol);
	}
}

void merge(float **x, int low, int high, int mid, int varIdx, int colCount, int respCol) {
	int i = low, j = mid + 1, k = 0;
	const int size = high - low + 1;

	float **temp = new float*[high - low + 1];
	for (int idx = 0; idx < (high - low + 1); idx++) {
		temp[idx] = new float[colCount - 1];
	}

	while (i <= mid && j <= high) {
		if (x[i][varIdx] < x[j][varIdx]) {
			for (int col = 0; col < colCount; col++) {
				temp[k][col] = x[i][col];
			}
			k++;
			i++;
		}
		else if (x[i][varIdx] > x[j][varIdx]) {
			for (int col = 0; col < colCount; col++) {
				temp[k][col] = x[j][col];
			}
			k++;
			j++;
		}
		else { // if equal sort on response column (is this what excel does?)
			if (x[i][respCol] < x[j][respCol]) {
				for (int col = 0; col < colCount; col++) {
					temp[k][col] = x[i][col];
				}
				k++;
				i++;
			}
			else {
				for (int col = 0; col < colCount; col++) {
					temp[k][col] = x[j][col];
				}
				k++;
				j++;
			}
		}
	}

	while (i <= mid) {
		for (int col = 0; col < colCount; col++) {
			temp[k][col] = x[i][col];
		}
		k++;
		i++;
	}

	while (j <= high) {
		for (int col = 0; col < colCount; col++) {
			temp[k][col] = x[j][col];
		}
		k++;
		j++;
	}

	for (k = 0, i = low; i <= high; ++i, ++k) {
		for (int col = 0; col < colCount; col++) {
			x[i][col] = temp[k][col];
		}
	}

	//for (int idx = 0; idx < (high - low + 1); idx++) {
	//	delete[] temp[idx];
	//}
	//delete[] temp;
}
