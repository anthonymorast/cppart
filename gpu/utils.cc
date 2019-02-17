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
	} catch(exception &e) {
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
	int maxDepth = 30;
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
			int pos_max_depth = param.find("maxdepth=");

			if(pos_xval != (int)string::npos) {
				try {
					xvals = stoi(string(1, argv[i][pos_xval+9]));
				} catch (exception &e) {
					cout << "Warning: error parsing integer value for cross-validation flag..." << endl;
				}
			} else if(pos_split != (int)string::npos) {
				try {
					split_data = stoi(string(1, argv[i][pos_split+10]));
				} catch (exception &e) {
					cout << "Warning: error parsing integer value for split data flag..." << endl;
				}
			} else if(pos_test != (int)string::npos) {
				test_data_filename = param.substr(pos_test+9);
			} else if(pos_rand != (int)string::npos) {
				try {
					randomSplit = stoi(string(1, argv[i][pos_rand+12]));
				} catch(exception &e) {
					cout << "Warning: random split integer cannot be parsed." << endl;
				}
			} else if(pos_cp != (int)string::npos) {
				try {
					cp = stod(param.substr(pos_cp+3)); 
				} catch(exception &e) {
					cout << "Warning: cp parameter not parsable as double, dedault 0.01 used." << endl;
				}
			} else if(pos_meth != (int)string::npos) {
				try {
					string val = param.substr(pos_meth+7);
					transform(val.begin(), val.end(), val.begin(), ::tolower);
					if (val == "gini") {
						m = GINI;
					}
				} catch(exception &e) {
					cout << "Warning: method parameter not parsable, defaulting to anova." << endl;
				}
			} else if (pos_verbose != (int)string::npos) {
				try {
					verbose = stoi(string(1, argv[i][pos_verbose+8]));
				} catch(exception &e) {
					cout << "Warning: verbose parameter no parsable integer, defaulting to 0." << endl;
				}
			} else if (pos_max_depth != (int)string::npos){
				try {
					maxDepth = stoi(param.substr(pos_max_depth+9));
				} catch(exception &e) {
					cout << "Warning: unable to parse max depth parameter, defaulting to 30." << endl;
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
	double **data = new double*[lineCount];
	for (int i = 0; i <= lineCount; i++) {
		data[i] = new double[colCount];
	}
	// Read in the data
	getData(filename, data);
	// END OF READING INPUT FILE

	// NOW WE MAY NEED TO SPLIT INTO TRAING AND TESTING SETS
	double **testData = NULL, **trainData;
	int numObs = lineCount - 1;
	int testSize = 0, trainSize = numObs;

	if(test_data_filename == "" && split_data)
	{
		// If they did not give us a test data file, then split the input data 80/20
		// AM: need to include split_data check, if not true then don't split data
		// and don't check the accuracy/MAE. Justs builds a full tree.
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
	else if (test_data_filename != "")
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
	else 
	{
		trainData = deepCopyData(data, lineCount-1, colCount);
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
	p->maxDepth = maxDepth;
	p->maxNodes = (int)pow(2, (p->maxDepth)) - 1;
	p->minObs = 20;
	p->minNode = 7;
	p->numXval = 10;
	p->delayed = delayed;
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
	p->method = m;
	p->numclasses = numclasses;
	p->verbose = verbose;

	for (int i = 0; i < lineCount; i++) {
		p->where[i] = 0;	
	}

	if(p->splitdata && test_data_filename == "") {
		numObs /= 5;
	}
   return;	
}

