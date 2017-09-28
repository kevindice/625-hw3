#!/bin/bash
#$ -pe mpi-fill 16
#$ -l mem=3G
#$ -l h_rt=0:30:00
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
mpirun -np 16 /homes/kmdice/625/hw3/mpi $JOB_ID 6
