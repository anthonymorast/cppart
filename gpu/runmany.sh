#!/usr/bin/env bash

# Script parameters
method=anova
depths=(6 8 10 12 14 16 18)
dataset=""
response=""

echo ""
if [ "$#" -ge 1 ] || [ "$#" -eq 0 ]; then 
	if [ "$#" -eq 0 ] || [ $1 == 'help' ]; then 
		echo "Usage: ./runmany.sh <train dataset path> <response> <method {anova, gini}> <depth {big, small}>"
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
rm -f $dfile $gfile
echo "delayed,depth,impurity,relerror,accuracy" > $dfile
echo "delayed,depth,impurity,relerror,accuracy" > $gfile
while [[ $i -le $trials ]]
do 
	for d in "${depths[@]}"; do 
		cmd="mpiexec -np 4 ./lacart $dataset $response 2 cp=0 maxdepth=$d splitdata=1 randomsplit=0 method=$method >> $gfile"
		echo "Running greedy algorithm: trial $i, depth $d. Command: $cmd"
		eval $cmd
		cmd="mpiexec -np 4 ./lacart $dataset $response 3 cp=0 maxdepth=$d splitdata=1 randomsplit=0 method=$method >> $dfile"
		echo "Running delayed algorithm: trial $i, depth $d. Command: $cmd"
		eval $cmd
	done
	if [ $i -lt $trials ]; then
		echo ""
		echo "Randomizing dataset..."
		rand="python3 ../data_utils/randomize.py $dataset"
		eval $rand
		echo ""
	fi
	(( i = i + 1 ))
done
