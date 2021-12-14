/* spmd2.c
 * ... illustrates the single program multiple data
 *      (SPMD) pattern using MPI and OpenMP commands.
 *
 * Joel Adams, Calvin College, November 2009.
 *
 * Usage: mpirun -np N ./spmd2 [numThreads]
 */

#include <stdio.h>    // printf()
#include <stdlib.h>   // atoi()
#include <mpi.h>      // MPI commands
#include <omp.h>      // OpenMP commands

int processCommandLine(int argc, char ** argv) {
   if (argc == 2) {
      return atoi( argv[1] );
   } else {
      return 1;
   }
}

int main(int argc, char** argv) {
	int processID= -1, numProcesses = -1, 
	    length = -1, threadSupportLevel = -1;
        int numThreads = -1;
	char hostName[MPI_MAX_PROCESSOR_NAME];

        int result = MPI_Init_thread(&argc, &argv,
                                      MPI_THREAD_FUNNELED, //funneled
                                      &threadSupportLevel);
        if (result != MPI_SUCCESS) {
           fprintf(stderr, "\nMPI+multithreading not supported,\n");
           fprintf(stderr, " support level = %d\n\n", threadSupportLevel);
           exit(1);
        }

	MPI_Comm_rank(MPI_COMM_WORLD, &processID);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
	MPI_Get_processor_name (hostName, &length);

        numThreads = processCommandLine(argc, argv);
        #pragma omp parallel num_threads(numThreads)
        {
          int threadID = omp_get_thread_num();

	  printf("Hello from thread %d of %d from process %d of %d on %s\n",
		 threadID, numThreads,
                 processID, numProcesses, hostName);
        }

	MPI_Finalize();
	return 0;
}

//think of it that each process is threaded. If you specify 4 processes, it will print 4 times for 4 processes, times
//the number of threads. so if 4 p's and 2 threads, each process spawns two threads. 

//p1_____ t1
//|_______t2



