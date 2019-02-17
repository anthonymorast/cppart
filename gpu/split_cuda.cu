#include "split_cuda.h"

/* 
   fills improve, sse, leftsse, and rightsse arrays with values from splitting
   on each column, i.e. each of the arrays is of size numCols()
*/
__global__ void get_split(float **x, float *y, int *col, float *sse, 
                          float *rightSse, float *leftSse, int *method) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    *col = idx;

    // can we pass in function pointers for this part?
    int *data = x[idx];
    switch(method) {
        case 0: // gini
            // do gini split
            break;
        case 1: // anova
            // do anova split
            break;
        default: // error
            break;
    }
}


/* uses a gpu to split on all columns at once. 
 * this function returns a column number. the calling
 * function will need to re-split on the column to
 * get the other data, for the time being
 */
int split(datatable *tab) {
    // get x and y data into array format to be referenced in device function

    // send all columns to device

    // aggregate results and select best split

    // was going to return best column and re-split but if we make 
    // SSE, leftSSE, improve, rightSSE arrays we could just return those
    // and use the correct column
    return 0;
} 
