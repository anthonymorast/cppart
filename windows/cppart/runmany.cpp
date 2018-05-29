/*
 * Runs the cross-validations many times after building the tree
 * so that an average R^2 value can be found. 
 *
 * Author: Anthony Morast
 */

// Includes
#include "utils.h" 		// parse params, build tree, xvals, tons of other goodies
#include <pthread.h>	// run many cross-validations at once.

// Structures 
struct thread_params {
    params *p;
    node *root;
};

// Function Definitions
node* copyTree(node *root);
void* threadedCpTable(void *p);


void printUsage() {
    cout << "Usage:" << endl;
    cout << "\t./runmany <data filename> <response> <delayed {0,1}> <number iterations>" << endl;
}

void* threadedCpTable(void *p) {
    struct thread_params *tp = (struct thread_params*)p;
    cpTable *cpTableHead = buildCpTable(tp->root, tp->p);
}

int main(int argc, char *argv[]) {
    clock_t start, end;
    start = clock(); // may need to change for threading

    if(argc < 5) {
        printUsage();
        return 0;
    }
    params p;
    int numObs = parseParameters(argv, &p);
    int numNodes = 0;
    node root = buildTree(&p, numObs, numNodes);

    struct thread_params *tp = new thread_params;
    tp->p = &p;
    tp->root = &root;

    float avgRSquared = 0;
    int iters = stoi(argv[4]);

    // need to do something similar to PBT python implementation to get around
    // memory leak for now. Start processes to get R^2 values, process finishes, 
    // restarts itself, reads params from input file, output results to output file.
    for(int i = 0; i < iters; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, threadedCpTable, (void*)tp);
        pthread_join(thread, NULL);
    }

    return 0;
}
