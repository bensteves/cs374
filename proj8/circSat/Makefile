PROG = circuitSatisfiability
CC = mpicc
CFLAGS = -Wall -ansi -pedantic -std=c99
LFLAGS = -o $(PROG) -fopenmp

$(PROG): $(PROG).c
	module load openmpi-2.0/gcc; \
	$(CC) $(CFLAGS) $(PROG).c $(LFLAGS)

clean:
	rm -f $(PROG) a.out *~ *# *.o

