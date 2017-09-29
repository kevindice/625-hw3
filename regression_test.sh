git pull
mpicc mpi.c -o mpi
mpirun -np 2 mpi pizza 4 pizza 1 1 | md5sum
cat output/wiki-pizza-part* | md5sum
