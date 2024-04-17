#!/bin/bash

#SBATCH --mem-per-cpu=512M

#SBATCH --time=1:00:00

##Nodes tells how many machines / how many cores
#SBATCH --nodes=1 --ntasks-per-node=1

#SBATCH --constraint=moles

#SBATCH --output=mpi1Mil1Node1Thread.out

/homes/ajbingh2/Project-4/src/3way-openMP/openMP1Mil/build/openMP3