#!/bin/bash
#$ -l mem=4G
#$ -l h_rt=2:00:00
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
#$ -N serial_run
/homes/kmdice/625/hw3/exp3/serial $JOB_ID 6

