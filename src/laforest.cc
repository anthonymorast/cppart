#include <forest.h>
#include <laforest.h>

int main(int argc, char* argv[]) {
	srand(time(NULL));
	if (argc < 4) {
		printUsage(argv,argc);
		exit(0);
	}

	// parse params same as trees
	params p = getParams(argv, argc);
	DataTable *Data = new DataTable(p.varNames,p.trainData,p.trainSize,getColumnCount(p.headers));

	// create the forest
	Forest::setParams(&p);
	Forest *forest = new Forest(Data, 20);
	forest->build();
	forest->train();

	int idx = p.filename.find(".");
	string treeFileName = p.filename.substr(0, idx);
	if(p.delayed > 0)
		treeFileName += ".delayed";
	treeFileName += ".forest";
	ofstream fout;
	fout.open(treeFileName);
	forest->print(fout);
	fout.close();

	double mae = 0, relError = 0;
	int correct = 0, incorrect = 0;

	for (int i = 0; i < p.testSize; i++) {
	    double *sample = p.testData[i];
	    double pred = forest->predict(sample);
	    double e = abs(pred - sample[0]);
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

	cout << "Error: " << (error/p.testSize) << endl;

	return 0;
}

params getParams(char* argv[], int argc)
{
	params p;
	parseParameters(argv, argc, &p);

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

	return p;
}

