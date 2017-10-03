HOST=$(shell hostname)

ifeq ($(HOST), voldemort-linux)
	FLAGS := -D PERSONAL
else ifeq ($(HOST), viper)
	FLAGS := -D VIPER
else ifeq ($(HOST), cougar)
	FLAGS := -D VIPER
else ifeq ($(HOST), beretta)
	FLAGS := -D VIPER
else
	FLAGS := -D BEOCAT
endif

all: serial mpi test_linked_list

serial: serial.c unrolled_int_linked_list.c
	cc serial.c -o serial $(FLAGS)

mpi: mpi.c unrolled_int_linked_list.c
	mpicc mpi.c -o mpi $(FLAGS) -O2

test_linked_list: test_linked_list.c unrolled_int_linked_list.c
	cc test_linked_list.c -O0 -o test_linked_list
