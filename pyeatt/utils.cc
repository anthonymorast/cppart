/*
 * Utilities shared by cppart and runmany.
 *
 * Author: Anthony Morast
 */

#include <fstream>
#include <cstring>
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>


#include <utils.h>
#include <data_handler.h>

using namespace std;

void printUsage(char **argv, int argc)
{
  cout << endl << "Usage:" << endl;
  cout <<argv[0]<<" <data filename> <response> <delayed> [optional: runxvals=(0,1) splitdata=(0,1) testdata=<test data filename> randomsplit=(0,1) method=anova verbose={0, 1, 2}]" << endl;
  cout << "\trunxvals - 1=true to run cross-validations" << endl;
  cout << "\tsplitdata - if true (non-zero) randomly splits the data into train and\n\t\ttest data, 80/20 for training/testing." << endl;
  cout << "\ttestdata - filename for a file containing the data to be used as test \n\t\tdata, if populated ignores splitdata param." << endl;
  cout << "\trandomsplit - if true (non-zero), splits the data randomly, otherwise \n\t\ttakes the last 20% for training." << endl;
  cout << "\tmethod - \"Gini\" for classificaion \"Anova\" otherwise" << endl;
  cout << "\tverbose - 0 = no progress output, 1 = some output, 2 = all available output" << endl << endl;
}



// returns the number of observations for the first node.
void parseParameters(char * argv[], int argc, params *p)
{
  string filename = argv[1];
  string response = argv[2];

  int delayed = 0;
  try {
    delayed = stoi(argv[3]);
  } catch(exception e) {
    cout << "Could not parse integer value from delayed parameter \" " << argv[3] << "\"." << endl;
    printUsage(argv,argc);
    exit(0);
  }

  int xvals = 1;
  int split_data = 0;
  string test_data_filename = "";
  int randomSplit = 1;
  double cp = 0.01;
  // double alpha = 0;
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

      if(pos_xval != (int)string::npos) {
	try {
	  xvals = stoi(string(1, argv[i][pos_xval+9]));
	} catch (exception e) {
	  cout << "Warning: error parsing integer value for cross-validation flag..." << endl;
	}
      } else if(pos_split != (int)string::npos) {
	try {
	  split_data = stoi(string(1, argv[i][pos_split+10]));
	} catch (exception e) {
	  cout << "Warning: error parsing integer value for split data flag..." << endl;
	}
      } else if(pos_test != (int)string::npos) {
	test_data_filename = param.substr(pos_test+9);
      } else if(pos_rand != (int)string::npos) {
	try {
	  randomSplit = stoi(string(1, argv[i][pos_rand+12]));
	} catch(exception e) {
	  cout << "Warning: random split integer cannot be parsed." << endl;
	}
      } else if(pos_cp != (int)string::npos) {
	try {
	  cp = stod(param.substr(pos_cp+3)); 
	} catch(exception e) {
	  cout << "Warning: cp parameter not parsable as double, dedault 0.01 used." << endl;
	}
      } else if(pos_meth != (int)string::npos) {
	try {
	  string val = param.substr(pos_meth+7);
	  transform(val.begin(), val.end(), val.begin(), ::tolower);
	  if (val == "gini") {
	    m = GINI;
	  }
	} catch(exception e) {
	  cout << "Warning: method parameter not parsable, defaulting to anova." << endl;
	}
      } else if (pos_verbose != (int)string::npos) {
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

  // READ THE DATA FILE(S) -- move this somewhere else
  ifstream fin;
  fin.open(filename);
  if (!fin.is_open()) {
    cout << "File \"" << filename << "\" does not exist." << endl;
    exit(0);
  }

  // FIRST PART -- count columns and parse the column names Read the
  // first line as a single string, and assume it contains the name of
  // each column, separated by commas
  string headers;
  bool responseFound = false;
  getline(fin, headers);
  fin.close();
  // count the number of columns, and also search for the response
  // variable name
  string value;
  istringstream ss(headers);
  int varCount = 0;
  while (getline(ss, value, ',')) {
    if(!responseFound) {
      responseFound = !strcmp(value.c_str(), response.c_str());
    }
    varCount += 1;
  }
  // Allocate array of strings, and split up the single string
  string *headerValues = new string[varCount];
  varCount = 0;
  istringstream ss2(headers);
  while (getline(ss2, value, ',')) {
    headerValues[varCount] = value;
    varCount += 1;
  }
  
  if (!responseFound) {
    cerr << "Response column name not found in dataset, does your data have a\n"
      " header line with the column names?" << endl;
    exit(0);
  }

  // We now have the column count and column names.  Read the numbers.
  // Allocate space to hold the data
  int lineCount = getLineCount(filename);
  int colCount = getColumnCount(headers);
  p->dataLineCount = lineCount;
  double **data = new double*[lineCount - 1];
  for (int i = 0; i < lineCount; i++) {
    data[i] = new double[colCount];
  }
  // Read in the data
  getData(filename, data);

  // END OF READING INPUT FILE

  // alpha is not even used
  // double* y = getResponseData(response, headers, data, lineCount-1);
  // double mean, risk;
  // anovaSS(y, lineCount-1, mean, risk);
  // alpha = cp * risk;
  // free1DData(y);

  // NOW WE MAY NEED TO SPLIT INTO TRAING AND TESTING SETS
  double **testData, **trainData;
  int numObs = lineCount - 1;
  int testSize = 0, trainSize = numObs;

  cout<<"test_data_filename="<<test_data_filename<<endl;
  
  if(test_data_filename == "")
    {
      // If they did not give us a test data file, then split the input data 80/20

      if(!randomSplit)
	{
	  cerr << "Error: 'splitdata' set to false and no test data file supplied." << endl;
	  exit(11);
	}
      
      testSize = numObs/5;  // either way test data will be 20% of total
      trainSize = numObs - testSize;

      // create an index into the data rows
      vector<int> index(numObs);
      for(int i = 0;i<numObs;i++)
	index[i]=i;

      // if the want it randomized, then do the shuffle, not the twist or the tango
      if(randomSplit)
	{
	  random_device rd;      
	  mt19937 g(rd());
	  shuffle(index.begin(), index.end(), g);
	}

      trainData = new double*[trainSize];
      testData = new double*[testSize];
      // now just go through the index and assign each row to either
      // testing or training.
      for(int i = 0;i<numObs;i++)
	{
	  if(i<trainSize)
	    trainData[i] = data[index[i]];
	  else
	    testData[i-trainSize] = data[index[i]];
	}
    }
  else
    {   // IF they gave us a test data file, then read it in...
      if(split_data)
	{
	  cerr << "Warning: 'splitdata' set to true, but test data file '"<<
	    test_data_filename<<"' also specified." << endl;
	}
      ifstream tfin;
      tfin.open(test_data_filename);
      if (!tfin.is_open())
	{
	  cout << "Test data file \"" << test_data_filename << "\" does not exist." << endl;
	  exit(0);
	}

      string theaders;
      // bool tresponseFound = false;
      getline(tfin, theaders);
      if(headers != theaders)
	{
	  cout << "Error: headers for test data file do not match headers for training data file." << endl;
	  exit(0);
	}

      int tlineCount = getLineCount(test_data_filename);
      int tcolCount = getColumnCount(theaders);

      testData = new double*[tlineCount - 1];
      for (int i = 0; i < tlineCount-1; i++)
	{
	  testData[i] = new double[tcolCount];
	}
      getData(test_data_filename, testData);
      trainData = deepCopyData(data, lineCount - 1, colCount);
      testSize = tlineCount - 1;
      trainSize = numObs;
    }

  // get number of classes
  int numclasses = 0;
  if(m == GINI) { //classification problem
    vector<double> classes;
    int respCol = getResponseColumnNumber(response, headers);
    // get all unique classes in training data
    for(int i = 0; i < trainSize; i++) {
      double c = trainData[i][respCol];
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
      double c = testData[i][respCol];
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
  // p->alpha = alpha;
  p->method = m;
  p->numclasses = numclasses;
  p->verbose = verbose;

  for (int i = 0; i < lineCount; i++) {
    p->where[i] = 0;	
  }

  if(p->splitdata && test_data_filename == "") {
    numObs /= 5;
  } 
}

// node buildTree(params * p, int numObs, int &numNodes)
// {
//     // node tree;
//     // tree.numObs = numObs;
//     // tree.data = deepCopyData(p->trainData, numObs, getColumnCount(p->headers));

//     // double risk;
//     // partition(p, &tree, 1, risk);
//     // //freeTreeData(&tree);
//     // return tree;
// }

// void fixTree(node * n, double cpScale, int nodeId, int &nodeCount, vector<int> &iNode)
// {
//     n->cp *= cpScale;
//     n->nodeId = nodeId;
//     nodeCount += 1;
//     iNode.push_back(nodeId);

//     if (n->leftNode != NULL) {
//         fixTree(n->leftNode, cpScale, 2 * nodeId, nodeCount, iNode);
//     }
//     if (n->rightNode != NULL) {
//         fixTree(n->rightNode, cpScale, (2 * nodeId) + 1, nodeCount, iNode);
//     }
// }

// cpTable *buildCpTable(node *root, params *p)
// {
//     cpTable *cpTableHead = new cpTable(), *currCpTable, *tempCpTable, *prevCpTable;
//     cpTable *cpTail = new cpTable();

//     cout << "Building CP Table..." << endl;
//     vector<double> cpList;
//     cpList.push_back(root->cp);
//     double parentCp = root->cp;
//     int uniqueCp = 2;

//     makeCpList(root, parentCp, cpList, uniqueCp, p);
//     sort(cpList.begin(), cpList.end());
//     reverse(cpList.begin(), cpList.end());
//     p->uniqueCp = cpList.size();

//     // make linked list
//     cpTableHead->back = new cpTable();
//     currCpTable = cpTableHead;
//     prevCpTable = cpTableHead->back;
//     vector<double>::iterator it;
//     for (it = cpList.begin(); it != cpList.end(); it++) {
//         double cp = *it;
//         currCpTable->cp = (double) cp;
//         if (cp != cpList.at(cpList.size() - 1)) {
//             currCpTable->forward = new cpTable;
//         }
//         if (&prevCpTable != NULL) {
//             currCpTable->back = prevCpTable;
//         }
//         prevCpTable = currCpTable;
//         currCpTable = currCpTable->forward;
//     }
//     cpTail = prevCpTable;

//     makeCpTable(root, parentCp, 0, cpTail);
//     tempCpTable = cpTableHead;

//     // cross validations
//     if (p->numXval && p->runXVals) {
//         cout << "Running cross-validations..." << endl;
//         vector<int> groups;
//         for(int i = 0; i < root->numObs; i++) {
//             groups.push_back(i % p->numXval);
//         }
//         shuffle(groups.begin(), groups.end(), default_random_engine(time(NULL)));
//         int *xGrps = &groups[0];
//         xval(cpTableHead, xGrps, *p);
//     }

//     tempCpTable = cpTableHead;
//     double scale = 1 / root->dev;
//     while (tempCpTable != NULL) {
//         tempCpTable->cp *= scale;
//         tempCpTable->risk *= scale;
//         tempCpTable->xstd *= scale;
//         tempCpTable->xrisk *= scale;

//         tempCpTable = tempCpTable->forward;
//     }
//     cpTableHead->risk = 1.0;

//     return cpTableHead;
// }


// double getPrediction(node *tree, double row[], int responseCol)
// {
//     /* some leaves (all leaves?) don't have spltVar, splitPoint, etc. set*/
//     if(tree->leftNode == NULL && tree->rightNode == NULL) {
//         return tree->yval;
//     }

//     int splitVar = tree->varIndex;
//     double splitPoint = tree->splitPoint;
//     int direction = tree->direction;
//     double predValue = 0;

//     if(row[splitVar] > splitPoint) {
//         direction *= -1;
//     }
//     if(direction < 0) {
//         if(tree->leftNode == NULL) {
//             return tree->yval;
//         } else {
//             predValue = getPrediction(tree->leftNode, row, responseCol);
//         }
//     } else {
//         if(tree->rightNode == NULL) {
//             cout << tree->yval << endl;
//             return tree->yval;
//         } else {
//             predValue = getPrediction(tree->rightNode, row, responseCol);
//         }
//     }

//     return predValue;
// }

