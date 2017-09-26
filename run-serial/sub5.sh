#!/bin/bash
#$ -pe single 1
#$ -q \*@@elves
#$ -l mem=4G
#$ -l h_rt=2:00:00
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
/homes/kmdice/625/hw3/serial $JOB_ID 5
