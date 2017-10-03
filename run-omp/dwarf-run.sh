#!/bin/bash
#$ -l mem=512M
#$ -q \*@@dwarves
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com

/homes/kmdice/625/hw3/omp "${JOB_ID}" 6 $NSLOTS
