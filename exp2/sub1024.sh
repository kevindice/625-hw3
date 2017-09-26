#!/bin/bash
#$ -l mem=5G
#$ -l h_rt=0:30:00
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
/homes/kmdice/625/hw3/exp1/serial-1024 $JOB_ID 5
