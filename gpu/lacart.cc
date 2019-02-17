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
#include <thread>
#include <omp.h>
using namespace std;


int main(int argc, char* argv[]) {
	clock_t start, end;

	if (argc < 4) {
		printUsage(argv,argc);
		exit(0);
	}

	params p;
	//cout << endl;

	parseParameters(argv, argc, &p);
	
	if(p.verbose>0)
		cerr<<"back from parseParameters"<<endl;

#ifdef _OPENMP
	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
	// printf("Running with %d threads.\n",concurentThreadsSupported);
	omp_set_num_threads(concurentThreadsSupported);
#endif

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
	start = clock();

	//int numNodes = 0;
	//cout << "Building tree..." << endl;

	statisticalMetric *metric;
	if(p.method == GINI) 
		metric = new giniMetric;
	else
		metric = new anovaMetric;
	double mean,cp;

	metric->getSplitCriteria(Data,&mean,&cp);

	Node::setMetric(metric);
	Node::setMinNodeData(p.minNode);
	Node::setMinObsData(p.minObs);
	Node::setAlpha(p.complexity * cp);
	Node::setDelays(p.delayed);
	Node::setVerbose(p.verbose);

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
	//cout << "Creating tree file '" << treeFileName << "'..." << endl;
	ofstream fout;
	fout.open(treeFileName);
	tree->print(fout, false);
	fout.close();

	double mae = 0, relError = 0;
	int correct = 0, incorrect = 0;
    float impurity = 0;

    tree->getImpurity(impurity);

	for (int i = 0; i < p.testSize; i++) {
	    double *sample = p.testData[i];
	    double pred = tree->predict(sample);
	    double e = abs(pred - sample[0]);
		// cout << pred << " vs. " << sample[0] << endl;
	    if(pred == sample[0]) {
	        correct++;
	    } else {
	        incorrect++;
	    }
	    relError += e/1+abs(sample[0]); // class 0 = nan, need to add 1 to denominator
	    mae += e;
	}

	double error = mae;
	if(p.method == GINI)
		error = correct;

	cout << p.delayed << "," << p.maxDepth << "," << impurity << "," << (relError/p.testSize) << "," << (error/p.testSize) << endl;

	cout << "Time Elapsed: " << ((double)(end-start))/CLOCKS_PER_SEC << endl;
	return 0;
}

