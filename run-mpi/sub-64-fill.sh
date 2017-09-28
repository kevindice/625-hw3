#!/bin/bash
#$ -pe mpi-fill 64
#$ -l mem=3G
#$ -l h_rt=0:10:00
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
mpirun -np 64 /homes/kmdice/625/hw3/mpi $JOB_ID 6
