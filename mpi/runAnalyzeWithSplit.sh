#!/usr/bin/env bash

# Script parameters
method=anova
depth=big
depths=(6 8 10 12 14 16 18)
dataset=""
response=""
test=""

echo ""
if [ "$#" -ge 1 ] || [ "$#" -eq 0 ]; then 
	if [ "$#" -eq 0 ] || [ $1 == 'help' ]; then 
		echo "Usage: ./runAnalyzeWithSplit.sh <train dataset path> <test dataset path> <response> <method {anova, gini}> <depth {big, small}>"
		echo ""
		exit
	fi
fi

# Determine parameters based on script arguments.
# Parameters are positional so we can't pass the depths without the 
# method. Valid methods are 'gini' and 'anova' but will default to anova.
# Passing 'small' as the second argument uses smaller depths than default
# which is used for small datasets.
if [[ "$#" -lt 1 ]]; then 
	echo "Dataset path is required in position 1."
	exit
else 
	dataset=$1
fi

if [[ "$#" -lt 2 ]]; then 
	echo "Testing dataset path is required in position 2."
	exit
else 
	test=$2
fi

if [[ "$#" -lt 3 ]]; then 
	echo "Response variable name is required in position 3."
	exit
else 
	response=$3
fi

if [ "$#" -ge 4 ]; then
	method=$4
fi

if [ "$#" -ge 5 ]; then
    if [ $5 != 'small' ] && [ $5 != 'big' ]; then 
		echo "Depths parameter passed but is not one of 'big' or 'small', using default depths."
	fi
	if [[ $5 == 'small' ]]; then 
		depth=$5
		depths=(1 2 3 4 5 6)
	fi
fi

# used for results output
filename=$(basename $dataset .csv)
ext="Multi.csv"

rm $filename$ext 
echo "Creating 'multi' file for $filename"
cmd="python3 ../data_utils/makeMultiCsv.py $filename $depth"
eval $cmd

dfile="results.delayed.dat"
gfile="results.dat"
rm -f $dfile $gfile
echo "delayed,depth,impurity,relerror,accuracy" > $dfile
echo "delayed,depth,impurity,relerror,accuracy" > $gfile

# run the algorithms
for d in "${depths[@]}"; do 
	cmd="mpiexec -np 6 ./lacart $dataset $response 0 cp=0 maxdepth=$d testdata=$test method=$method >> $gfile"
	echo "Running greedy algorithm: trial $i, depth $d. Command: $cmd"
	eval $cmd
	cmd="mpiexec -np 6 ./lacart $dataset $response 1 cp=0 maxdepth=$d testdata=$test method=$method >> $dfile"
	echo "Running delayed algorithm: trial $i, depth $d. Command: $cmd"
	eval $cmd
done

echo ""
echo "Processing results"
cmd="python3 ../data_utils/resultsToMulti.py $filename$ext results.dat results.delayed.dat"
eval $cmd
cmd="python3 ../data_utils/processMulti.py $filename$ext"
eval $cmd
eet="Multi.results.csv"
cmd="python3 ../data_utils/createGraphs.py $filename$eet $filename"
eval $cmd
echo ""

echo "Cleaning up..."
echo ""

cmd="mv $filename$ext ../results/datasetsFa18/multis"
eval $cmd
ext="Multi.results.csv"
cmd="mv $filename$ext ../results/datasetsFa18/"
eval $cmd
cmd="mv $filename*.eps ../results/datasetsFa18/"
eval $cmd
