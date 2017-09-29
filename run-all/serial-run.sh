#!/bin/bash
#$ -l mem=3G
#$ -q \*@@elves
#$ -cwd
#$ -m abe
#$ -M kevin.dice1@gmail.com
for x in 6 5 4 3
do
    /homes/kmdice/625/hw3/serial "{$JOB_NAME}_{$JOB_ID}_$x" $x
done
