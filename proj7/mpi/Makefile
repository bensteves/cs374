OBJ = arraySumMPI
CC = mpicc
CFLAGS = -Wall -ansi -pedantic -std=c99
LFLAGS = -o $(OBJ)

$(OBJ): $(OBJ).c
	#module load openmpi-2.0/gcc; \
	$(CC) $(CFLAGS) $(OBJ).c $(LFLAGS)

clean:
	rm -f a.out $(OBJ)
