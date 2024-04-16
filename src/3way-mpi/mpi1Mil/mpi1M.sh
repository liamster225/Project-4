#!/bin/bash

#SBATCH --mem-per-cpu=512M

#SBATCH --time=1:00:00

##Nodes tells how many machines / how many cores
#SBATCH --nodes=1 --ntasks-per-node=1

#SBATCH --constraint=moles

#SBATCH --output=mpi1Mil1Node1Thread.out

/homes/zarensman/hw4/3way-mpi/mpi1Mil/build/mpi10
