/*
 * Entry point of the program. Parses parameters, does some preprocessing,
 * creates the tree, runs cross-validations, creates tree files, etc.
 *
 * Author: Anthony Morast
 */


#include "cppart.h"

#include <fstream>

int main(int argc, char* argv[]) {
    clock_t start, end;

    start = clock();

    if (argc < 4) {
        printUsage();
        exit(0);
    }

    params p;
    int numObs = parseParameters(argv, argc, &p);

    // split test and train data
    if(!p.splitdata && p.testDataFilename == "") {
        cout << "Warning: splitting data set to false and no test data supplied..." << endl;
    }

    int numNodes = 0;
    cout << "Building tree..." << endl;
    node root = buildTree(&p, p.trainSize, numNodes);
    cpTable *cpTableHead = buildCpTable(&root, &p);

    vector<int> iNode;
    int count = 0;
    fixTree(&root, (1 / root.dev), 1, count, iNode);

    int idx = p.filename.find(".");
    string treeFileName = p.filename.substr(0, idx);
    if(p.delayed) {
        treeFileName += ".delayed";
    }
    treeFileName += ".tree";
    printTree(&root, treeFileName);

    end = clock();
    cout << "Time elapsed " << ((float)(end - start)) / CLOCKS_PER_SEC << endl;

    float error = 0;
    for (int i = 0; i < p.testSize; i++) {
        cout << i << endl;
        float *sample = p.testData[i];
        int respCol = getResponseColumnNumber(p.response, p.headers);
        float pred = getPrediction(&root, sample, respCol);
        cout << "here" << endl;
        float e = pred - sample[respCol];
        error += e;
        // error += anovaPredict(sample[0], pred); // square error
        cout << "Actual: " << sample[respCol] << "\tPredicted: " <<  pred << endl;
    }
    cout << "MAE: " << error/p.testSize << endl;

    return 0;
}

void printUsage()
{
    cout << "Usage:" << endl;
    cout << "\tcppart.exe <data filename> <response> <delayed> [optional: runxvals=(0,1) splitdata=(0,1) testdata=<test data filename>]" << endl;
}
