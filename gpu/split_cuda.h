#include "datatab.h"

#include <iostream>
using namespace std;

/* uses a gpu to split on all columns at once. 
 * this function returns a column number. the calling
 * function will need to re-split on the column to
 * get the other data, for the time being
 */
int split(datatable tab); 
