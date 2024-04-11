#!/bin/bash
#SBATCH --job-name=pThreads
#SBATCH -o 3way-pThreads-MASSBATCH-STATS.out
for i in 1 2 4 8 16
do
	echo "Tasks: $i"
	sbatch --constraint=elves --ntasks-per-node=$i --nodes=1 --job-name=pThreads -o $i-core-100k.out pThreads_sbatch.sh
done