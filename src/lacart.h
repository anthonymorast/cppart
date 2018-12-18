#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include <datatab.h>
#include <float.h>
#include <structures.h>
#include <utils.h>
#include <data_handler.h>
#include <metric.h>
#include <node.h>
#include <thread>
#include <omp.h>
#include <mpi.h>
#include <stddef.h>
using namespace std;


void dsplit(DataTable *temp, DataTable *&l, DataTable *&r, statisticalMetric *metric, int minNode, int delays);
