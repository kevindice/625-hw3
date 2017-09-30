all: serial mpi test_linked_list

serial: serial.c
	cc serial.c -o serial

mpi: mpi.c
	mpicc mpi.c -o mpi

test_linked_list: test_linked_list.c
	cc test_linked_list.c -O0 -o test_linked_list
