"""
    Converts the data found at https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/
    to csv format. The format of the libsvm data is

    label col#:value col#:value ...

    If the value above is 0, the column is excluded and the column # is one based,
    i.e. column 203 = 202 in other MNIST datasets starting at pixel 0.


    Author: Anthony Morast
"""

import sys
import csv

class sample(object):
    def __init__(self, max_col):
        """
            Holds the class (label) and the feature values. max_col is the max
            column value for each dataset not counting the label value. For example,
            MNIST will have 784 pixel values so the max_col param = 784. Setting
            each value to 0 to start with saves processing time as 0 values are
            excluded from the libsvm datasets. The other values will be filled in
            by using the value before the colon (column #) - 1 in the libsvm files.
        """
        self.label = -1
        self.columnValues = [0 for i in range(max_col)]
        self.max_col = max_col

    def toList(self):
        """
            Return a list starting with the label, to be output to csv.
        """
        return [self.label] + [val for val in self.columnValues]


def printUsage():
    print("python3 libsvmToCsv.py <filename> <max_col> ")
    print("See comments in 'sample' class for max_col info.")


def libsvmToRowList(filename, max_col):
    """
        Create list of lists, easy to output with csv library.
    """
    rows = []
    with open(filename) as f:
        content = f.readlines()
    content = [x.strip() for x in content]

    for row in content:
        s = sample(max_col)
        row = ' '.join(row.split()) # replace multiple spaces with single space
        row = row.replace(": ", ":") # some had spaces after colon
        row = row.split(' ')
        s.label = float(row[0])
        if int(s.label) == s.label:
            s.label = int(s.label)
        row = row[1:]
        for val in row:
            col, value = val.split(':')
            col = int(col)
            s.columnValues[col-1] = float(value)
        rows.append(s.toList())

    return rows


def writeCsv(filename, rows, labels):
    """
        Outputs list of lists from libsvmToRowList(...) to csv file with name
        filename.
    """
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f, quoting=csv.QUOTE_NONE)
        writer.writerow(labels)
        writer.writerows(rows)


if __name__ == '__main__':
    if len(sys.argv) < 3:
        printUsage()
        exit()

    filename = sys.argv[1]
    max_col = int(sys.argv[2])
    output_filename = filename + '.csv'
    rows = libsvmToRowList(filename, max_col)

    """ Change for every dataset!!! """
    labels = ['response'] + ['col' + str(i) for i in range(max_col)]
    writeCsv(output_filename, rows, labels)
