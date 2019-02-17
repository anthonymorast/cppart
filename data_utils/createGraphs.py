import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def usage():
    print("python3 createGraphs.py <in file> <out file(no ext)>")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        usage()
        exit(0)

    infile = sys.argv[1]
    outfile = sys.argv[2]

    d1 = 2
    d2 = 3

    df = pd.read_csv(infile)
    greedy = df.query('delayed == '+str(d1))
    delayed = df.query('delayed != '+str(d1))
    depths = df.depth.unique()
    runs = int(greedy.shape[0] / len(depths))
 
    plt.plot('depth', 'acc', data=delayed, marker='o', markerfacecolor='blue', markersize=8, color='blue', label='delayed'+str(d2))
    plt.plot('depth', 'acc', data=greedy, marker='o', markerfacecolor='red', markersize=8, color='red', label='delayed'+str(d1))
    plt.legend()
    plt.xlabel('Depth')
    plt.ylabel('Error')
    plt.title("Error vs. Depth")
    plt.savefig(outfile+'_error.eps')
    plt.gcf().clear()

    plt.plot('depth', 'imp', data=delayed, marker='o', markerfacecolor='blue', markersize=8, color='blue', label='delayed'+str(d2))
    plt.plot('depth', 'imp', data=greedy, marker='o', markerfacecolor='red', markersize=8, color='red', label='delayed'+str(d1))
    plt.legend()
    plt.xlabel('Depth')
    plt.ylabel('Impurity')
    plt.title("Impurity vs. Depth")
    plt.savefig(outfile+'_impurity.eps')
    plt.gcf().clear()
    
    plt.plot('depth', 'rel', data=delayed, marker='o', markerfacecolor='blue', markersize=8, color='blue', label='delayed'+str(d2))
    plt.plot('depth', 'rel', data=greedy, marker='o', markerfacecolor='red', markersize=8, color='red', label='delayed'+str(d1))
    plt.legend()
    plt.xlabel('Depth')
    plt.ylabel('Relative Error')
    plt.title("Rel. Error vs. Depth")
    plt.savefig(outfile+'_relerror.eps')
    plt.gcf().clear()
