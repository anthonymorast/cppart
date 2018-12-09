#!/usr/bin/env bash

# Script parameters
method=anova
depth=big
dataset=""
response=""

echo ""
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
	echo "Response variable name is required in position 2."
	exit
else 
	response=$2
fi

if [ "$#" -ge 3 ]; then
	method=$3
fi

if [ "$#" -ge 4 ]; then
    if [ $4 != 'small' ] && [ $4 != 'big' ]; then 
		echo "Depths parameter passed but is not one of 'big' or 'small', using default depths."
	fi
	if [[ $4 == 'small' ]]; then 
		$depth=$4
	fi
fi

# used for results output
filename=$(basename $dataset .csv)
ext="Multi.csv"

rm $filename$ext 
echo "Creating 'multi' file for $filename"
cmd="python3 ../data_utils/makeMultiCsv.py $filename $depth"
eval $cmd

# run the algorithms
cmd="./runmany.sh $dataset $response $method $depth"
eval $cmd

echo ""
echo "Processing results"
cmd="python3 ../data_utils/resultsToMulti.py $filename$ext results.dat results.delayed.dat"
eval $cmd
cmd="python3 ../data_utils/processMulti.py $filename$ext"
eval $cmd
echo ""

cmd="mv $filename$ext ../results/datasetsFa18/multis"
eval $cmd
cmd="rm results.d*"
eval $cmd
ext="Multi.results.csv"
cmd="mv $filename$ext ../results/datasetsFa18/"
eval $cmd
