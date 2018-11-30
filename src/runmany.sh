#!/usr/bin/env bash

# Script parameters
method=anova
depths=(6 8 10 12 14 16 18)
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
		depths=(1 2 3 4 5 6)
	fi
fi
echo "Using method '$method' and depths [${depths[@]}] for dataset '$dataset'."
echo ""

# Trial variables
trials=5
dfile="results.delayed.dat"
gfile="results.dat"

# For each trial, get delayed and greedy results then re-randomize the dataset via
# Python script.
i=1							# 1 rather than 0 for nice output
while [[ $i -le $trials ]]
do 
	for d in "${depths[@]}"; do 
		cmd="./lacart $dataset $response 0 cp=0 maxdepth=$d splitdata=1 randomsplit=0 >> $gfile"
		echo "Running greedy algorithm: trial $i, depth $d. Command: $cmd"
		eval $cmd
		cmd="./lacart $dataset $response 1 cp=0 maxdepth=$d splitdata=1 randomsplit=0 >> $dfile"
		echo "Running delayed algorithm: trial $i, depth $d. Command: $cmd"
		eval $cmd
	done
	echo "Randomizing dataset..."
	rand="python3 ../data_utils/randomize.py $dataset"
	eval $rand
	(( i = i + 1 ))
done

# for a specified number of trials (5 usually)
# 	Loop through all depths
#	Run the delayed/greedy alg for each depth
# 	output the results to a file based on program output (use >> to append)
#	Randomize dataset 
# repeat

# This should create 2 csv files with the data used by the multi files so we don't need to do any data entry 
# output formatting should be handled in the actual executables. 
