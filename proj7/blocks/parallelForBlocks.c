/* parallelForBlocks.c
 * ... illustrates the use of OpenMP's default parallel for loop
 *  	in which threads iterate through blocks of the index range
 *	(cache-beneficial when accessing adjacent memory locations).
 *
 * Joel Adams, Calvin College, November 2009.
 * Usage: ./parallelForBlocks [numThreads]
 * Exercise
 * - Compile, run, trace execution
 */

#include <stdio.h>    // printf()
#include <stdlib.h>   // atoi()
#include <omp.h>      // OpenMP

int main(int argc, char** argv) {
    const int REPS = 8; //8 reps
    int id, i;

    printf("\n");
    if (argc > 1) {
        omp_set_num_threads( atoi(argv[1]) );
    }

    #pragma omp parallel for private(id, i) 
    for (i = 0; i < REPS; i++) {
        id = omp_get_thread_num();
        printf("Thread %d performed iteration %d\n",  //8 times do something
                 id, i);
    }

    printf("\n");
    return 0;
}

/*  ./parallelForBlocks uses 8 threads and they all do the iteration equal to itself
 *   thread 0 did iteration 0
 *   thread 3 did iteration 3
 *   and so on
 *
 *  ./parallelForBlocks 1 uses just thread 0
 *  ./parallelForBlocks 2 uses threads 0 and 1
 * prints all iterations from one certain thread first
 */
