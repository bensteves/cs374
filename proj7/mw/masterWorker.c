/* masterWorker.c
 * ... illustrates the master-worker pattern in OpenMP
 *
 * Joel Adams, Calvin College, November 2009.
 * Usage: ./masterWorker
 * Exercise: 
 * - Compile and run as is.
 * - Uncomment #pragma directive, re-compile and re-run
 * - Compare and trace the different executions.
 */

#include <stdio.h>   // printf()
#include <omp.h>     // OpenMP

int main(int argc, char** argv) {
    int id = -1, numThreads = -1;

    printf("\n");

    #pragma omp parallel private(id, numThreads)
    {
        id = omp_get_thread_num();
        numThreads = omp_get_num_threads();

        if ( id == 0 ) {  // thread with ID 0 is master
            printf("Greetings from the master, # %d of %d threads\n",
			    id, numThreads);
        } else {          // threads with IDs > 0 are workers 
            printf("Greetings from a worker, # %d of %d threads\n",
			    id, numThreads);
        }
    }

    printf("\n");

    return 0;
}

//as is, of course as every master worker program goes, it says "greetings" because the master worker is a weird guy who
//doesn't speak like a normal person. but it only does this once. 

//after you comment it out, now you've got greetings from 12 threads with master 0 and workers everything else
