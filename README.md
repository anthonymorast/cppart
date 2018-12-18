# CPPart

A parallel implementation of the CART algorithm for building classification and regression trees in C++. Implements delayed greedy splitting wherein the dataset is paritioned further than a left and right partition before making a splitting decision at each node. 

## Dependencies
I haven't done any work on determining which versions of the following tools are required to make the project work. However, most versions of these libraries should not cause too many problems. 

The following are required to use the Python data processing utilities:
1. [Python3.x](https://www.python.org/downloads/)
2. [Pandas](https://pandas.pydata.org/)
3. [Matplotlib](https://matplotlib.org/)
4. [NumPy](http://www.numpy.org/) (could prbably modify qual_to_quant.py to remove this dependency)

To run the C++ implementation of CART the following dependencies must be met:
1. [Make](https://www.gnu.org/software/make/)
2. [g++](https://gcc.gnu.org/)
3. [MPICH](https://www.mpich.org/)

## Basic Usage

Building the project with the supplied Makefile will create two excutables, **_lacart_** and **_laforest_**.

Running either without parameters, i.e.
```
  ./lacart
```
or 
```
  ./laforest
```
will print information on how to use the program including descriptions of acceptable parameters and acceptable parameter values.

## Structure and Descriptions

```
CPPart
│   README.md
└───data_utils
|   | createGraphs.py - script to ceate Depth vs. {Impurity, Relative Error, Accuracy} plots
│   │ libsvmToCsv.py - converts the format used by LibSVM to a CSV file
|   | makeMultiCsv.py - creates a structured file used to analyze datasets not split into training/testing partitions
|   | processMulti.py - processes the files created by makeMultiCsv.py once they're filled with data
|   | qual_to_quant.py - transforms qualitative variables to quantitative ones (CPPart does not yet handle qualitative variables)
|   | randomize.py - reads in a dataset and randomizes the rows
|   | resultsToMulti.py - reads in datafiles created by 'one-and-done' scripts and adds them to the multi files created by makeMultiCsv.py
│   
└─── old_ignore
|   | an old (validated) implementation used for validation of current implementation
|   |
└─── results
|   └─── datasetsFa18 - results for multiple datasets starting in the fall of 2018
|   |   └─── multis - 'multi' files used to generate results
|   └─── ICMLR 18 - results used for the 2018 ICMLR conference paper
|   |   └─── multis - 'multi' files used to generate results
|   
└─── src
|   └─── Data - various datasets
|   | Makefile - builds the project
|   | data_handler.cc/h - handles data input and manipulation (largely unused in the new implementation)
|   | datatab.cc/h - object to manage data belonging to each node or forest
|   | forest.cc/h - random delayed/greedy forest implementation class
|   | lacart.cc/h - entry point of the program. Handles used input, tree building, and file/results creation
|   | laforest.cc/h - similar to lacart but for the random forests
|   | metric.cc/h - object used to calculate the statistical metrics when building the tree
|   | node.cc/h - the beef of the program. Represents the nodes in the decision tree including their data partitions and splitting rules. alls in this class build the trees
|   | structures.h - various structures used by the tree building and input handling processes
|   | utils.cc/h - in this implementation its primary purpose is to parse user input and read in test/train data
|   | runAndAnalyze.sh - one-and-done script to run datasets that are not partitioned into train/test sets. Will build the trees, output results, randomize the dataset, and process final results including creating graphs.
|   | runAnalyzeWithSplit.sh - does the same thing as runAndAnalyze.sh except uses pre-split train/test datasets (i.e. a csv for training and a csv for testing)
|   | runmany.sh - called by runAndAnalyze.sh and runAnalyzeWithSplit.sh to run 'lacart' for multiple depths and output results
|
└─── mpi
|   | Essentially the same contents as 'src' except uses MPI for parallel processing (changes to lacart.cc/h, node.cc/h, and the Makefile)
```
