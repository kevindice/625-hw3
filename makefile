all: serial openmp mpi

serial: serial.c
	cc serial.c -O2 -o serial

openmp: omp.c
	cc omp.c -O2 -fopenmp -o omp
	
mpi: mpi.c
	mpicc mpi.c -o mpi
