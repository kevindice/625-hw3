all: serial openmp mpi

serial: find_keys_serial.c
	cc find_keys_serial.c -o find_keys_serial

openmp: find_keys_omp.c
	cc find_keys_omp.c -fopenmp -o find_keys_omp
	
mpi: find_keys_mpi.c
	mpicc find_keys_mpi.c -o find_keys_mpi
