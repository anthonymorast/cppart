/*
 * Entry point of the program. Parses parameters, does some preprocessing,
 * creates the tree, runs cross-validations, creates tree files, etc.
 *
 * Author: Anthony Morast
 */


#include "cppart.h"

#include <fstream>

int main(int argc, char* argv[]) {
    auto start = chrono::system_clock::now();

    if (argc < 4) {
        printUsage();
        exit(0);
    }

    params p;
    cout << endl;
    int numObs = parseParameters(argv, argc, &p);
    
    // split test and train data
    if(!p.splitdata && p.testDataFilename == "") {
        cout << "Warning: splitting data set to false and no test data supplied..." << endl;
    }

    int numNodes = 0;
    cout << "Building tree..." << endl;
    node root = buildTree(&p, p.trainSize, numNodes);
    
    // stop the clock before xvals
    auto end = chrono::system_clock::now();
    cpTable *cpTableHead = buildCpTable(&root, &p);

    vector<int> iNode;
    int count = 0;
    fixTree(&root, (1 / root.dev), 1, count, iNode);

    int idx = p.filename.find(".");
    string treeFileName = p.filename.substr(0, idx);
    string cpTableFilename = p.filename.substr(0,idx);
    if(p.delayed) {
        treeFileName += ".delayed";
        cpTableFilename += ".delayed";
    }
    treeFileName += ".tree";
    cpTableFilename += ".cptable";
    printTree(&root, treeFileName);
    printCpTable(cpTableHead, cpTableFilename);

    float dev = 0;
    getTreeDeviance(&root, dev);

    cout << endl << endl << "Results: " << endl;
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end-start);
    cout << "Time elapsed " << ((float)elapsed.count() / 1000) << endl;
    cout << "Tree Deviance/Impurity: " << dev << endl << endl;
    cout << "Test Data: " << endl;
    float mae = 0;
    float relError = 0;
    int correct = 0, incorrect = 0;
    for (int i = 0; i < p.testSize; i++) {
        float *sample = p.testData[i];
        int respCol = getResponseColumnNumber(p.response, p.headers);
        float pred = getPrediction(&root, sample, respCol);
        float e = abs(pred - sample[respCol]);
        if(pred == sample[respCol]) {
            correct++;
        } else {
            incorrect++;
        }
        relError += e/1+abs(sample[respCol]); // class 0 = nan, need to add 1 to denominator
        mae += e;
        // error += anovaPredict(sample[0], pred); // square error
        //cout << "Actual: " << sample[respCol] << "\tPredicted: " <<  pred << endl;
    }
    if(p.method == ANOVA) {
        cout << "\tMAE: " << mae/p.testSize << endl;
    } else {
        cout << "\tCorrect Classifications: " << correct << endl;
        cout << "\tIncorrect Classifications: " << incorrect << endl;
        cout << "\tClassification Error (correct/total): " << (double)correct/p.testSize << endl;
    }
    cout << "\tRelative Error: " << relError/p.testSize << endl;

    return 0;
}

void printUsage()
{
    cout << endl << "Usage:" << endl;
    cout << "\tcppart.exe <data filename> <response> <delayed> [optional: runxvals=(0,1) splitdata=(0,1) testdata=<test data filename> randomsplit=(0,1) method=anova verbose={0, 1, 2}]" << endl;
    cout << "\t\trunxvals - 1=true to run cross-validations" << endl;
    cout << "\t\tsplitdata - if true (non-zero) splits the data into train and test data, 80/20 for train/test split." << endl;
    cout << "\t\ttestdata - filename for a file containing the data to be used as test data, if populated ignores splitdata param." << endl;
    cout << "\t\trandomsplit - if true (non-zero), splits the data randomly, otherwise takes the last 20% for training." << endl;
	cout << "\t\tmethod - \"Gini\" for classificaion \"Anova\" otherwise" << endl;
    cout << "\t\tverbose - 0 = no progress output, 1 = some output, 2 = all available output" << endl << endl;
}
