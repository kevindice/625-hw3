#!/bin/bash
#$ -l mem=3G
#$ -q \*@@elves
#$ -cwd
#$ -m abe
#$ -M kevin.dice1@gmail.com

for x in 6 5 4 3
do
    mpirun -np $NSLOTS /homes/kmdice/625/hw3/mpi "${JOB_ID}_${x}" $x "$PE" $NSLOTS $NHOSTS
    cat /homes/kmdice/625/hw3/output/wiki-${JOB_ID}_${x}-part-* > /homes/kmdice/625/hw3/output/${JOB_NAME}_${JOB_ID}_$x.out
    rm /homes/kmdice/625/hw3/output/wiki-${JOB_ID}_${x}-part-*
done
