qsub -N 'omp-dwarf' -pe single 32 -l h_rt=0:20:00 dwarf-run.sh
