#!/bin/bash
#$ -pe mpi-spread 8
#$ -q \*@@elves
#$ -l mem=3G
#$ -l h_rt=0:05:00
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
mpirun -np 8 /homes/kmdice/625/hw3/mpi $JOB_ID 4
