HOST=$(shell hostname)

ifeq ($(HOST), voldemort-linux)
	FLAGS := -D PERSONAL
else ifeq ($(HOST), viper)
	FLAGS := -D VIPER
else ifeq ($(HOST), cougar)
	FLAGS := -D VIPER
else
	FLAGS := -D BEOCAT
endif

all: serial mpi test_linked_list

serial: serial.c
	cc serial.c -o serial $(FLAGS)

mpi: mpi.c
	mpicc mpi.c -o mpi $(FLAGS)

test_linked_list: test_linked_list.c
	cc test_linked_list.c -O0 -o test_linked_list
