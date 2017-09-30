all: serial mpi unit_test

serial: serial.c
	cc serial.c -o serial

mpi: mpi.c
	mpicc mpi.c -o mpi

unit_test: unit_test.c
	cc unit_test.c -O0 -o unit_test
