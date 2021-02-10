#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <unistd.h>
#include "mandelbrot.h"
// same ordering with code from serial code

static void mandelbrot(double Cx, double Cy, unsigned char pixel[3]) {
	double Zx, Zy;
	double Zx2, Zy2; /* Zx2 = Zx*Zx;  Zy2 = Zy*Zy  */

	int Iteration;
	const int IterationMax = ITERATION_MAX; // default

	const double EscapeRadius = 400;
	double ER2 = EscapeRadius * EscapeRadius;
	double c;

	Zx = 0.0;
	Zy = 0.0;
	Zx2 = Zx * Zx;
	Zy2 = Zy * Zy;

	for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
	{
		Zy = (2 * Zx * Zy) + Cy;
		Zx = Zx2 - Zy2 + Cx;
		Zx2 = Zx * Zx;
		Zy2 = Zy * Zy;
	};

	if (Iteration == IterationMax)
	{
		// Point within the set. Mark it as black
		pixel[0] = 0;
		pixel[1] = 0;
		pixel[2] = 0;
	}
	else
	{
		// Point outside the set. Mark it as white
		// serial code did not do rounding explicitly
		c = 3 * log((double)Iteration) / log((double)(IterationMax)-1.0);
		if (c < 1)
		{
			pixel[0] = 0;
			pixel[1] = 0;
			pixel[2] = 255 * c;
		}
		else if (c < 2)
		{
			pixel[0] = 0;
			pixel[1] = 255 * (c - 1);
			pixel[2] = 255;
		}
		else
		{
			pixel[0] = 255 * (c - 2);
			pixel[1] = 255;
			pixel[2] = 255;
		}
	}
}

int main(int argc, char *argv[]) {

    double start_t, end_t;
    start_t = MPI_Wtime();
	// int imagesize;
	/* screen ( integer) coordinate */
	int iX, iY;
	const int iXmax=IX_MAX; // default
	const int iYmax=IY_MAX; // default
	/* world ( double) coordinate = parameter plane*/
	double Cx, Cy;
	const double CxMin = CX_MIN;
	const double CxMax = CX_MAX;
	const double CyMin = CY_MIN;
	const double CyMax = CY_MAX;

	double PixelWidth = (CxMax - CxMin) / iXmax;
	double PixelHeight = (CyMax - CyMin) / iYmax;

	int rank, size;
	int rootRank = 0;
    int tag = 0;

	int startFromPixel, endWithPixel;
    unsigned char *image;
    unsigned char *pixels;
    unsigned int uctypesize = sizeof(unsigned char);
    char* filename = "Mandelbrot.ppm";
    const int MaxColorComponentValue = 255;
    char *comment = "# ";
    FILE *fp;   
    double x, y;
	int totalDatapoint = iXmax * iYmax * 3;
    MPI_Datatype localwork_type;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (size < 2) {
		printf("Size must be greater than one");
        MPI_Finalize();
        return(1);
	}
    MPI_Request requests[size-1];
	int pixel_workload = (iXmax * iYmax) / (size-1);
	int pixel_remainder = (iXmax * iYmax) % (size-1);
    int flag, starti, endi, stepsize;
    int local_datapoint_num = pixel_workload * 3 * uctypesize;
	if (rank == rootRank) {
        MPI_Type_vector(pixel_workload, 3, 3*(size - 1), MPI_UNSIGNED_CHAR, &localwork_type);
        MPI_Type_commit(&localwork_type);
        MPI_Alloc_mem((MPI_Aint)(totalDatapoint * uctypesize), MPI_INFO_NULL, &image);
		for (int i = 1; i < size; i ++ ) {
			MPI_Irecv(&image[(i - 1)*3], 1, localwork_type, i, tag, MPI_COMM_WORLD, &requests[i-1]);
		}
        for (int i=pixel_remainder-1;i>=0 ; i -= 3) {
            x = CxMax - (double)(i % iXmax) * PixelWidth;
            y = CyMax - (double)(i % iYmax) * PixelHeight;
            mandelbrot(x, y, &image[i]);
        }
		MPI_Waitall(size - 1, requests, MPI_STATUSES_IGNORE);
  
	} else {
		starti = (rank - 1) * 3 * uctypesize;
        stepsize = (size - 1) * 3 * uctypesize;
        endi = starti + stepsize * pixel_workload;
		MPI_Alloc_mem((MPI_Aint)(local_datapoint_num), MPI_INFO_NULL, &pixels);
		for (int i=starti; i< endi; i+=stepsize) {
            x = ((double)((i/3) % iXmax)) * PixelWidth + CxMin;
            y = ((double)ceil((i/3) / iXmax)) * PixelHeight + CyMin;
			mandelbrot(x, y, &pixels[(i-starti) / stepsize * 3]);
		}
		MPI_Send(pixels, local_datapoint_num, MPI_UNSIGNED_CHAR, rootRank, tag, MPI_COMM_WORLD);


		free(pixels);
		MPI_Finalize();
        return(0);
    }

    fp = fopen(filename, "wb");
    fprintf(fp,"P6\n %s\n %d\n %d\n %d\n", comment, iXmax, iYmax, MaxColorComponentValue);
    fwrite(image, uctypesize, totalDatapoint, fp);

    free(image);
    fclose(fp);
    end_t = MPI_Wtime();

    printf("time taken : %f\n", end_t-start_t);
    MPI_Finalize();
	return(0);
}

