/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 *
 * Edited: Ben Steves 9-26-21 CS 374
 *
 * Errors: Exit code 6 error on NP=64
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
	       yStart = -1,
	       yStop = -1;
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
	MPE_Color *colors = NULL;
 	//int colorPoints[(WINDOW_WIDTH*WINDOW_HEIGHT/numProcesses)];	
	
	//arrays to store computed values
	int * colorPoints;
   	int * finalPoints;
	colorPoints = malloc((WINDOW_HEIGHT * WINDOW_WIDTH * sizeof(MPI_INT))/numProcesses);
	finalPoints = malloc(WINDOW_HEIGHT * WINDOW_WIDTH * sizeof(MPI_INT));

	//for computing chunks
	int chunkSize1 = (int)ceil(((double)WINDOW_HEIGHT) / numProcesses);
        int chunkSize2 = chunkSize1 - 1;
	int remainder = WINDOW_HEIGHT % numProcesses;
	

	if (numProcesses > WINDOW_HEIGHT) {
		if (id == 0) {
			printf("Please run with -np less than or equal to %d\n.", WINDOW_HEIGHT);
			MPI_Finalize();
			exit(0);
		}
	} else { //handle remainders
		if (remainder == 0 || (remainder != 0 && id < remainder)) {
			yStart = id * chunkSize1;
			yStop = yStart + chunkSize1;
		} else {
			yStart = (remainder * chunkSize1) + (chunkSize2 * (id - remainder));
			yStop = yStart + chunkSize2;
		} 
		int i = 0;
		for (iy = yStart; iy < yStop; iy++) {
       		       for (ix = 0; ix < WINDOW_WIDTH; ix++) {
         		    c_real = (ix - 400) * SPACING - x_center;
          		    c_imag = (iy - 400) * SPACING - y_center;
          		    x = y = 0.0;
                            n = 0;

          		    while (n < 50 && distance(x, y) < 4.0) {
             			compute(x, y, c_real, c_imag, &x, &y);
             			n++;
          		    }

			//store ints in array as indexes to retreive color to draw
          		    if (n < 10) {
	      			colorPoints[i*WINDOW_WIDTH+ix] = 0;	 
          		    } else if (n < 15) {
              			colorPoints[i*WINDOW_WIDTH+ix] = 1;
          		    } else if (n < 25) {
          		    	colorPoints[i*WINDOW_WIDTH+ix] = 2;
          		    } else if (n < 40) {
          		    	colorPoints[i*WINDOW_WIDTH+ix] = 3;
          		    } else {
          		    	colorPoints[i*WINDOW_WIDTH+ix] = 4;
          		    }
       			}
       			i++;
       			
    		 } 
    		 MPI_Gather(colorPoints, WINDOW_WIDTH*(WINDOW_HEIGHT/numProcesses), MPI_INT,
               		finalPoints, WINDOW_WIDTH*(WINDOW_HEIGHT/numProcesses), MPI_INT,
               		0, MPI_COMM_WORLD);
             
	}
		

	 
	

    // pause until mouse-click so the program doesn't terminate
      if (id == 0) {
		MPE_Open_graphics( &graph, MPI_COMM_WORLD, 
                         getDisplay(),
                         -1, -1,
                         WINDOW_WIDTH, WINDOW_HEIGHT, 0 );
		
		
		//int colorIdx = 0;
 		colors = malloc( numProcesses * sizeof(MPE_Color) );
		//lucky number 34
		MPE_Make_color_array(graph, 34, colors);

						 
		for (iy = 0; iy < WINDOW_HEIGHT; iy++) {
		    for(ix = 0; ix < WINDOW_WIDTH; ix++) {
			if (finalPoints[iy*WINDOW_WIDTH+ix] == 0) 
			  {
			    MPE_Draw_point(graph, ix, iy, colors[24]);
			  } 
			else  if (finalPoints[iy*WINDOW_WIDTH+ix] == 1) 
			  {
			    MPE_Draw_point(graph, ix, iy, colors[20]);
                          } 
			else if (finalPoints[iy*WINDOW_WIDTH+ix] == 2) 
			  {
                        	MPE_Draw_point(graph, ix, iy, colors[5]);
                          } 
			else if (finalPoints[iy*WINDOW_WIDTH+ix] == 3) 
			  {
                        	MPE_Draw_point(graph, ix, iy, colors[1]);
                          } 
			else 
			  {
                            MPE_Draw_point(graph, ix, iy, MPE_BLACK);
                          }
		    }
		}			
       
		
		totalTime = MPI_Wtime() - startTime;
	printf("Process finished in time %f secs.\n", totalTime);

	 printf("\nClick in the window to continue...\n");
        MPE_Get_mouse_press( graph, &ix, &iy, &button );
	MPE_Close_graphics( &graph );
  		
    	
    }
    free(finalPoints);
    free(colorPoints);
    free(colors);
    MPI_Finalize();
    return 0;
}

