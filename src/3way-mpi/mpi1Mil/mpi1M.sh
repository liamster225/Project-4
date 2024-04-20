#!/bin/bash

#SBATCH --mem-per-cpu=1G

#SBATCH --time=1:00:00

##Nodes tells how many machines / how many cores
#SBATCH --nodes=1 --ntasks-per-node=20

#SBATCH --constraint=moles

#SBATCH --output=mpi1Mil20Thread.out

mpirun -n 20 /homes/zarensman/hw4/3way-mpi/mpi1Mil/build/mpi10
