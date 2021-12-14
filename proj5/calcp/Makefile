TARGET = calcPI
CC = mpicc
CFLAGS = -Wall -c -ansi -pedantic -std=c99 -g -I/opt/intel/advisor/include
LFLAGS = -o $(TARGET) -lm -g -ldl

#comment out module load if on borg

$(TARGET): $(TARGET).o integral.o
	#module load openmpi-2.0/gcc; \
	$(CC) $(TARGET).o integral.o $(LFLAGS)

$(TARGET).o: $(TARGET).c integral.h
	#module load openmpi-2.0/gcc; \
	$(CC) $(CFLAGS) $(TARGET).c

integral.o: integral.c integral.h
	#module load openmpi-2.0/gcc; \
	$(CC) $(CFLAGS) integral.c

clean:
	rm -f $(TARGET) *.o *~ *#

