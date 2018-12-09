import pandas as pd
import os
import sys

def usage():
    print("python3 resultsToMulti.py <multifile> <greedy results file> <delayed results file>")


if __name__ == '__main__':
    args = sys.argv
    if len(args) < 4:
        usage()
        exit()

    multi = pd.read_csv(args[1])
    greedy = pd.read_csv(args[2])
    delayed = pd.read_csv(args[3])

    results = delayed.append(greedy)
    multi.iloc[:, 1:len(multi)] = results.iloc[:, 0:len(results)].values
    multi['depth'] = multi['depth'].astype('int8')
    multi['delayed'] = multi['delayed'].astype('int8')
    multi.to_csv(args[1], index=False)

