/* puzzle3.c explores MPI+OpenMP hybridization.
 *
 * Joel Adams, Calvin College, Fall 2013.
 *
 * Usage: mpirun -np 3 ./puzzle3 
 *
 * Exercise:
 *  - Predict what the code below will print
 *  - Predict what it will print when the
 *     #pragma omp master is uncommented
 */

#include "mpi.h"      // MPI calls
#include <omp.h>      // OpenMP pragmas
#include <stdio.h>    // printf()
#include <stdlib.h>

int main (int argc, char *argv[]) {
   const int NUM_THREADS = 4; //4 threads again
   int numProcesses = 0, processID = 0, value = 0;
   int threadSupportLevel = 0;

   int result = MPI_Init_thread(&argc, &argv,
                                 MPI_THREAD_FUNNELED,
                                 &threadSupportLevel);
   if (result != MPI_SUCCESS) {
       fprintf(stderr, "\nMPI+multithreading not supported,\n");
       fprintf(stderr, " support level = %d\n\n", threadSupportLevel);
       exit(1);
   }

   MPI_Comm_size(MPI_COMM_WORLD,&numProcesses);
   MPI_Comm_rank(MPI_COMM_WORLD,&processID);

   omp_set_num_threads( NUM_THREADS );

	//master commented out: p0 prints 4, p1-2 print 8
	//OTHERWISE, 1, 2, 2
   #pragma omp parallel reduction(+:value) 
   {
      #pragma omp master
      {
         if ( processID == 0 ) {
            value = 1;
         } else {
            value = 2;
         }
      }
   }

   // predict what each process will output 
   printf("process %d: %d\n", processID, value); 

   MPI_Finalize();
   return 0;
}

