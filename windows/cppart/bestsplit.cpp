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


	for (int varIdx = 0; varIdx < colCount; varIdx++) {
		if (varIdx == respCol) {
			continue;
		}
		// sort on x[varidx]
		mergeSort(data, 0, numObs-1, varIdx, colCount, respCol); // -1 since we go until <= high

		// call anova split point
		int where, direction;
		float splitPoint, improve;
		double leftSS, rightSS, mean, thisSS, thisSSLeft, thisSSRight;

		float *x = getExplanatoryDataCol(p->response, p->headers, data, numObs, varIdx);
		float *y = getResponseData(p->response, p->headers, data, numObs);
		anovaSplit(x, y, p, varIdx, where, direction, splitPoint, improve, numObs);

		// split data (set left, right, numLeft, numRight)
		int numLeft = 0;
		int numRight = 0;
		getSplitCounts(data, varIdx, splitPoint, direction, numObs, numLeft, numRight);

		float **L1 = new float*[numLeft];
		float **L2 = new float*[numRight];
		for (int i = 0; i < numLeft; i++) {
			L1[i] = new float[colCount];
		}for (int i = 0; i < numRight; i++) {
			L2[i] = new float[colCount];
		}

		// split the data
		splitData(direction, splitPoint, varIdx, colCount, numObs, L1, L2, data);

		if (p->delayed && improve > 0) {
			double bestLeftSS = DBL_MAX, bestRightSS = DBL_MAX;

			for (int varIdx2 = 0; varIdx2 < colCount; varIdx2++) {
				mergeSort(L1, 0, numLeft-1, varIdx2, colCount, respCol);
				mergeSort(L2, 0, numRight-1, varIdx2, colCount, respCol);

				int whereL, directionL, whereR, directionR;
				float splitPointL, improveL, splitPointR, improveR;
				
				// left data
				float *lx = getExplanatoryDataCol(p->response, p->headers, L1, numLeft, varIdx2);
				float *ly = getResponseData(p->response, p->headers, L1, numLeft);
				anovaSplit(lx, ly, p, varIdx2, whereL, directionL, splitPointL, improveL, numLeft);

				int L3Size = 0, L4Size = 0;
				getSplitCounts(L1, varIdx2, splitPointL, directionL, numLeft, L3Size, L4Size);
				float **L3 = new float*[L3Size];
				float **L4 = new float*[L4Size];
				for (int i = 0; i < L3Size; i++) {
					L3[i] = new float[colCount];
				}
				for (int i = 0; i < L4Size; i++) {
					L4[i] = new float[colCount];
				}

				splitData(directionL, splitPointL, varIdx2, colCount, numLeft, L3, L4, L1);
				double l3SS, l4SS;
				anovaSS(getResponseData(p->response, p->headers, L3, L3Size), L3Size, mean, l3SS);
				anovaSS(getResponseData(p->response, p->headers, L4, L4Size), L4Size, mean, l4SS);
				thisSSLeft = l3SS + l4SS;

				if (thisSSLeft < bestLeftSS && improveL > 0) {
					bestLeftSS = thisSSLeft;
				}

				// right data
				float *rx = getExplanatoryDataCol(p->response, p->headers, L2, numRight, varIdx2);
				float *ry = getResponseData(p->response, p->headers, L2, numRight);
				anovaSplit(rx, ry, p, varIdx2, whereR, directionR, splitPointR, improveR, numRight);

				int L5Size = 0, L6Size = 0;
				getSplitCounts(L2, varIdx2, splitPointR, directionR, numRight, L5Size, L6Size);
				float **L5 = new float*[L5Size];
				float **L6 = new float*[L6Size];
				for (int i = 0; i < L5Size; i++) {
					L5[i] = new float[colCount];
				}
				for (int i = 0; i < L6Size; i++) {
					L6[i] = new float[colCount];
				}
				splitData(directionR, splitPointR, varIdx2, colCount, numRight, L5, L6, L2);
				double l5SS, l6SS;
				anovaSS(getResponseData(p->response, p->headers, L5, L5Size), L5Size, mean, l5SS);
				anovaSS(getResponseData(p->response, p->headers, L6, L6Size), L6Size, mean, l6SS);
				thisSSRight = l5SS + l6SS;
				
				if (thisSSRight < bestRightSS && improveR > 0) {
					bestRightSS = thisSSRight;
				}
			}

			if (bestLeftSS != baseSS && bestRightSS != baseSS) {
				thisSS = bestLeftSS + bestRightSS;
			}
			else {
				thisSS = baseSS;
			}
		}
		else {
			anovaSS(getResponseData(p->response, p->headers, L1, numLeft), numLeft, mean, leftSS);
			anovaSS(getResponseData(p->response, p->headers, L2, numRight), numRight, mean, rightSS);
			thisSS = leftSS + rightSS;
		}

		if (thisSS == baseSS) {
			anovaSS(getResponseData(p->response, p->headers, L1, numLeft), numLeft, mean, leftSS);
			anovaSS(getResponseData(p->response, p->headers, L2, numRight), numRight, mean, rightSS);
			thisSS = leftSS + rightSS;
		}

		if (thisSS < bestSS && improve > 0) {
			bestSS = thisSS;
			n->splitPoint = splitPoint;
			n->direction = direction;
			n->index = where;
			n->varIndex = varIdx;
			n->yval = yBar;
			n->dev = deviance;
			n->improvement = improve;

			n->rightNode->data = L2;
			n->leftNode->data = L1;

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

void getSplitCounts(float ** data, int splitVar, float splitPoint, int direction, int numObs, int & leftCount, int & rightCount)
{
	if (direction < 0) {
		for (int i = 0; i < numObs; i++) {
			if (data[i][splitVar] < splitPoint) {
				leftCount++;
			}
			else {
				rightCount++;
			}
		}
	}
	else {
		for (int i = 0; i < numObs; i++) {
			if (data[i][splitVar] <= splitPoint) {
				rightCount++;
			}
			else {
				leftCount++;
			}
		}
	}
}

void splitData(int direction, float splitPoint, int splitVar, int colCount, int numObs, float ** left, float ** right, float ** data)
{
	int leftCnt = 0, rightCnt = 0;
	for (int i = 0; i < numObs; i++) {
		if (direction < 0) {
			if (data[i][splitVar] < splitPoint) {
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
			if (data[i][splitVar] <= splitPoint) {
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

}