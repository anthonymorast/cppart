"""
    Creates a CSV file that can be filled with results for datasets
    not broken into test and train datasets. These CSVs (once filled in)
    can be imported into a separate script to get results.
"""
import sys 
import csv

def printUsage():
    print("python3 makeMultiCsv.py <dataset_name>")


if __name__ == '__main__':
    if(len(sys.argv) < 2):
        printUsage()
        exit(0)

    name = sys.argv[1]
    delays = [0,1]
    depths = [6, 8, 10, 12, 14, 16, 18]
    headers = ['dataset','delayed','depth','impurity','relerror','accuracy']
    separator = ','

    f = open(name+'Multi.csv', 'w', newline='')
    writer = csv.writer(f, delimiter=separator, quotechar='|', quoting=csv.QUOTE_MINIMAL)
    for d in delays:
        writer.writerow(headers)
        for depth in depths:
            for i in range(5):
                row = [name, d, depth]
                writer.writerow(row)
    f.close()
