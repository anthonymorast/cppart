/*
 * Entry point of the program. Parses parameters, does some preprocessing,
 * creates the tree, runs cross-validations, creates tree files, etc.
 *
 * Author: Anthony Morast
 */


#include <lacart.h>

#include <fstream>

#include <structures.h>
#include <utils.h>
#include <data_handler.h>
#include <datatab.h>
#include <metric.h>
#include <node.h>
using namespace std;


int main(int argc, char* argv[]) {
	clock_t start, end;

	if (argc < 4) {
		printUsage(argv,argc);
		exit(0);
	}

	params p;
	cout << endl;

	parseParameters(argv, argc, &p);

	if(p.verbose>0)
		cerr<<"back from parseParameters"<<endl;

	// Move response column to column 0
	int respCol = getResponseColumnNumber(p.response, p.headers);
	double tmp;
	string tmpstr;
	for(int i=0;i<p.testSize;i++)
	{
		tmp = p.testData[i][0];
		p.testData[i][0] = p.testData[i][respCol];
		p.testData[i][respCol] = tmp;
	}
	for(int i=0;i<p.trainSize;i++)
	{
		tmp = p.trainData[i][0];
		p.trainData[i][0] = p.trainData[i][respCol];
		p.trainData[i][respCol] = tmp;
	}    
	tmpstr = p.varNames[0];
	p.varNames[0] = p.varNames[respCol];
	p.varNames[respCol] = tmpstr;

	// create data table for training

	DataTable *Data = new DataTable(p.varNames,p.trainData,p.trainSize,getColumnCount(p.headers));

	// if(p.verbose>1)    // training data table:
	//   {
	// 	cout<<"training data:"<<endl;
	// 	Data->dump();
	//   }

	start = clock();

	//int numNodes = 0;
	cout << "Building tree..." << endl;

	anovaMetric *metric = new anovaMetric;
	double mean,cp;

	metric->getSplitCriteria(Data,&mean,&cp);

	Node::setMetric(metric);
	Node::setMinNodeData(p.minNode);
	Node::setMinObsData(p.minObs);
	Node::setAlpha(p.complexity * cp);
	Node::setDelays(p.delayed);

	Node *tree = new Node(NULL,Data,mean,cp,0);
	tree->setMaxDepth(p.maxDepth);
	tree->setId(); 
	tree->split(0);

	// stop the clock before xvals
	end = clock();

	int idx = p.filename.find(".");

	string treeFileName = p.filename.substr(0, idx);
	if(p.delayed > 0)
		treeFileName += ".delayed";
	treeFileName += ".tree";
	cout << "Creating tree file '" << treeFileName << "'..." << endl;
	ofstream fout;
	fout.open(treeFileName);
	tree->print(fout, false);
	fout.close();

	// string cpTableFilename = p.filename.substr(0,idx);
	// if(p.delayed) {
	//     treeFileName += ".delayed";
	//     cpTableFilename += ".delayed";
	// }
	// cpTableFilename += ".cptable";



	// //    printCpTable(cpTableHead, cpTableFilename);

	// cout << endl << endl << "Results: " << endl;
	// uncommented because warnings suck
	cout << "Time elapsed " << ((double)(end - start)) / CLOCKS_PER_SEC << endl;
	// cout << "Test Data: " << endl;
	// double mae = 0;
	// double relError = 0;
	// int correct = 0, incorrect = 0;



	// for (int i = 0; i < p.testSize; i++) {
	//     double *sample = p.testData[i];
	//     int respCol = getResponseColumnNumber(p.response, p.headers);
	//     double pred = getPrediction(&root, sample, respCol);
	//     double e = abs(pred - sample[respCol]);
	//     if(pred == sample[respCol]) {
	//         correct++;
	//     } else {
	//         incorrect++;
	//     }
	//     relError += e/1+abs(sample[respCol]); // class 0 = nan, need to add 1 to denominator
	//     mae += e;
	//     // error += anovaPredict(sample[0], pred); // square error
	//     //cout << "Actual: " << sample[respCol] << "\tPredicted: " <<  pred << endl;
	// }
	// if(p.method == ANOVA) {
	//     cout << "\tMAE: " << mae/p.testSize << endl;
	// } else {
	//     cout << "\tCorrect Classifications: " << correct << endl;
	//     cout << "\tIncorrect Classifications: " << incorrect << endl;
	//     cout << "\tClassification Error (correct/total): " << (double)correct/p.testSize << endl;
	// }
	// cout << "\tRelative Error: " << relError/p.testSize << endl << endl;

	return 0;
}

