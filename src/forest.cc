#include <forest.h>

params *Forest::p;

Forest::Forest(DataTable *d, int numTree)
{
	data = d;
	numTrees = numTree;
	setMetric();
}

Forest::~Forest()
{
	for(vector<Node>::iterator it = trees.begin(); it != trees.end(); it++)
		delete &(*it);
}

void Forest::build()
{
	Node::setMetric(metric);
	Node::setMinNodeData(p->minNode);
	Node::setMinObsData(p->minObs);
	Node::setDelays(p->delayed);

	double mean,cp;
	for(unsigned int i = 0; i < numTrees; i++)
	{
		DataTable *bootstrap = getBootstrapSample();
		metric->getSplitCriteria(bootstrap,&mean,&cp);
		Node *n = new Node(NULL, bootstrap, mean, cp, 0);
		n->setMaxDepth(p->maxDepth);
		n->setId();
		trees.push_back(*n);
	}

	// should never happen
	if(trees.size() != numTrees)
	{
		cout << "Number of trees in forest does not match specified amount." << endl;
		exit(0);
	}
}

void Forest::train()
{
	// for each tree in vector, build tree
	int i = 0;
	for(vector<Node>::iterator it = trees.begin(); it != trees.end(); it++)
	{	
		double mean,cp;
		metric->getSplitCriteria((*it).getData(),&mean,&cp);
		Node::setAlpha(p->complexity * cp);		// this will be different for each tree, need to set here
		if(p->verbose > 0)
			cout << "Training tree " << (i+1) << " out of " << numTrees << "..." << endl;
		(*it).split(0);
		i++;
	}
}

float Forest::predict(double *sample)
{
	float pred = 0;
	if(p->method == GINI)
		pred = classPredict(sample);
	else 
		pred = contPredict(sample);
	return pred;
}


/******** Private *********/
float Forest::classPredict(double *sample)
{
	// take most popular class from all trees
	int max = 0;
	float pred = 0;
	map<float, int> freq;
	for(vector<Node>::iterator it = trees.begin(); it != trees.end(); it++)
	{
		// get each tree's guess
		float guess = (*it).predict(sample);
		if(freq.count(guess))
			freq.at(guess)++;
		else
			freq.insert(pair<float,int>(guess, 1));
	}
	
	for(auto it = freq.cbegin(); it != freq.cend(); it++)
	{
		if(it->second > max)
		{
			max = it->second;
			pred = it->first;
		}
	}
	
	return pred;
}

float Forest::contPredict(double *sample)
{	
	// take the average of all trees' predictions
	float pred = 0;

	for(vector<Node>::iterator it = trees.begin(); it != trees.end(); it++)
		pred += (*it).predict(sample);

	return (pred/((float)numTrees));
}

DataTable* Forest::getBootstrapSample()
{
	return data;
}

void Forest::setMetric()
{
	if(p->method == GINI) 
		metric = new giniMetric;
	else
		metric = new anovaMetric;
}

