#!/bin/bash
#$ -l mem=3G
#$ -l h_rt=0:05:00
#$ -cwd
#$ -j y
#$ -m abe
#$ -M kevin.dice1@gmail.com
/homes/kmdice/625/hw3/serial $JOB_ID
