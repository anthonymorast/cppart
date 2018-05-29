/*
 * Performs the cross-validations which fills in some values
 * in the CP table.
 *
 * Author: Anthony Morast
 */


#include "xval.h"

void xval(cpTable * tableHead, int xGroups[], params p)
{
    int colCount = getColumnCount(p.headers);
    int numObs = p.dataLineCount - 1;
    int respColNumber = getResponseColumnNumber(p.response, p.headers);

    float *xtemp = new float[p.uniqueCp];
    float *xpred = new float[p.uniqueCp];

    float *cpList = new float[p.uniqueCp];
    cpTable *cpTemp = tableHead;
    cpList[0] = tableHead->cp * 10;
    int i = 1;
    while (cpTemp->forward != NULL) {
        cpList[i] = sqrt(cpTemp->cp * cpTemp->forward->cp);
        cpTemp = cpTemp->forward;
        i++;
    }

    float totalWt = numObs;
    float oldWt = numObs;

    cout << "\tPerforming cross-validations..." << endl;
    for (int i = 0; i < p.numXval; i++) {
        cout << "\t\tGroup " << (i+1) << " of " << p.numXval << "..." << endl;
        float size = getXGroupSize(xGroups, i, numObs);
        float **groupData = new float*[(int)size];
        for (int j = 0; j < size; j++) {
            groupData[j] = new float[colCount];
        }
        int holdoutsize = numObs - size;
        float **holdout = new float*[holdoutsize];
        for (int j = 0; j < holdoutsize; j++) {
            holdout[j] = new float[colCount];
        }
        getGroupData(p.data, groupData, holdout, xGroups, i, colCount, numObs);

        for (int j = 0; j < p.uniqueCp; j++) {
            cpList[j] *= (size / oldWt);
        }

        oldWt = size;
        node *xtree = new node();
        xtree->numObs = size;
        xtree->data = deepCopyData(groupData, size, colCount);
        double mean, ss;
        float* y = getResponseData(p.response, p.headers, groupData, size);
        anovaSS(y, size, mean, ss);
        xtree->cp = (float) ss;
        double sumrisk;
        partition(&p, xtree, 1, sumrisk);
        fixCp(xtree, xtree->cp);

        for (int j = 0; j < holdoutsize; j++) {
            float *row = holdout[j];
            rundown(xtree, row, cpList, xtemp, xpred, p.uniqueCp, respColNumber);

            cpTable *tempCpTable = tableHead;
            for (int k = 0; k < p.uniqueCp; k++) {
                tempCpTable->xrisk += xtemp[k];
                tempCpTable->xstd += xtemp[k] * xtemp[k];
                tempCpTable = tempCpTable->forward;
            }
        } 
        freeTreeData(xtree);
        free2DData(holdout, holdoutsize);
        free2DData(groupData, size);
        free1DData(y);
    }

    cpTable *tempCpTable = tableHead;
    while (tempCpTable != NULL) {
        tempCpTable->xstd = sqrt(tempCpTable->xstd - (tempCpTable->xrisk * tempCpTable->xrisk) / totalWt);
        tempCpTable = tempCpTable->forward;
    }
    
    free1DData(xtemp);
    free1DData(xpred);
}

void rundown(node *tree, float row[], float cpList[], float xtemp[], float xpred[], int uniqueCp, int responseCol)
{
    for (int i = 0; i < uniqueCp; i++) {
        float cp = cpList[i];
        while (cp < tree->cp) {
            tree = branch(tree, row);
            if (tree == NULL) {
                cout << "What are you trying to pull?" << endl;
                exit(0);
            }
        }

        xpred[i] = tree->yval;
        xtemp[i] = anovaPredict(row[responseCol], tree->yval);
    }
}

node *branch(node *tree, float row[])
{
    if (tree->leftNode == NULL) {
        return NULL;
    }

    int splitVar = tree->varIndex;
    float splitPoint = tree->splitPoint;
    int direction = tree->direction;

    if (row[splitVar] > splitPoint) {
        direction = -direction;
    }

    if (direction < 0) {
        return tree->leftNode;
    }
    return tree->rightNode;
}

void fixCp(node *n, float parentCp)
{
    if (n->cp > parentCp) {
        n->cp = parentCp;
    }

    if (n->leftNode) {
        fixCp(n->leftNode, n->cp);
        fixCp(n->rightNode, n->cp);
    }
}

int getXGroupSize(int xGrps[], int group, int numObs)
{
    int size = 0;
    for (int i = 0; i < numObs; i++) {
        if (xGrps[i] != group) {
            size++;
        }
    }

    return size;
}

void getGroupData(float **data, float **groupData, float **holdout, int xGrps[], int group, int colCount, int numObs) {
    int groupIdx = 0;
    int holdoutIdx = 0;

    for (int i = 0; i < numObs; i++) {
        if (xGrps[i] != group) {
            for (int j = 0; j < colCount; j++) {
                groupData[groupIdx][j] = data[i][j];
            }
            groupIdx++;
        }
        else {
            for (int j = 0; j < colCount; j++) {
                holdout[holdoutIdx][j] = data[i][j];
            }
            holdoutIdx++;
        }
    }
}
