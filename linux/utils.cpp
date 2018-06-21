/*
 * Utilities shared by cppart and runmany.
 *
 * Author: Anthony Morast
 */

#include "utils.h"
#include <fstream>

// returns the number of observations for the first node.
int parseParameters(char * argv[], int argc, params *p)
{
    string filename = argv[1];
    string response = argv[2];

    int delayed = 0;
    try {
        delayed = stoi(argv[3]);
    } catch(exception e) {
        cout << "Could not parse integer value from delayed parameter \" " << argv[3] << "\"." << endl;
        printUsage();
        exit(0);
    }

    int xvals = 1;
    int split_data = 0;
    string test_data_filename = "";
    int randomSplit = 1;
    float cp = 0.01, alpha = 0;
    methods m = ANOVA;
    int verbose = 0;
    if(argc > 4) {
        for(int i = 4; i < argc; i++) {
            string param = argv[i];
            int pos_xval = param.find("runxvals=");
            int pos_split = param.find("splitdata=");
            int pos_test = param.find("testdata=");
            int pos_rand = param.find("randomsplit=");
            int pos_cp = param.find("cp=");
            int pos_meth = param.find("method=");
            int pos_verbose = param.find("verbose=");
            if(pos_xval != string::npos) {
                try {
                    xvals = stoi(string(1, argv[i][pos_xval+9]));
                } catch (exception e) {
                    cout << "Warning: error parsing integer value for cross-validation flag..." << endl;
                }
            } else if(pos_split != string::npos) {
                try {
                    split_data = stoi(string(1, argv[i][pos_split+10]));
                } catch (exception e) {
                    cout << "Warning: error parsing integer value for split data flag..." << endl;
                }
            } else if(pos_test != string::npos) {
                test_data_filename = param.substr(pos_test+9);
            } else if(pos_rand != string::npos) {
                try {
                    randomSplit = stoi(string(1, argv[i][pos_rand+12]));
                } catch(exception e) {
                    cout << "Warning: random split integer cannot be parsed." << endl;
                }
            } else if(pos_cp != string::npos) {
                try {
                    cp = stof(param.substr(pos_cp+3)); 
                } catch(exception e) {
                    cout << "Warning: cp parameter not parsable as float, dedault 0.01 used." << endl;
                }
            } else if(pos_meth != string::npos) {
                try {
                    string val = param.substr(pos_meth+7);
                    transform(val.begin(), val.end(), val.begin(), ::tolower);
                    if (val == "gini") {
                        m = GINI;
                    }
                } catch(exception e) {
                    cout << "Warning: method parameter not parsable, defaulting to anova." << endl;
                }
            } else if (pos_verbose != string::npos) {
                try {
                    verbose = stoi(string(1, argv[i][pos_verbose+8]));
                } catch(exception e) {
                    cout << "Warning: verbose parameter no parsable integer, defaulting to 0." << endl;
                }
            } else {
                cout << "Warning: Unused parameter " << param << "..." << endl;
            }
        }
    }	

    ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        cout << "File \"" << filename << "\" does not exist." << endl;
        exit(0);
    }

    string headers;
    bool responseFound = false;
    getline(fin, headers);
    string value;
    istringstream ss(headers);
    int varCount = 0;
    while (getline(ss, value, ',')) {
        if(!responseFound) {
            responseFound = !strcmp(value.c_str(), response.c_str());
        }
        varCount += 1;
    }

    string *headerValues = new string[varCount];
    varCount = 0;
    istringstream ss2(headers);
    while (getline(ss2, value, ',')) {
        headerValues[varCount] = value;
        varCount += 1;
    }

    if (!responseFound) {
        cout << "Response column not found in dataset, does your data have headers?" << endl;
        exit(0);
    }
    fin.close();

    int lineCount = getLineCount(filename);
    int colCount = getColumnCount(headers);
    p->dataLineCount = lineCount;

    float **data = new float*[lineCount - 1];
    for (int i = 0; i < lineCount; i++) {
        data[i] = new float[colCount];
    }
    getData(filename, response, headers, data);

    float* y = getResponseData(response, headers, data, lineCount-1);
    double mean, risk;
    anovaSS(y, lineCount-1, mean, risk);
    alpha = cp * risk;
    free1DData(y);

    float **testData, **trainData;
    int numObs = lineCount - 1;
    int testSize = 0, trainSize = numObs;
    if(split_data && test_data_filename == "") {
        testSize = numObs/5;
        trainSize = numObs - testSize;

        testData = new float*[testSize];
        for(int i = 0; i < testSize; i++) {
            testData[i] = new float[colCount];
        }
        trainData = new float*[trainSize];
        for(int i = 0; i < trainSize; i++) {
            trainData[i] = new float[colCount];
        }

        // generate random indicies to be held out for testing data, no replacement.
        int indices[testSize];  // this was float... shouldn't it be int?
        int counter = 0;
        if(randomSplit) {
            srand(time(NULL));
            while(counter < testSize) {
                int value = rand() % numObs;
                bool found = false;
                for(int i = 0; i < counter; i++) {
                    if (value == indices[i]) {
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    indices[counter] = value;
                    counter++;
                }
            }
        }

        int testIdx = 0;
        int trainIdx = 0;
        for(int i = 0; i < numObs; i++) {
            if(randomSplit) {
                bool found = false;
                for(int j = 0; j < counter; j++) {
                    if(indices[j] == i) {
                        found = true;
                        break;
                    }
                }
                if(found) {
                    testData[testIdx] = data[i];
                    testIdx++;
                } else {
                    trainData[trainIdx] = data[i];
                    trainIdx++; 
                }
            } else {
                // grab first 80% for train, last 80% for test
                if(i < trainSize) {
                    trainData[trainIdx] = data[i];
                    trainIdx++;
                } else {
                    testData[testIdx] = data[i];
                    testIdx++;
                }
            }
        }
    } else if (test_data_filename != "") {
        ifstream tfin;
        tfin.open(test_data_filename);
        if (!tfin.is_open()) {
            cout << "Test data file \"" << test_data_filename << "\" does not exist." << endl;
            exit(0);
        }

        string theaders;
        bool tresponseFound = false;
        getline(tfin, theaders);
        if(headers != theaders) {
            cout << "Error: headers for test data file do not match headers for training data file." << endl;
            exit(0);
        }

        int tlineCount = getLineCount(test_data_filename);
        int tcolCount = getColumnCount(theaders);

        testData = new float*[tlineCount - 1];
        for (int i = 0; i < tlineCount-1; i++) {
            testData[i] = new float[tcolCount];
        }
        getData(test_data_filename, response, theaders, testData);
        trainData = deepCopyData(data, lineCount - 1, colCount);
        testSize = tlineCount - 1;
        trainSize = numObs;
    } else {
        trainData = deepCopyData(data, lineCount - 1, colCount);
    }

    // get number of classes
    int numclasses = 0;
    if(m == GINI) { //classification problem
        vector<float> classes;
        int respCol = getResponseColumnNumber(response, headers);
        // get all unique classes in training data
        for(int i = 0; i < trainSize; i++) {
            float c = trainData[i][respCol];
            if(!classes.empty()) {
                if(find(classes.begin(), classes.end(), c) == classes.end()) {
                    classes.push_back(c);
                }
            } else {
                classes.push_back(c);
            }
        }
        // testing data may have unseen classes
        for(int i = 0; i < testSize; i++) {
            // assuming at least one example in the training data is labeled
            float c = testData[i][respCol];
            if(find(classes.begin(), classes.end(), c) == classes.end()) {
                classes.push_back(c);
            }
        }
        numclasses = classes.size();
    }

    p->response = response;
    p->maxDepth = 30;	// only used to set maxNodes
    p->maxNodes = (int)pow(2, (p->maxDepth + 1)) - 1;
    p->minObs = 20;
    p->minNode = 7;
    p->numXval = 10;
    p->iscale = 0;		// this may not be used
    p->delayed = delayed == 1;
    p->trainData = trainData;
    p->testData = testData;
    p->testSize = testSize;
    p->trainSize = trainSize;
    p->data = data;
    p->where = new int[lineCount];
    p->headers = headers;
    p->filename = filename;
    p->varNames = headerValues;
    p->runXVals = xvals >= 1;
    p->splitdata = split_data >= 1;
    p->testDataFilename = test_data_filename;
    p->complexity = cp;
    p->alpha = alpha;
    p->method = m;
    p->numclasses = numclasses;
    p->verbose = verbose;

    for (int i = 0; i < lineCount; i++) {
        p->where[i] = 0;	
    }

    if(p->splitdata && test_data_filename == "") {
        numObs /= 5;
    } 
    return numObs;
}

node buildTree(params * p, int numObs, int &numNodes)
{
    node tree;
    tree.numObs = numObs;
    tree.data = deepCopyData(p->trainData, numObs, getColumnCount(p->headers));

    double risk;
    partition(p, &tree, 1, risk);
    //freeTreeData(&tree);
    return tree;
}

void fixTree(node * n, float cpScale, int nodeId, int &nodeCount, vector<int> &iNode)
{
    n->cp *= cpScale;
    n->nodeId = nodeId;
    nodeCount += 1;
    iNode.push_back(nodeId);

    if (n->leftNode != NULL) {
        fixTree(n->leftNode, cpScale, 2 * nodeId, nodeCount, iNode);
    }
    if (n->rightNode != NULL) {
        fixTree(n->rightNode, cpScale, (2 * nodeId) + 1, nodeCount, iNode);
    }
}

cpTable *buildCpTable(node *root, params *p)
{
    cpTable *cpTableHead = new cpTable(), *currCpTable, *tempCpTable, *prevCpTable;
    cpTable *cpTail = new cpTable();

    cout << "Building CP Table..." << endl;
    vector<double> cpList;
    cpList.push_back(root->cp);
    double parentCp = root->cp;
    int uniqueCp = 2;

    makeCpList(root, parentCp, cpList, uniqueCp, p);
    sort(cpList.begin(), cpList.end());
    reverse(cpList.begin(), cpList.end());
    p->uniqueCp = cpList.size();

    // make linked list
    cpTableHead->back = new cpTable();
    currCpTable = cpTableHead;
    prevCpTable = cpTableHead->back;
    vector<double>::iterator it;
    for (it = cpList.begin(); it != cpList.end(); it++) {
        double cp = *it;
        currCpTable->cp = (float) cp;
        if (cp != cpList.at(cpList.size() - 1)) {
            currCpTable->forward = new cpTable;
        }
        if (&prevCpTable != NULL) {
            currCpTable->back = prevCpTable;
        }
        prevCpTable = currCpTable;
        currCpTable = currCpTable->forward;
    }
    cpTail = prevCpTable;

    makeCpTable(root, parentCp, 0, cpTail);
    tempCpTable = cpTableHead;

    // cross validations
    if (p->numXval && p->runXVals) {
        cout << "Running cross-validations..." << endl;
        vector<int> groups;
        for(int i = 0; i < root->numObs; i++) {
            groups.push_back(i % p->numXval);
        }
        shuffle(groups.begin(), groups.end(), default_random_engine(time(NULL)));
        int *xGrps = &groups[0];
        xval(cpTableHead, xGrps, *p);
    }

    tempCpTable = cpTableHead;
    float scale = 1 / root->dev;
    while (tempCpTable != NULL) {
        tempCpTable->cp *= scale;
        tempCpTable->risk *= scale;
        tempCpTable->xstd *= scale;
        tempCpTable->xrisk *= scale;

        tempCpTable = tempCpTable->forward;
    }
    cpTableHead->risk = 1.0;

    return cpTableHead;
}


float getPrediction(node *tree, float row[], int responseCol)
{
    /* some leaves (all leaves?) don't have spltVar, splitPoint, etc. set*/
    if(tree->leftNode == NULL && tree->rightNode == NULL) {
        return tree->yval;
    }

    int splitVar = tree->varIndex;
    float splitPoint = tree->splitPoint;
    int direction = tree->direction;
    float predValue = 0;

    if(row[splitVar] > splitPoint) {
        direction *= -1;
    }
    if(direction < 0) {
        if(tree->leftNode == NULL) {
            return tree->yval;
        } else {
            predValue = getPrediction(tree->leftNode, row, responseCol);
        }
    } else {
        if(tree->rightNode == NULL) {
            cout << tree->yval << endl;
            return tree->yval;
        } else {
            predValue = getPrediction(tree->rightNode, row, responseCol);
        }
    }

    return predValue;
}

