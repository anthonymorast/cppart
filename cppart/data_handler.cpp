#include "data_handler.h"

/*
*	Function Implementation
*/
/*
*	Gets the x data, returns the number of observations in the dataset.
*/
void getXData(string filename, string response, string headers,  float** x) {
	ifstream fin;
	fin.open(filename);
	if (!fin.is_open()) {
		cout << "Error opening file " << filename << endl;
		cout << "Does the file exist?" << endl;
		exit(0);
	}

	int responseColumn = getResponseColumnNumber(response, headers);
	string line;
	int row = 0;
	getline(fin, line);
	while (getline(fin, line)) {
		istringstream ss(line);
		string value;
		int curCol = 0;
		while (getline(ss, value, ',')) {
			if (curCol < responseColumn) {
				x[row][curCol] = stof(value);
			}
			else if (curCol > responseColumn) {
				x[row][curCol - 1] = stof(value);
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
void getYData(string filename, string response, string headers, float y[]) {
	ifstream fin;
	fin.open(filename);
	if (!fin.is_open()) {
		cout << "Error opening file " << filename << endl;
		cout << "Does the file exist?" << endl;
		exit(0);
	}

	int responseColumn = getResponseColumnNumber(response, headers);
	string line;
	int row = 0;
	getline(fin, line);
	while (getline(fin, line)) {
		istringstream ss(line);
		string value;
		int colNumber = 0;
		while (getline(ss, value, ',')) {
			if (colNumber == responseColumn) {
				y[row] = stof(value);
			}
			colNumber++;
		}
		row++;
	}

	fin.close();
}


int getResponseColumnNumber(string response, string headers) {
	istringstream ss(headers);
	string value;

	int index = 0;
	while (getline(ss, value, ',')) {
		if (!strcmp(value.c_str(), response.c_str())) {
			break;
		}
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