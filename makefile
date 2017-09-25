all: serial openmp mpi

serial: find_keys.c
	cc find_keys.c -o find_keys

openmp: find_keys_omp.c
	cc find_keys_omp.c -o find_keys_omp
	
mpi: find_keys_mpi.c
	mpicc find_keys_mpi.c -o find_keys_mpi
