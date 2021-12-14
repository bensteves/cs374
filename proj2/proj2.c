/* proj2.c
 * Message passing and master worker patterns
 * By: Ben Steves CS374 9-11-21
 */

#include <stdio.h>
#include <mpi.h>
#include <math.h>   // sqrt()
#include <stdlib.h>  // malloc()
#include <string.h>  // strlen()

//args for reference in send/receive
//(address, numValues, type, rank, tag, communicator, status <for recv>)

int main(int argc, char** argv) {
	int id = -1, numProcesses = -1;
	const int SIZE = (1024+MPI_MAX_PROCESSOR_NAME) * sizeof(char);
	char * sendString = NULL;
	char * receivedString = NULL;
	MPI_Status status;
	double startTime = 0.0, totalTime = 0.0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

	sendString = (char*) malloc( SIZE );
        receivedString = (char*) malloc( SIZE );

	startTime = MPI_Wtime();
	
	if (id == 0) {
		sprintf(sendString, "\n%d", id);
		MPI_Send(sendString, strlen(sendString)+1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(receivedString, SIZE, MPI_CHAR, numProcesses-1, 0, MPI_COMM_WORLD, 
				&status);
		totalTime = MPI_Wtime() - startTime;
		printf("%s\nTime: %f secs\n", receivedString, totalTime);
		
	} else {
		MPI_Recv(receivedString, SIZE, MPI_CHAR, id-1, 0, 
                       MPI_COMM_WORLD, &status);
		sprintf(sendString, "%s %d", receivedString, id);
                MPI_Send(sendString, strlen(sendString)+1, MPI_CHAR, (id+1)%numProcesses, 
			0, MPI_COMM_WORLD);
	}

	free(sendString); free(receivedString);
	MPI_Finalize();
	return 0;
}
