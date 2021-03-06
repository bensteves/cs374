/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 *
 * Edited: Ben Steves 10-4-21 CS 374
 *
 * Errors: Larger processes missing bands
 *        
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <mpe.h>
#include "display.h"


/* compute the Mandelbrot-set function for a given
 *  point in the complex plane.
 *
 * Receive: doubles x and y,
 *          complex c.
 * Modify: doubles ans_x and ans_y.
 * POST: ans_x and ans_y contain the results of the mandelbrot-set
 *        function for x, y, and c.
 */
void compute(double x, double y, double c_real, double c_imag,
              double *ans_x, double *ans_y)
{
        *ans_x = x*x - y*y + c_real;
        *ans_y = 2*x*y + c_imag;
}

/* compute the 'distance' between x and y.
 *
 * Receive: doubles x and y.
 * Return: x^2 + y^2.
 */
double distance(double x, double y)
{
        return x*x + y*y;
}


int main(int argc, char* argv[])
{
    const int  WINDOW_HEIGHT = 900;
    const int  WINDOW_WIDTH  = 1200;
    const double SPACING     = 0.003;

    int        n        = 0,
               ix       = 0,
               iy       = 0,
               button   = 0,
               id       = 0,
	       numProcesses = -1,
		nextRow = 0,
		computeRow = 0,
		countRows;
    double     x        = 0.0,
               y        = 0.0,
               c_real   = 0.0,
               c_imag   = 0.0,
               x_center = 1.16,
               y_center = -0.10,
		startTime = 0.0,
	       totalTime = 0.0;

    MPE_XGraph graph;
	
	int * inputRow = (int*)malloc(WINDOW_WIDTH*sizeof(MPI_INT));
	int * outputRow = (int*)malloc(WINDOW_WIDTH*sizeof(MPI_INT));
    MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
	
	MPI_Status status;
/*
    // Uncomment this block for interactive use
    printf("\nEnter spacing (.005): "); fflush(stdout);
    scanf("%lf",&spacing);
    printf("\nEnter coordinates of center point (0,0): "); fflush(stdout);
    scanf("%lf %lf", &x_center, &y_center);
    printf("\nSpacing=%lf, center=(%lf,%lf)\n",
            spacing, x_center, y_center);
*/

	startTime = MPI_Wtime();
    
	//can't have np > size of display
	if (numProcesses > WINDOW_HEIGHT) {
		if (id == 0) {
			printf("Please run with -np less than or equal to %d\n.", WINDOW_HEIGHT);
			MPI_Finalize();
			exit(0);
		}
	} 
	
	//for np=1. does sequential calculation
	else if (numProcesses == 1) {

		MPE_Open_graphics( &graph, MPI_COMM_WORLD, 
                         getDisplay(),
                         -1, -1,
                         WINDOW_WIDTH, WINDOW_HEIGHT, 0 );

		for (ix = 0; ix < WINDOW_WIDTH; ix++) {
		   for (iy = 0; iy < WINDOW_HEIGHT; iy++)
		   {
			  c_real = (ix - 400) * SPACING - x_center;
			  c_imag = (iy - 400) * SPACING - y_center;
			  x = y = 0.0;
			  n = 0;

			  while (n < 50 && distance(x, y) < 4.0)
			  {
				 compute(x, y, c_real, c_imag, &x, &y);
				 n++;
			  }

			  if (n < 50) {
				 MPE_Draw_point(graph, ix, iy, MPE_PINK);
			  } else {
				 MPE_Draw_point(graph, ix, iy, MPE_BLACK);
			  }
		    }
		}
			
			totalTime = MPI_Wtime() - startTime;
			printf("Process finished in time %f secs.\n", totalTime);
			printf("\nClick in the window to continue...\n");
			MPE_Get_mouse_press( graph, &ix, &iy, &button );
			MPE_Close_graphics( &graph );
			//MPI_Finalize;
			//return 0;
		
		//NP>1
	} else {
		
		//master - prints outputs
		if (id == 0) {
			MPE_Open_graphics( &graph, MPI_COMM_WORLD, 
                         getDisplay(),
                         -1, -1,
                         WINDOW_WIDTH, WINDOW_HEIGHT, 0 );
						 
			
			//print row 0
			for (iy = 0; iy < WINDOW_HEIGHT; iy++) {
				c_real = (id - 400) * SPACING - x_center;
				c_imag = (iy - 400) * SPACING - y_center;
				x = y = 0.0;
				n = 0;
				  
				while (n < 50 && distance(x, y) < 4.0) {
					compute(x, y, c_real, c_imag, &x, &y);
					n++; 
				}
				  
				if (n < 50) {
					 MPE_Draw_point(graph, id, iy, MPE_PINK);
				} else {
					 MPE_Draw_point(graph, id, iy, MPE_BLACK);
				}    
			}
			
			//send - the next row to compute
			//receive - an input row with calculations done by any worker
			countRows = 1;//double check this
			nextRow = 1;
			while (countRows < WINDOW_WIDTH) {
				MPI_Recv(inputRow, WINDOW_WIDTH, MPI_INT, MPI_ANY_SOURCE,
								MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	
				
				if (nextRow < WINDOW_WIDTH) {
					nextRow++; //maybe change this - might cause program to break early
				} else {
					nextRow = -1;
				}
					
				MPI_Send(&nextRow, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
				
				for (int i = 0; i < WINDOW_WIDTH; i++) {
					if (inputRow[i] == 0 ) {
						MPE_Draw_point(graph, status.MPI_TAG, i, MPE_PINK);
					} else {
						MPE_Draw_point(graph, status.MPI_TAG, i, MPE_BLACK);
					}
				}
				
				countRows++;
			}	
			totalTime = MPI_Wtime() - startTime;
			printf("%i , %i\n", countRows, nextRow); //debugging, making sure increments work
			printf("Process finished in time %f secs.\n", totalTime);
			printf("\nClick in the window to continue...\n");
			MPE_Get_mouse_press( graph, &ix, &iy, &button );
			MPE_Close_graphics( &graph ); 
		}
		
		//workers
		else  {
			computeRow = id -1;
			while(nextRow != -1) {
				if (computeRow == -1) {break;}
			
				for (iy = 0; iy < WINDOW_HEIGHT; iy++) {
					c_real = (computeRow - 400) * SPACING - x_center; //may not be the id
					c_imag = (iy - 400) * SPACING - y_center;
					x = y = 0.0;
					n = 0;
					  
					while (n < 50 && distance(x, y) < 4.0) {
						compute(x, y, c_real, c_imag, &x, &y);
						n++; 
					}
					  
					if (n < 50) {
						outputRow[iy] = 0;
					} else {
						outputRow[iy] = 1;
					}
				}
				
				MPI_Send(outputRow, WINDOW_WIDTH, MPI_INT, 0, computeRow, MPI_COMM_WORLD);
				MPI_Recv(&computeRow, 1, MPI_INT, 0, 
						1, MPI_COMM_WORLD, &status); //try another tag if fails, probably 1
			} 
		}
		
	}
	free(inputRow);
	free(outputRow);
	

    MPI_Finalize();
    return 0;
}

