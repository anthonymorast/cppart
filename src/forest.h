#include <metric.h>
#include <structures.h>
#include <vector>
#include <map>
#include <metric.h>
#include <node.h>

#include <iostream>
using namespace std;

class Forest{
	vector<Node> trees;
	DataTable *data;
	unsigned int numTrees = 0;	
	static params *p;
	statisticalMetric *metric;
	
public:
	Forest(DataTable *d, int numTree);
	~Forest();

	void build();
	void train();
	float predict(double *sample);

	static void setParams(params *parms){p = parms;}

private:
	float contPredict(double *sample);
	float classPredict(double *sample);
	DataTable* getBootstrapSample();
	void setMetric();
};

