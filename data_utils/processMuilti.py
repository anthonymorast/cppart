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

    df = pd.read_csv(sys.argv[1])
    greedy = df.query('delayed == 0')
    delayed = df.query('delayed == 1')
    depths = df.depth.unique()
    runs = int(greedy.shape[0] / len(depths))

    cols = ['dataset', 'depth', 'delayed', 'imp', 'rel', 'acc']
    rows = []
    for d in depths:
        dres = delayed.loc[delayed['depth'] == d]
        gres = greedy.loc[greedy['depth'] == d]

        drow = [dres.iloc[0][0]]
        drow.append(d)
        drow.append(1)
        drow.append(dres['impurity'].sum()/runs)
        drow.append(dres['rel error'].sum()/runs)
        drow.append(dres['accuracy'].sum()/runs)

        grow = [gres.iloc[0][0]]
        grow.append(d)
        grow.append(0)
        grow.append(gres['impurity'].sum()/runs)
        grow.append(gres['rel error'].sum()/runs)
        grow.append(gres['accuracy'].sum()/runs)

        rows.append(grow)
        rows.append(drow)

    odf = pd.DataFrame(rows, columns=cols)
    odf.to_csv(sys.argv[1][:-4]+'.results.csv', index=False)
