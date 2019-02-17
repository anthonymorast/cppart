/*
 * Entry point of the program. Parses parameters, does some preprocessing,
 * creates the tree, runs cross-validations, creates tree files, etc.
 *
 * IMPORTANT: In order for the runAnd* shell scripts to work the entire
 * program cannot ouput anything except the output statement at the end 
 * of thread_0's main function, i.e. 
 *
 * cout << p.delayed << "," << p.maxDepth << "," << impurity << "," << (relError/p.testSize) << "," << (error/p.testSize) << endl;
 *
 * This output is redirected to a CSV file that is processed by the python
 * scripts. Any other output will cause errors. 
 *
 * Author: Anthony Morast
 */
#include <lacart.h>

int main(int argc, char* argv[]) {
    //clock_t start, end;

    int rank, procs, name_len;
    char name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Get_processor_name(name, &name_len);

    const int done_tag = 100;
    const int send_struct_tag = 1;	// tell slave nodes they have data
    const int send_x_tag = 4;		// send the x data (should combine with struct)
    const int send_y_tag = 5;		// send the y data (should combine with struct)
    const int results_tag = 2;		// tell master node we have results

    // create MPI types for structs
    const int nitems_send = 2;
    const int nitems_resp = 5;

    int blocklengths[2] = {1, 1}; // probbaly used to send array (set block legnth = array length [not sure though])
    MPI_Datatype types[2] = {MPI_INT, MPI_INT};
    MPI_Datatype send_data;
    MPI_Aint offsets[nitems_send];
    offsets[0] = offsetof(mpi_send, column);
    offsets[1] = offsetof(mpi_send, nrows);
    MPI_Type_create_struct(nitems_send, blocklengths, offsets, types, &send_data);

    int bl[5] = {1, 1, 1, 1, 1};
    MPI_Datatype t[5] = {MPI_INT, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype resp_data;
    MPI_Aint os[nitems_resp];
    os[0] = offsetof(mpi_resp, column);
    os[1] = offsetof(mpi_resp, sse);
    os[2] = offsetof(mpi_resp, improve);
    os[3] = offsetof(mpi_resp, leftSS);
    os[4] = offsetof(mpi_resp, rightSS);
    MPI_Type_create_struct(nitems_resp, bl, os, t, &resp_data);

    MPI_Type_commit(&send_data);
    MPI_Type_commit(&resp_data);

    if (argc < 4) {
        printUsage(argv,argc);
        exit(0);
    }

    // might be a better way to do this but for now parse the parameters
    // on each node so that each machine has a copy of 'p' in its memory
    params p;
    parseParameters(argv, argc, &p);
    statisticalMetric *metric;
    if(p.method == GINI) 
        metric = new giniMetric;
    else
        metric = new anovaMetric;

    // time the program
    double start = MPI_Wtime();
    if(rank == 0) 
    {	
        // handle thread admin stuff
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

        DataTable *Data = new DataTable(p.varNames,p.trainData,p.trainSize,getColumnCount(p.headers));
        //start = clock();

        double mean,cp;
        metric->getSplitCriteria(Data,&mean,&cp);

        Node::setMetric(metric);
        Node::setMinNodeData(p.minNode);
        Node::setMinObsData(p.minObs);
        Node::setAlpha(p.complexity * cp);
        Node::setDelays(p.delayed);
        Node::setVerbose(p.verbose);

        // build the tree
        Node *tree = new Node(NULL,Data,mean,cp,0);
        tree->setMaxDepth(p.maxDepth);
        tree->setId(); 
        tree->split(0);

        // print tree to a file
        int idx = p.filename.find(".");
        string treeFileName = p.filename.substr(0, idx);
        if(p.delayed > 0)
            treeFileName += ".delayed";
        treeFileName += ".tree";
        ofstream fout;
        fout.open(treeFileName);
        tree->print(fout, false);
        fout.close();

        // get error measurments and impurity
        double mae = 0, relError = 0;
        int correct = 0, incorrect = 0;
        float impurity = 0;

        tree->getImpurity(impurity);

        // get mae, rel_error
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

        // this needs to output just like this in order for the scripts to work properly
        cout << p.delayed << "," << p.maxDepth << "," << impurity << "," << (relError/p.testSize) << "," << (error/p.testSize) << endl;

        for(int i = 1; i < procs; i++)
        {
            int dummy = 1;
            MPI_Send(&dummy, 1, MPI_INT, i, done_tag, MPI_COMM_WORLD);
        }
    }
    else 
    {
        MPI_Status status;
        int dummy;

        // get the first status, the node will either do a split or stop waiting
        MPI_Recv(&dummy, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        while(status.MPI_TAG != done_tag) {
            // get all data from the master node
            struct mpi_send snd;
            MPI_Recv(&snd, 1, send_data, 0, send_struct_tag, MPI_COMM_WORLD, &status);

            double *x = new double[snd.nrows];
            double *y = new double[snd.nrows];

            MPI_Recv(x, snd.nrows, MPI_DOUBLE, 0, send_x_tag, MPI_COMM_WORLD, &status);
            MPI_Recv(y, snd.nrows, MPI_DOUBLE, 0, send_y_tag, MPI_COMM_WORLD, &status);

            // combine the x and y columns and create data table s.t. col0 = y and col1 = x;
            double **d = new double*[snd.nrows];
            for(int i = 0; i < snd.nrows; i++)
                d[i] = new double[2];
            for(int i = 0; i < snd.nrows; i++)
            {
                d[i][0] = y[i];
                d[i][1] = x[i];
            }
            string names[2] = {"resp", "explain"};
            DataTable *tbl = new DataTable(names, d, snd.nrows, 2);
            tbl->sortBy(1);

            // call function to find split point	
            int where, dir;
            double splitPoint, improve = 0;
            double leftMean, rightMean;
            double leftSS, rightSS, totalSS = 0;
            queue<DataTable*> q;
            DataTable *lft, *rgt;
    
            // find the initial split for the dataset
            metric->findSplit(tbl, 1, where, dir, splitPoint, improve, p.minNode);
            if (dir < 0) {
                lft = tbl->subSet(0,where);
                rgt = tbl->subSet(where+1,tbl->numRows()-1);
            } else {
                lft = tbl->subSet(where+1, tbl->numRows()-1);
                rgt = tbl->subSet(0, where);
            }

            // do the delayed splitting
            q.push(lft); q.push(rgt);
            unsigned int stopSize = pow(2, p.delayed+1);
            while(q.size() < stopSize)
            {
                DataTable *temp = q.front(), *l = NULL, *r = NULL;

                dsplit(temp, l, r, metric, p.minNode, p.delayed);

                if(l == NULL && r == NULL)
                {
                    // if there isn't enough data to split on, just use the SSE of the smallest possible partitions 
                    stopSize--;
                    q.push(q.front());
                }
                else 
                {
                    q.push(l); q.push(r);
                }
                q.pop();
            }
            if(q.size() != stopSize) 
            {
                cout << "Error splitting node, queue does not contain the right amount of partitions.";
                cout << endl;
                exit(0);
            }
            while(!q.empty())
            {
                // sum up SSE for each partition
                DataTable *temp = q.front();
                q.pop();
                metric->getSplitCriteria(temp, &leftMean, &leftSS);
                totalSS += leftSS;
            }

            // get split statistics to decide on best split
            metric->getSplitCriteria(lft,&leftMean,&leftSS);
            metric->getSplitCriteria(rgt,&rightMean,&rightSS);

            struct mpi_resp resp;
            resp.column = snd.column;
            resp.improve = improve;
            resp.sse = totalSS;
            resp.leftSS = leftSS;
            resp.rightSS = rightSS;
            MPI_Send(&resp, 1, resp_data, 0, results_tag, MPI_COMM_WORLD);

            delete lft;
            delete rgt;
            delete tbl;
            delete x;
            delete y;

            // keep receiving data until we get the done_tag value, at which point we'll exit
            MPI_Recv(&dummy, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
    }


    if(rank == 0)
    {
        double end = MPI_Wtime();
        //printf("Time elapsed is %f\n", (end-start));
    }

    MPI_Finalize();
    return 0;
}

void dsplit(DataTable *temp, DataTable *&l, DataTable *&r, statisticalMetric *metric, int minNode, int delays)
{
    // this is taken from the Node class, more or less, so the delayed splitting can be done outside of the 
    // Node class.
    int cols = temp->numCols();
    double bestSS = DBL_MAX;
    DataTable *ltab,*rtab;

    for (int curCol = 1; curCol < cols; curCol++)
    {
        // sort by current column
        temp->sortBy(curCol);

        int where, dir;
        double splitPoint, improve;
        double leftMean,rightMean;
        double leftSS, rightSS, totalSS;

        metric->findSplit(temp, curCol, where, dir, splitPoint, improve, minNode);
        // I would have swapped rtab and ltab, but... whatever.
        ltab = temp->subSet(0,where);
        rtab = temp->subSet(where+1,temp->numRows()-1);

        metric->getSplitCriteria(ltab,&leftMean,&leftSS);
        metric->getSplitCriteria(rtab,&rightMean,&rightSS);

        totalSS = leftSS + rightSS;

        if ((improve>0) && (totalSS<bestSS))
        {
            bestSS = totalSS;
            r = rtab;
            l = ltab;
        }
    }
}
