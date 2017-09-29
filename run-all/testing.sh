#!/bin/bash
#$ -l mem=3G
#$ -q \*@@elves
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com

for x in 3 4 5 6
do
    echo "mpirun -np $NSLOTS /homes/kmdice/625/hw3/mpi \"${JOB_ID}_${x}\" $x "$PE" $NSLOTS $NHOSTS"
done
