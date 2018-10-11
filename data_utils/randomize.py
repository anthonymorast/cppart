import pandas as pd
import sys


def usage():
    print("python3 randomize.py <data filename>")


if __name__ == '__main__':
    if(len(sys.argv) < 2):
        usage()
        exit(0)
    
    filename = sys.argv[1]
    df = pd.read_csv(filename)
    df = df.sample(frac=1).reset_index(drop=True)
    df.to_csv(filename, index=False)
