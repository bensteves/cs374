/* arraySumMPI.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * for CS 374 (HPC) at Calvin College.
 *
 * edits By: Ben Steves
 * why       Project 7 for CS374
 * where     Calvin University
 * date      11-8-21
 *
 *
 */

#include <stdio.h>      /* I/O stuff */
#include <stdlib.h>     /* calloc, etc. */
#include <mpi.h>

void readArray(char * fileName, double ** a, int * n);
double sumArray(double * a, int numValues) ;

int main(int argc, char * argv[])
{
  int  howMany,                //size of array
	   numProcesses = -1,      //np
	   id = -1,                //process id
	   sendCount,              //number to send (chunk size)
	   remainder;              //used if array size is not the same among p's
  double sum;
  double * a;
  double * recvA;
  double totalProgramTime = 0.0, startProgramTime = 0.0,    //whole program
		 totalReadTime = 0.0, startReadTime = 0.0,          //io
		 totalScatterTime = 0.0, startScatterTime = 0.0,    //scatter
		 totalReduceTime = 0.0, startReduceTime = 0.0;      //reduce
  

  if (argc != 2) {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile>\n\n");
    exit(1);
  }
  
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
	
   startProgramTime = MPI_Wtime();
   startReadTime = MPI_Wtime();
  
  if (id==0) {
	readArray(argv[1], &a, &howMany); //only do this if id=0, possibly use scatter in func
  }
  
  //we read in with id 0, need howMany on other p's as well.
  MPI_Bcast(&howMany, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  totalReadTime = MPI_Wtime() - startReadTime;
  
  //if uneven, sendCount uses int division to compute value to send as a whole number
  sendCount = howMany / numProcesses;  
  remainder = howMany % numProcesses;
  
  startScatterTime = MPI_Wtime();
  
  //if there is offset in array, use scatterv. on borg, this should only be for 10000/32p
  //but now is able to be run with uneven numProcesses
  if (remainder != 0) {
	  recvA = malloc((sizeof(double)*sendCount) + remainder); //give array extra space when needed for scatter

	  //will increment by sendCount every time. Used both in the displacement array and as part of the while condition
	  int i = 0;   
	
	  //will increment by 1 each time           
	  int count = 0;          
	  
	  //arrays for scatterv
	  int countArray[numProcesses]; 
	  int dispArray[numProcesses]; 
	  while (i + sendCount < howMany) {
		  countArray[count] = sendCount;  //for n iterations, insert the sendCount, which is an integer
		  dispArray[count] = i;     
		  i+=sendCount;           //stops when i+sendCount goes over array size limit
		  count++;                //increments by one to get next element in arrays
	  }
	  countArray[count-1] += remainder;                      //add remainder to last index
	  dispArray[count-1] = sendCount + dispArray[count-2];   //add final displacement
	  
	  MPI_Scatterv(a, countArray, dispArray, MPI_DOUBLE,
					recvA, countArray[id], MPI_DOUBLE, 
					0, MPI_COMM_WORLD);
		
	  totalScatterTime = MPI_Wtime() - startScatterTime;
  	  startReduceTime = MPI_Wtime();
 	  sum = sumArray(recvA, countArray[id]); 
  
  //otherwise, scatter using sendCount as size
  } else {
	  recvA = malloc(sizeof(double)*sendCount);
	  MPI_Scatter(a, sendCount, MPI_DOUBLE, 
			recvA, sendCount, MPI_DOUBLE, 
			0, MPI_COMM_WORLD); 
			
	  totalScatterTime = MPI_Wtime() - startScatterTime;
  	  startReduceTime = MPI_Wtime();
 	  sum = sumArray(recvA, sendCount); 
  }
  
  
  
  totalReduceTime = MPI_Wtime() - startReduceTime;

  MPI_Barrier(MPI_COMM_WORLD);

  totalProgramTime = MPI_Wtime() - startProgramTime;
  
  
  if (id == 0) {
	  printf("The sum of the values in the input file '%s' is %g\n",
		   argv[1], sum);
	  printf("Total Time: %f \n Read Time: %f \n Scatter Time: %f \n Reduce Time %f \n",
	  	totalProgramTime, totalReadTime, totalScatterTime, totalReduceTime);

	free(a);
  }
  
  MPI_Finalize();
  free(recvA);

  return 0;
}

/* readArray fills an array with values from a file.
 * Receive: fileName, a char*,
 *          a, the address of a pointer to an array,
 *          n, the address of an int.
 * PRE: fileName contains N, followed by N double values.
 * POST: a points to a dynamically allocated array
 *        containing the N values from fileName
 *        and n == N.
 */

void readArray(char * fileName, double ** a, int * n) {
  int count, howMany;
  double * tempA;
  FILE * fin;

  fin = fopen(fileName, "r");
  if (fin == NULL) {
    fprintf(stderr, "\n*** Unable to open input file '%s'\n\n",
                     fileName);
    exit(1);
  }

  fscanf(fin, "%d", &howMany);
  tempA = calloc(howMany, sizeof(double));
  if (tempA == NULL) {
    fprintf(stderr, "\n*** Unable to allocate %d-length array",
                     howMany);
    exit(1);
  }

  for (count = 0; count < howMany; count++)
   fscanf(fin, "%lf", &tempA[count]);

  fclose(fin);

  *n = howMany;
  *a = tempA;
}

/* sumArray sums the values in an array of doubles.
 * Receive: a, a pointer to the head of an array;
 *          numValues, the number of values in the array.
 * Return: the sum of the values in the array.
 */

 

double sumArray(double * a, int numValues) {
  int i;
  double localSum = 0.0, finalSum;

  for (i = 0; i < numValues; ++i) { 
    localSum += a[i]; 
  }
  MPI_Reduce(&localSum, &finalSum, 1,MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  return finalSum;
}

