#!/bin/bash

#SBATCH --mem-per-cpu=512M

#SBATCH --time=1:00:00

##Nodes tells how many machines / how many cores
#SBATCH --nodes=1 --ntasks-per-node=1

#SBATCH --constraint=moles

#SBATCH --output=mpi100k1Thread.out

mpirun -n 1 /homes/zarensman/hw4/3way-mpi/mpi100k/build/mpi1
