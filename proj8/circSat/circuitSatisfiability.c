/* circuitSatifiability.c solves the Circuit Satisfiability
 *  Problem using a brute-force sequential solution.
 *
 *   The particular circuit being tested is "wired" into the
 *   logic of function 'checkCircuit'. All combinations of
 *   inputs that satisfy the circuit are printed.
 *
 *   16-bit version by Michael J. Quinn, Sept 2002.
 *   Extended to 32 bits by Joel C. Adams, Sept 2013.
 *scp
 * First version with MPI
 *   edits By: Ben Steves
 *   why       Project 1 for CS374
 *   where     Calvin University
 *   date      9-9-2021
 * 
 * OpenMP and MPI hybrid integration:
 *   edits By: Ben Steves
 *   why       Project 8 for CS374
 *   where     Calvin University
 *   date      11-18-21
 */

#include <stdio.h>     // printf()
#include <stdlib.h>
#include <limits.h>    // UINT_MAX
#include <mpi.h>
#include <omp.h>

int checkCircuit (int, long);

/*processCommandLine borrowed from spmd2 (Written by prof. Adams) 
*  reads in command line and grabs numThreads
*  from user. If not specified, will automatically use 1. 
*/
int processCommandLine(int argc, char ** argv) {
   if (argc == 2) {
      return atoi( argv[1] );
   } else {
      return 1;
   }
}

int main (int argc, char *argv[]) {
   long i;                                   // loop variable (64 bits) 
   int id = -1, numProcesses = -1,           //mpi vars
       numThreads,                           //omp vars
       length = -1, threadSupportLevel = -1, //hybrid vars
       localCount = 0, globalCount = 0;      //reduction vars
    double startTime = 0.0, totalTime = 0.0; //timing vars
	char hostName[MPI_MAX_PROCESSOR_NAME];
	

	int result = MPI_Init_thread(&argc, &argv, 
                                      MPI_THREAD_FUNNELED, 
                                      &threadSupportLevel);
        if (result != MPI_SUCCESS) {
           fprintf(stderr, "\nMPI+multithreading not supported,\n");
           fprintf(stderr, " support level = %d\n\n", threadSupportLevel);
           exit(1);
        }

   //launch processes with MPI
   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
   MPI_Get_processor_name (hostName, &length);

   printf ("\nProcess %d is checking the circuit...\n", id);

   startTime = MPI_Wtime();

   numThreads = processCommandLine(argc, argv);
   omp_set_num_threads(numThreads);
	
  //divide up work - each processor spawns n threads to help its computation
  //reduction work done by threads into localCount
   #pragma omp parallel for reduction(+:localCount)
   for (i = id; i <= UINT_MAX; i+=numProcesses) {
      localCount += checkCircuit (id, i);
   }

   //printf("%i ", localCount);

   //there are N local counts for N number of processes, reduce them
   MPI_Reduce(&localCount, &globalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

   totalTime = MPI_Wtime() - startTime;

   if (id == 0) {
	   printf("Process %d finished in time %f secs.\n", id, totalTime);
	   fflush (stdout);
	   printf("\nA total of %d solutions were found.\n\n", globalCount);
   }

   MPI_Finalize();
   return 0;
}

/* EXTRACT_BIT is a macro that extracts the ith bit of number n.
 *
 * parameters: n, a number;
 *             i, the position of the bit we want to know.
 *
 * return: 1 if 'i'th bit of 'n' is 1; 0 otherwise 
 */

#define EXTRACT_BIT(n,i) ( (n & (1<<i) ) ? 1 : 0)


/* checkCircuit() checks the circuit for a given input.
 * parameters: id, the id of the process checking;
 *             bits, the (long) rep. of the input being checked.
 *
 * output: the binary rep. of bits if the circuit outputs 1
 * return: 1 if the circuit outputs 1; 0 otherwise.
 */

#define SIZE 32

int checkCircuit (int id, long bits) {
   int v[SIZE];        /* Each element is one of the 32 bits */
   int i;

   for (i = 0; i < SIZE; i++) {
     v[i] = EXTRACT_BIT(bits,i);
   }

   if ( ( (v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
       && (!v[3] || !v[4]) && (v[4] || !v[5])
       && (v[5] || !v[6]) && (v[5] || v[6])
       && (v[6] || !v[15]) && (v[7] || !v[8])
       && (!v[7] || !v[13]) && (v[8] || v[9])
       && (v[8] || !v[9]) && (!v[9] || !v[10])
       && (v[9] || v[11]) && (v[10] || v[11])
       && (v[12] || v[13]) && (v[13] || !v[14])
       && (v[14] || v[15]) )
       &&
          ( (v[16] || v[17]) && (!v[17] || !v[19]) && (v[18] || v[19])
       && (!v[19] || !v[20]) && (v[20] || !v[21])
       && (v[21] || !v[22]) && (v[21] || v[22])
       && (v[22] || !v[31]) && (v[23] || !v[24])
       && (!v[23] || !v[29]) && (v[24] || v[25])
       && (v[24] || !v[25]) && (!v[25] || !v[26])
       && (v[25] || v[27]) && (v[26] || v[27])
       && (v[28] || v[29]) && (v[29] || !v[30])
       && (v[30] || v[31]) ) )
   {
      printf ("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n", id,
         v[31],v[30],v[29],v[28],v[27],v[26],v[25],v[24],v[23],v[22],
         v[21],v[20],v[19],v[18],v[17],v[16],v[15],v[14],v[13],v[12],
         v[11],v[10],v[9],v[8],v[7],v[6],v[5],v[4],v[3],v[2],v[1],v[0]);
      fflush (stdout);
      return 1;
   } else {
      return 0;
   }
}

