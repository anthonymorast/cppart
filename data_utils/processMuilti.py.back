"""
    Takes a file with multiple runs of a randomized datasets.
    Averages the information for each depth/delay number and ouputs
    the result.
"""
import sys
import csv


def usage():
    print('python3 processMulti.py <multi filename>')

def printResults(greedy, delayed):
    print("Greedy Results:")
    print("\tImpurity\tRel. Err.\tAccuracy")
    #print("\t", greedy[1][0], "\t", greedy[1][1], "\t", greedy[1][2])
    #print("\t", greedy[2][0], "\t", greedy[2][1], "\t", greedy[2][2])
    #print("\t", greedy[3][0], "\t", greedy[3][1], "\t", greedy[3][2])
    #print("\t", greedy[4][0], "\t", greedy[4][1], "\t", greedy[4][2])
    #print("\t", greedy[5][0], "\t", greedy[5][1], "\t", greedy[5][2])
    #print("\t", greedy[6][0], "\t", greedy[6][1], "\t", greedy[6][2])
    print("\t", greedy[6][0], "\t", greedy[6][1], "\t", greedy[6][2])
    print("\t", greedy[8][0], "\t", greedy[8][1], "\t", greedy[8][2])
    print("\t", greedy[10][0], "\t", greedy[10][1], "\t", greedy[10][2])
    print("\t", greedy[12][0], "\t", greedy[12][1], "\t", greedy[12][2])
    print("\t", greedy[14][0], "\t", greedy[14][1], "\t", greedy[14][2])
    print("\t", greedy[16][0], "\t", greedy[16][1], "\t", greedy[16][2])
    print("\t", greedy[18][0], "\t", greedy[18][1], "\t", greedy[18][2])
    print("Delayed Results:")
    print("\tImpurity\tRel. Err.\tAccuracy")
    #print("\t", delayed[1][0], "\t", delayed[1][1], "\t", delayed[1][2])
    #print("\t", delayed[2][0], "\t", delayed[2][1], "\t", delayed[2][2])
    #print("\t", delayed[3][0], "\t", delayed[3][1], "\t", delayed[3][2])
    #print("\t", delayed[4][0], "\t", delayed[4][1], "\t", delayed[4][2])
    #print("\t", delayed[5][0], "\t", delayed[5][1], "\t", delayed[5][2])
    #print("\t", delayed[6][0], "\t", delayed[6][1], "\t", delayed[6][2])
    print("\t", delayed[6][0], "\t", delayed[6][1], "\t", delayed[6][2])
    print("\t", delayed[8][0], "\t", delayed[8][1], "\t", delayed[8][2])
    print("\t", delayed[10][0], "\t", delayed[10][1], "\t", delayed[10][2])
    print("\t", delayed[12][0], "\t", delayed[12][1], "\t", delayed[12][2])
    print("\t", delayed[14][0], "\t", delayed[14][1], "\t", delayed[14][2])
    print("\t", delayed[16][0], "\t", delayed[16][1], "\t", delayed[16][2])
    print("\t", delayed[18][0], "\t", delayed[18][1], "\t", delayed[18][2])


if __name__ == '__main__':
    if(len(sys.argv) < 2):
        usage()
        exit(0)

    rows = []
    with open(sys.argv[1], newline='') as f:
        reader = csv.reader(f, delimiter=',', quotechar='|')
        for row in reader:
            if(row[2] == 'depth'):
                continue
            app = [row[0]] + [float(row[i]) for i in range(1, len(row))]
            rows.append(app)

    depthCol = 2
    impurityCol = 3
    relErrCol = 4
    accuracyCol = 5
    delayedCol = 1
    
    numberRuns = 5

    delayedMap = \
            {6: [0,0,0], 8: [0,0,0], 10: [0,0,0], 
            12: [0,0,0], 14: [0,0,0], 16: [0,0,0], 18: [0,0,0]}
            #{1: [0,0,0], 2: [0,0,0], 3: [0,0,0], 
            #4: [0,0,0], 5: [0,0,0], 6: [0,0,0]}
    greedyMap = \
            {6: [0,0,0], 8: [0,0,0], 10: [0,0,0], 
            12: [0,0,0], 14: [0,0,0], 16: [0,0,0], 18: [0,0,0]}
            #{1: [0,0,0], 2: [0,0,0], 3: [0,0,0], 
            #4: [0,0,0], 5: [0,0,0], 6: [0,0,0]}

    for row in rows:
        if row[delayedCol] == 0:
            greedyMap[row[depthCol]][0] += (row[impurityCol]/numberRuns)
            greedyMap[row[depthCol]][1] += (row[relErrCol]/numberRuns)
            greedyMap[row[depthCol]][2] += (row[accuracyCol]/numberRuns)
        elif row[delayedCol] == 1:
            delayedMap[row[depthCol]][0] += (row[impurityCol]/numberRuns)
            delayedMap[row[depthCol]][1] += (row[relErrCol]/numberRuns)
            delayedMap[row[depthCol]][2] += (row[accuracyCol]/numberRuns)

    printResults(greedyMap, delayedMap) 
