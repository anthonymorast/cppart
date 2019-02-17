"""
    Takes a file with multiple runs of a randomized datasets.
    Averages the information for each depth/delay number and ouputs
    the result.
"""
import sys
import pandas as pd

def usage():
    print('python3 processMulti.py <multi filename>')

if __name__ == '__main__':
    if(len(sys.argv) < 2):
        usage()
        exit(0)

    d1 = 2
    d2 = 3

    df = pd.read_csv(sys.argv[1])
    greedy = df.query('delayed == '+str(d1))
    delayed = df.query('delayed != '+str(d1))
    depths = df.depth.unique()
    runs = int(greedy.shape[0] / len(depths))

    cols = ['dataset', 'depth', 'delayed', 'imp', 'rel', 'acc']
    rows = []
    for d in depths:
        dres = delayed.loc[delayed['depth'] == d]
        gres = greedy.loc[greedy['depth'] == d]

        drow = [dres.iloc[0][0]]
        drow.append(d)
        drow.append(d2)
        drow.append(dres['impurity'].sum()/runs)
        drow.append(dres['relerror'].sum()/runs)
        drow.append(dres['accuracy'].sum()/runs)

        grow = [gres.iloc[0][0]]
        grow.append(d)
        grow.append(d1)
        grow.append(gres['impurity'].sum()/runs)
        grow.append(gres['relerror'].sum()/runs)
        grow.append(gres['accuracy'].sum()/runs)

        rows.append(grow)
        rows.append(drow)

    odf = pd.DataFrame(rows, columns=cols)
    odf.to_csv(sys.argv[1][:-4]+'.results.csv', index=False)
