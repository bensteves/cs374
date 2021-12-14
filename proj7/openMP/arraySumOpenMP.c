/* arraySumOpenMP.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * for CS 374 (HPC) at Calvin College.
 *
 * edits By: Ben Steves
 * why       Project 7 for CS374
 * where     Calvin University
 * date      11-11-21
 *
 *
 */

#include <stdio.h>      /* I/O stuff */
#include <stdlib.h>     /* calloc, etc. */
#include <omp.h>

void readArray(char * fileName, double ** a, int * n);
double sumArray(double * a, int numValues, int numThreads) ;

int main(int argc, char * argv[])
{
  int  howMany,                //size of array     
	   numThreads;         
  double sum;
  double * a;
  double totalProgramTime = 0.0, startProgramTime = 0.0,    //whole program
		 totalReadTime = 0.0, startReadTime = 0.0,          //io
		 totalReduceTime = 0.0, startReduceTime = 0.0;      //reduce
  

  if (argc != 3) {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile> <numThreads>\n\n");
    exit(1);
  }
  
  numThreads = atoi(argv[2]);
	
  startProgramTime = omp_get_wtime();

  startReadTime = omp_get_wtime();
  
  readArray(argv[1], &a, &howMany); //only do this if id=0, possibly use scatter in func
  totalReadTime = omp_get_wtime() - startReadTime;
  
  startReduceTime = omp_get_wtime();
  sum = sumArray(a, howMany, numThreads); 
  totalReduceTime = omp_get_wtime() - startReduceTime;
  
  totalProgramTime = omp_get_wtime() - startProgramTime;
  
  printf("The sum of the values in the input file '%s' is %g\n",
           argv[1], sum);
  printf("Total Time: %f \n Read Time: %f \n Reduce Time %f \n",
  	totalProgramTime, totalReadTime, totalReduceTime);

  
  free(a);

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

/* sumArray sums the values in an array of doubles, using omp's parallel reduction.
 * Receive: a, a pointer to the head of an array;
 *          numValues, the number of values in the array.
	    numThreads, the number of threads to run
 * Return: the sum of the values in the array.
 */

 

double sumArray(double * a, int numValues, int numThreads) {
  int i;
  double result = 0.0;

   omp_set_num_threads(numThreads);

  #pragma omp parallel for reduction(+ : result)
  for (i = 0; i < numValues; ++i) { 
    result += a[i]; 
  }

  return result;
}

