#!/bin/bash
#$ -l mem=3G
#$ -q \*@@elves
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
mpirun -np $NSLOTS /homes/kmdice/625/hw3/mpi "{$JOB_ID}_6" 6 "$PE" $NSLOTS $NHOSTS
mpirun -np $NSLOTS /homes/kmdice/625/hw3/mpi "{$JOB_ID}_5" 5 "$PE" $NSLOTS $NHOSTS
mpirun -np $NSLOTS /homes/kmdice/625/hw3/mpi "{$JOB_ID}_4" 4 "$PE" $NSLOTS $NHOSTS
mpirun -np $NSLOTS /homes/kmdice/625/hw3/mpi "{$JOB_ID}_3" 3 "$PE" $NSLOTS $NHOSTS

for x in 3 4 5 6
do
    cat /homes/kmdice/625/hw3/output/wiki-{$JOB_ID}_$x-part-*.out > /homes/kmdice/625/hw3/output/{$JOB_NAME}_{$JOB_ID}_$x.out
    rm /homes/kmdice/625/hw3/output/wiki-{$JOB_ID}_$x-part-*.out
done
