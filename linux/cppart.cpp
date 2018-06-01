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

    cout << "\nTest Data: " << endl;
    float mae = 0;
    float relError = 0;
    for (int i = 0; i < p.testSize; i++) {
        float *sample = p.testData[i];
        int respCol = getResponseColumnNumber(p.response, p.headers);
        float pred = getPrediction(&root, sample, respCol);
        float e = pred - sample[respCol];
        relError += abs(e/sample[respCol]);
        mae += abs(e);
        // error += anovaPredict(sample[0], pred); // square error
        //cout << "Actual: " << sample[respCol] << "\tPredicted: " <<  pred << endl;
    }
    cout << "MAE: " << mae/p.testSize << endl;
    cout << "Relative Error: " << relError/p.testSize << endl;

    return 0;
}

void printUsage()
{
    cout << endl << "Usage:" << endl;
    cout << "\tcppart.exe <data filename> <response> <delayed> [optional: runxvals=(0,1) splitdata=(0,1) testdata=<test data filename> randomsplit=(0,1)]" << endl;
    cout << "\t\trunxvals - 1=true to run cross-validations" << endl;
    cout << "\t\tsplitdata - if true (non-zero) splits the data into train and test data, 80/20 for train/test split." << endl;
    cout << "\t\ttestdata - filename for a file containing the data to be used as test data, if populated ignores splitdata param." << endl;
    cout << "\t\trandomsplit - if true (non-zero), splits the data randomly, otherwise takes the last 20% for training." << endl << endl;
}
