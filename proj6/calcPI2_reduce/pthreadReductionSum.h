/* pthreadReductionSum.h performs reduction pattern
 * to speed up mutex and to calculate pi
 *
 * Contains pthreadReductionSum()
 *
 * author    Ben Steves
 * why       Project 6 for CS374
 * where     Calvin University
 * date      10-28-21
 */
 
#include "pthreadBarrier.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

//array to store sums for all threads
long double * localSumArray;

/* pthreadReductionSum uses reduction pattern to sum to some number
* (in this instance pi)
*
* written by - Ben Steves
* other sources - linked below
* https://sodocumentation.net/cuda/topic/6566/parallel-reduction--e-g--how-to-sum-an-array-
*
* parameter: localVal (type long double) - a value computed by one thread
* parameter: finalVal (type long double) - the final value to come out of reduction
* parameter: threadID (type unsigned long) - what thread is doing the work
* parameter: numThreads (type unsigned long) - number of threads
*
* postcondition: pi is calculated with the reduction pattern
*/
void pthreadReductionSum(long double localVal, volatile long double * finalVal, 
			unsigned long threadID, unsigned long numThreads) {	
	
	//initialize what our stride will be										
	int stride;

	//stride is always 2^x, so our divisions in stride can always be by two
	    int x = 0;
	    while (pow(2, x) < numThreads) {
	    	x++;
	    }
	    stride = pow(2, x) / 2;
	//}


	/* if the number of threads != stride, thread 0 inserts 0's dynamically
	* and treat extra 0's as values during reduction
	* (for numThreads where dividing by 2 each stride fails.. ex 6 or 10)
	*/
	if (threadID == 0) {
	    localSumArray = malloc((numThreads*2)*sizeof(long double)); 
	    for (int i = numThreads; i < (stride*2); i++) {
    		localSumArray[i] = 0.0;
       	    }
	}
	
	//put computed values into the array
	pthreadBarrier(numThreads);
       	localSumArray[threadID] = localVal;
	
	//reduction pattern, which works for any number of processes
	for (int i = stride; i > 0; i /= 2) {
		
		pthreadBarrier(numThreads); 
		if (threadID < i) {
			localSumArray[threadID] += localSumArray[threadID + i];
		}
		//pthreadBarrier(numThreads); 
		
	}
	
	//clean out our barrier and assign reduced value to the finalVal
	barrierCleanup();
	if (threadID == 0) {
		*finalVal = localSumArray[0];
	}
}
