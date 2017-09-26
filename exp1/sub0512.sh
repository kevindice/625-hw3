#!/bin/bash
#$ -l mem=4G
#$ -l h_rt=0:20:00
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
/homes/kmdice/625/hw3/exp1/serial-512 $JOB_ID 4
