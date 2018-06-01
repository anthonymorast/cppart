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
/*
    float squareError = 0;
    int n = 0;
    for (int i = 0; i < n; i++) {
        float *sample = p.data[i];
        int respCol = getResponseColumnNumber(p.response, p.headers);
        float pred = getPrediction(&root, sample, respCol);
        squareError += anovaPredict(sample[0], pred);
        cout << "Actual: " << sample[0] << "\tPredicted: " <<  pred << endl;
    }*/
//    cout << "MSE: " << squareError/n << endl;

    return 0;
}

void printUsage()
{
    cout << "Usage:" << endl;
    cout << "\tcppart.exe <data filename> <response> <delayed> [optional: runxvals=(0,1) splitdata=(0,1) testdata=<test data filename>]" << endl;
}
