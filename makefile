all: serial openmp mpi

serial: serial.c
	cc serial.c -o serial

openmp: omp.c
	cc omp.c -fopenmp -o omp
	
mpi: mpi.c
	mpicc mpi.c -o mpi
