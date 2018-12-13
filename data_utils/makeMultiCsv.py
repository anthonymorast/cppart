"""
    Creates a CSV file that can be filled with results for datasets
    not broken into test and train datasets. These CSVs (once filled in)
    can be imported into a separate script to get results.
"""
import sys 
import csv

def printUsage():
    print("python3 makeMultiCsv.py <dataset_name> [optional: {big, small}")
    print("big/small - depths to use big=[6, 8, 10, 12, 14, 16, 18] small=[1, 2, 3, 4, 5, 6]")


if __name__ == '__main__':
    if(len(sys.argv) < 2):
        printUsage()
        exit(0)

    depths_to_use = 'big'
    if len(sys.argv) == 3:
        depths_to_use = sys.argv[2]

    name = sys.argv[1]
    delays = [0,1]
    depths = [6, 8, 10, 12, 14, 16, 18]
    if depths_to_use == 'small':
        depths = [1, 2, 3, 4, 5, 6] 
    headers = ['dataset','delayed','depth','impurity','relerror','accuracy']
    separator = ','

    print(depths)

    f = open(name+'Multi.csv', 'w', newline='')
    writer = csv.writer(f, delimiter=separator, quotechar='', quoting=csv.QUOTE_NONE)
    writer.writerow(headers)
    for d in delays:
        for depth in depths:
            for i in range(5):
                row = [name, '']
                writer.writerow(row)
    f.close()
