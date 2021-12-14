/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 *
 * Edited: Ben Steves 10-4-21 CS 374
 *
 * Errors: Pixelates more and more with each increased NP
 *         Exit code 6 error on NP=64
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
	       numProcesses = -1;
    double     x        = 0.0,
               y        = 0.0,
               c_real   = 0.0,
               c_imag   = 0.0,
               x_center = 1.16,
               y_center = -0.10,
	       startTime = 0.0,
	       totalTime = 0.0;
		
    MPE_XGraph graph;

    MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

   
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
 	//int colorPoints[(WINDOW_WIDTH*WINDOW_HEIGHT/numProcesses)];	   
	int * colorPoints;
   	int * finalPoints;
	colorPoints = malloc((WINDOW_HEIGHT * WINDOW_WIDTH * sizeof(MPE_Color))/numProcesses);
	finalPoints = malloc(WINDOW_HEIGHT * WINDOW_WIDTH * sizeof(MPE_Color));
	//int chunkSize1 = (int)ceil(((double)WINDOW_HEIGHT) / numProcesses);
        //int chunkSize2 = chunkSize1 - 1;
	//int remainder = WINDOW_HEIGHT % numProcesses;
	
	if (numProcesses > WINDOW_HEIGHT) {
		if (id == 0) {
			printf("Please run with -np less than or equal to %d\n.", WINDOW_HEIGHT);
			MPI_Finalize();
			exit(0);
		}
	} else {
			//split computation up among slices
		for (ix = id; ix < WINDOW_WIDTH; ix+=numProcesses) {
		       for (iy = 0; iy < WINDOW_HEIGHT; iy++) {
	 		    c_real = (ix - 400) * SPACING - x_center;
	  		    c_imag = (iy - 400) * SPACING - y_center;
	  		    x = y = 0.0;
		            n = 0;

	  		    while (n < 50 && distance(x, y) < 4.0) {
	     			compute(x, y, c_real, c_imag, &x, &y);
	     			n++;
	  		    }

	  		    if (n < 50) {
				//slightly different than chunks, this helps them get to front of respective arrays
	      			colorPoints[(((ix-id)/numProcesses)*WINDOW_HEIGHT) + iy] = 0;	 
	  		    } else {
	      			colorPoints[(((ix-id)/numProcesses)*WINDOW_HEIGHT) + iy] = 1;
	  		    }
			}
    		 } 
		//gather computations from all processes
    		MPI_Gather(colorPoints, WINDOW_WIDTH*(WINDOW_HEIGHT/numProcesses), MPI_INT,
               		finalPoints, WINDOW_WIDTH*(WINDOW_HEIGHT/numProcesses), MPI_INT,
               		0, MPI_COMM_WORLD);
             //MPI_Reduce(colorPoints, finalPoints, WINDOW_WIDTH*(WINDOW_HEIGHT/numProcesses),
		//	MPI_INT, );
	}
		

	 
	

    // pause until mouse-click so the program doesn't terminate
      if (id == 0) {
		MPE_Open_graphics( &graph, MPI_COMM_WORLD, 
                         getDisplay(),
                         -1, -1,
                         WINDOW_WIDTH, WINDOW_HEIGHT, 0 );

		int i = 0;
		int cutoffPoint = WINDOW_WIDTH/numProcesses;
		for(i = 0; i < numProcesses; i++) {		 
		    for (ix = 0; ix < cutoffPoint; ix++) {
			int newX = i + (ix*numProcesses);
		        for(iy = 0; iy < WINDOW_HEIGHT; iy++) {
			    if (finalPoints[ix *(WINDOW_HEIGHT) + iy] == 0) {
			        MPE_Draw_point(graph, newX, iy, MPE_PINK);
				
			    } else  {
			        MPE_Draw_point(graph, newX, iy, MPE_BLACK);
		           }
		      }
		  }
		}		
       
		
		totalTime = MPI_Wtime() - startTime;
	printf("Process finished in time %f secs.\n", totalTime);

	//printf("Black: %i , Pink %i", blackCount, pinkCount);   //for debugging
	
	//for(int i = 0; i <912399; i++){       //returning 0
	//	printf("%i ", finalPoints[i]);
	//}

	 printf("\nClick in the window to continue...\n");
        MPE_Get_mouse_press( graph, &ix, &iy, &button );
	MPE_Close_graphics( &graph );
  		
    	
    }
    free(finalPoints);
    free(colorPoints);
    MPI_Finalize();
    return 0;
}

