#!/bin/bash -l
#SBATCH --time=0:01:00
#SBATCH --mem=100G
#SBATCH --constraint=elves

module load OpenMPI
module load foss/2020a --quiet

echo pThread

time /homes/.../cis520/Project4/3way-pthread/openMP-100k