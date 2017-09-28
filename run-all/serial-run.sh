#!/bin/bash
#$ -l mem=3G
#$ -q \*@@elves
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
for x in 6 5 4 3
do
    ./serial "{$JOB_NAME}_{$JOB_ID}_$x" $x
done
