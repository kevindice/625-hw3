#!/bin/bash
#$ -l mem=3G
#$ -l h_rt=0:15:00
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
/homes/kmdice/625/hw3/exp1/serial-128 $JOB_ID 4
