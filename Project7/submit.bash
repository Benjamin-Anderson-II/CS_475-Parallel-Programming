#!/bin/bash
#SBATCH -J AutoCorr
#SBATCH -A cs475-575
#SBATCH -p classmpitest
#SBATCH -N 8    # Number of nodes
#SBATCH -n 8    $ Number of tasks
#SBATCH -o mpiproject.out
#SBATCH -e mpiproject.err
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=anderbe2@oregonstate.edu

module load openmpi
mpic++ proj07.cpp -o mpiproject -lm
mpiexec -mca btl self,tcp -np 4 ./mpiproject
