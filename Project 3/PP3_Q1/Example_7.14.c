//
//  Example_7.14.c
//  CS4379_Project_3
//
//  Created by Dylan Buchheim on 4/20/20.
//  Copyright © 2020 Dylan Buchheim. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

/*----------------- GLOBAL VARIABLES ------------------*/
#define MAXDIM 2000
int dim = 200;

volatile float A[MAXDIM][MAXDIM];
volatile float B[MAXDIM][MAXDIM];
volatile float C[MAXDIM][MAXDIM];

/*----------------- HELPER FUNCTIONS ------------------*/

//GetTime Function, used to track runtimes of the algorithm.
static double GetTime(){
    struct timeval time;
    struct timezone timezone;
    //initialize the timevale object.
    gettimeofday(&time, &timezone);
    //return the current elapsed time.
    return ((double)time.tv_sec + (double)time.tv_usec * 1.e-6);
}

//GetRandomSeed Function, will return the number of microseconds of elapsed time to be used as a seed for srand().
unsigned int GetRandomSeed() {
    struct timeval time;
    struct timezone timezone;
    //initialize the timeval object.
    gettimeofday(&time, &timezone);
    //return elapsed microseconds - full seconds elapsed.
    return (unsigned int)(time.tv_usec);
}

//Init_Matricies Function, used to fill the matricies with random values.
void Init_Matricies() {
    int row, col;
    //Put random values into all the entries of A and B, and fill X with 0s.
    for (col = 0; col < dim; col++) {
        for (row = 0; row < dim; row++) {
            A[row][col] = (float)rand() / RAND_MAX;
            B[row][col] = (float)rand() / RAND_MAX;
            C[row][col] = 0.0;
        }
    }
}

//Print_Matricies Functions, used to report the input matricies that were generated.
void Print_Matricies() {
    int row, col;
    //Only report the matrix if it is not too large, i.e. < 10 x 10.
    if (dim < 10) {
        printf("\nInput Matricies:");
        printf("\nA =\n\t");
        for (row = 0; row < dim; row++) {
            for (col = 0; col < dim; col++) {
                printf("%5.2f%s", A[row][col], (col < dim-1) ? ", " : "\n\t");
            }
        }
        printf("\nB =\n\t");
        for (row = 0; row < dim; row++) {
            for (col = 0; col < dim; col++) {
                printf("%5.2f%s", B[row][col], (col < dim-1) ? ", " : "\n\t");
            }
        }
    }
}

//Print_Result Function, used to report the result of the Gaussian Elimination.
void Print_Result() {
    int row, col;
    //Only report the result if it is not too large, i.e. < 10.
    if (dim < 10) {
        printf("\nResult:");
        printf("\nC =\n\t");
        for (row = 0; row < dim; row++) {
            for (col = 0; col < dim; col++) {
                printf("%5.2f%s", C[row][col], (col < dim-1) ? ", " : "\n\t");
            }
        }
    }
}

//MatrixMultSingle Function, performs concurrent matrix multiplication, with only one loop parallelized.
double MatrixMultSingle(){
	int i,j,k;
	double start, end;
	start = GetTime();
	 
	for(i = 0; i < dim; i++){
		for(j = 0; j < dim; j++){
			C[i][j] = 0; 
			#pragma omp parallel for shared (A, B, C, dim) schedule(static)
			for(k = 0; k < dim; k++){
				C[i][j] += A[i][k] * B[k][j]; 
			}
		}
	}
	
	end = GetTime();
	return (end - start);
}
//MatrixMultDouble Function, performs concurrent matrix multiplication, with only two loops parallelized.
double MatrixMultDouble(){
	int i,j,k;
	double start, end;
	start = GetTime();
	
	for(i = 0; i < dim; i++){
		#pragma omp parallel for shared (A, B, C, dim) schedule(static)
		for(j = 0; j < dim; j++){
			C[i][j] = 0; 
			#pragma omp parallel for shared (A, B, C, dim) schedule(static)
			for(k = 0; k < dim; k++){
				C[i][j] += A[i][k] * B[k][j]; 
			}
		}
	}

	end = GetTime();
	return (end - start);
}
//MatrixMultTriple Function, performs concurrent matrix multiplication, with all loops parallelized.
double MatrixMultTriple(){
	int i,j,k;
	double start, end;
	start = GetTime();

	#pragma omp parallel for shared (A, B, C, dim) schedule(static)
	for(i = 0; i < dim; i++){
		#pragma omp parallel for shared (A, B, C, dim) schedule(static)
		for(j = 0; j < dim; j++){
			C[i][j] = 0; 
			#pragma omp parallel for shared (A, B, C, dim) schedule(static)
			for(k = 0; k < dim; k++){
				C[i][j] += A[i][k] * B[k][j]; 
			}
		}
	}

	end = GetTime();
	return (end - start);
}

/*----------------- MAIN ------------------*/
int main(int argc, char **argv)
{
	int seed = GetRandomSeed();
    srand(seed);
    double singleTime, doubleTime, tripleTime;

	Init_Matricies();
	//Print_Matricies();
	/*----- Perform Matrix Multiplication Here -----*/
	singleTime = MatrixMultSingle();
	printf("\nTime elapsed for case i: %f\n", singleTime);
	doubleTime = MatrixMultDouble();
	printf("\nTime elapsed for case ii: %f\n", doubleTime);
	tripleTime = MatrixMultTriple();
	printf("\nTime elapsed for case iii: %f\n", tripleTime);
	printf("--------------------\n");
	
	//Print_Result();
}
