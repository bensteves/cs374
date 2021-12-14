/* spmd.c
 * ... illustrates the single program multiple data
 *      (SPMD) pattern using MPI and OpenMP commands.
 *
 * Joel Adams, Calvin College, November 2009.
 *
 * Usage: mpirun -np 4 ./spmd 
 *
 * Exercise:
 *  1. Build and run the program, noting the output.
 *  2. Uncomment the OpenMP pragma; save.
 *  3. Rebuild and rerun the program,
 *      noting how the output changes.
 *  4. Note the use of MPI_Init_thread()
 *      instead of MPI_Init().
 */

#include <stdio.h>    // printf()
#include <stdlib.h>   // atoi()
#include <mpi.h>      // MPI commands
#include <omp.h>      // OpenMP commands

int main(int argc, char** argv) {
	int processID= -1, numProcesses = -1, 
	    length = -1, threadSupportLevel = -1;
	char hostName[MPI_MAX_PROCESSOR_NAME];

        int result = MPI_Init_thread(&argc, &argv, 
                                      MPI_THREAD_FUNNELED, 
                                      &threadSupportLevel);
        if (result != MPI_SUCCESS) {
           fprintf(stderr, "\nMPI+multithreading not supported,\n");
           fprintf(stderr, " support level = %d\n\n", threadSupportLevel);
           exit(1);
        }
	
	MPI_Comm_rank(MPI_COMM_WORLD, &processID);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
	MPI_Get_processor_name (hostName, &length);

       // #pragma omp parallel 
        {
          int threadID = omp_get_thread_num();
          int numThreads = omp_get_num_threads();

	  printf("Hello from thread %d of %d from process %d of %d on %s\n",
		 threadID, numThreads,
                 processID, numProcesses, hostName);
        }

	MPI_Finalize();
	return 0;
}

