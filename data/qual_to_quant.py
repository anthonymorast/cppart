import pandas
import numpy as np
import os
import sys

def usage():
    print("Usage: python3 qual_to_quant.py <filename> <response>")


if __name__ == '__main__':
    args = sys.argv
    if len(args) < 3:
        usage()
        exit(0)
    filename = args[1]
    response = args[2]

    if not filename.lower().endswith('.csv'):
        print("Input file must be a CSV file.")
        exit(0)

    if not os.path.isfile(filename):
        print("File '", filename, "' does not exist.")
        exit(0)

    df = pandas.read_csv(filename)
    df = df.apply(lambda x: x.str.strip() if x.dtype == "object" else x)
    for column in df:
        print("Processing column:", column)
        # if most values are unique or all the values are the same, delete the column since it is not useful
        if (df[column].nunique() / df[column].size > .15 or df[column].nunique() == 1) and column != response:
            print("Dropping column '"+column+"' for being useless.")
            df = df.drop([column], axis=1)
            continue

        # check if numeric
        if not np.issubdtype(df[column].dtype, np.number):
            uniquelst = df[column].unique()
            uniquedict = dict()
            for i in range(len(uniquelst)):
                uniquedict[uniquelst[i]] = i
            df = df.replace({column:uniquedict})

        # impute missing values by taking the average
        if df[column].isnull().values.any():
            df = df.fillna(df.mean())

    newfile = filename[:-4]+'_procd.csv'
    df.to_csv(newfile, sep=',', index=False)
