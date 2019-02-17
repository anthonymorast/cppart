#include <forest.h>

params *Forest::p;

Forest::Forest(DataTable *d, int numTree)
{
    data = d;
    numTrees = numTree;
    setMetric();
    trees = new Node*[numTrees];
}

Forest::~Forest()
{
    delete trees;
}

void Forest::build()
{
    // set static variables on the Node class
    Node::setMetric(metric);
    Node::setMinNodeData(p->minNode);
    Node::setMinObsData(p->minObs);
    Node::setDelays(p->delayed);

    // initialize each tree in the forest
    double mean,cp;
    for(unsigned int i = 0; i < numTrees; i++)
    {
        DataTable *bootstrap = getBootstrapSample();
        metric->getSplitCriteria(bootstrap,&mean,&cp);
        Node *n = new Node(NULL, bootstrap, mean, cp, 0);
        n->setMaxDepth(p->maxDepth);
        n->setId();
        trees[i] = n;
    }
}

void Forest::train()
{
    // create each tree in the forest
    for(unsigned int i = 0; i < numTrees; i++)
    {	
        double mean,cp;
        metric->getSplitCriteria(trees[i]->getData(),&mean,&cp);
        Node::setAlpha(p->complexity * cp);		// this will be different for each tree, need to set here
        if(p->verbose > 0)
            cout << "Training tree " << (i+1) << " out of " << numTrees << "..." << endl;
        trees[i]->split(0);
    }
}

float Forest::predict(double *sample)
{
    // get the average prediction for the forest
    float pred = 0;
    if(p->method == GINI)
        pred = classPredict(sample);
    else 
        pred = contPredict(sample);
    return pred;
}

void Forest::print(ofstream &fout)
{
    // print all the trees in the forest
    for(unsigned int i = 0; i < numTrees; i++)
    {
        trees[i]->print(fout, false);
        fout << endl << endl;
    }
}


/******** Private *********/
float Forest::classPredict(double *sample)
{
    // take most popular class from all trees
    int max = 0;
    float pred = 0;
    map<float, int> freq;
    for(unsigned int i = 0; i < numTrees; i++)
    {
        // get each tree's guess
        float guess = trees[i]->predict(sample);
        if(freq.count(guess))
            freq.at(guess)++;
        else
            freq.insert(pair<float,int>(guess, 1));
    }

    // select the class that was guessed most frequently
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

    for(unsigned int i = 0; i < numTrees; i++)
        pred += trees[i]->predict(sample);

    return (pred/((float)numTrees));
}

DataTable* Forest::getBootstrapSample()
{
    // scikit-learn and R documentations says subsample size should equal dataset size (sample with replacement)
    return data->subSample(data->numRows());
}

void Forest::setMetric()
{
    if(p->method == GINI) 
        metric = new giniMetric;
    else
        metric = new anovaMetric;
}

