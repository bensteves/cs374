/* gather.c
 * ... illustrates the use of MPI_Gather()...
 * Joel Adams, Calvin College, November 2009.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 3 

int main(int argc, char** argv) {
   int computeArray[MAX]; //array for computing
   int* gatherArray = NULL; //array for gathering, pointer
   int i, numProcs, myRank, totalGatheredVals; //new var which counts gathered vals

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

   for (i = 0; i < MAX; i++) {
      computeArray[i] = myRank * 10 + i; //computes 0*10+0, 0*10+1, 0*10+2, and so on for numprocs
   }
     
   totalGatheredVals = MAX * numProcs; //3*numprocs
   gatherArray = (int*) malloc( totalGatheredVals * sizeof(int) ); //allocate mem

   MPI_Gather(computeArray, MAX, MPI_INT, //gather array has vals now. multiple computeArrays for dif numProcs. I think. 
               gatherArray, MAX, MPI_INT, 0, MPI_COMM_WORLD);

   if (myRank == 0) { //rank 0 collects vals
      for (i = 0; i < totalGatheredVals; i++) {
         printf(" %d", gatherArray[i]);
      }
      printf("\n");
   }

   free(gatherArray);
   MPI_Finalize();

   return 0;
}

