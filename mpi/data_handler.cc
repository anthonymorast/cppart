/*
 * Handles data input and manipulation. Gets desired columns, finds
 * response column, etc.
 *
 * Author: Anthony Morast
 */


#include <data_handler.h>

/*
 *	Function Implementation
 */
/*
 *	Gets the x data, returns the number of observations in the dataset.
 */
void getData(string filename,double** x) {
    ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        cout << "Error opening file " << filename << endl;
        cout << "Does the file exist?" << endl;
        exit(0);
    }

    string line;
    int row = 0;
    getline(fin, line);
    while (getline(fin, line)) {
        istringstream ss(line);
        string value;
        int curCol = 0;
        while (getline(ss, value, ',')) {
	  try {
            x[row][curCol] = stod(value);
	  } catch(exception &e) {
	    cout << "Warning: value '"<<value<<"' not parsable as double" << endl;
	  }
	  curCol++;
        }
        row++;
    }

    fin.close();
}

/*
 *	Gets the y data, returns the number of observations in the dataset.
 */
double* getResponseData(string response, string headers, double** data, int numObs) {
    int responseColumn = getResponseColumnNumber(response, headers);
    double *y = new double[numObs];

    for (int i = 0; i < numObs; i++) {
        y[i] = data[i][responseColumn];
    }

    return y;
}

int getResponseColumnNumber(string response, string headers) {
    istringstream ss(headers);
    string value;

    int index = 0;
    while (getline(ss, value, ',')) {
        if (!strcmp(value.c_str(), response.c_str())) {
            break;
        }
        index++;
    }

    return index;
}

int getLineCount(string filename) {
    int count = 0;

    ifstream fin;
    fin.open(filename);

    string line;
    while (getline(fin, line)) {
        count++;
    }

    return count;
}

int getColumnCount(string headers) {
    int count = 0;

    istringstream ss(headers);
    string value;
    while (getline(ss, value, ',')) {
        count++;
    }

    return count;
}

void free1DData(double* y) {
    delete[] y;
    y = NULL;
}

void free2DData(double** data, int numObs) {
    for(int i = 0; i < numObs; i++) {
        delete[] data[i];
    }
    delete[] data;
    data = NULL;
}

double** deepCopyData(double** old_data, int numObs, int colCount) {
    double** new_data = new double*[numObs];
    for(int i = 0; i < numObs; i++) {
        new_data[i] = new double[colCount];
    }
    for(int i = 0; i < numObs; i++) {
        for (int j = 0; j < colCount; j++) {
            new_data[i][j] = old_data[i][j];
        }
    }
    return new_data;
}
