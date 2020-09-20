//
//  Gauss-Elim.c
//  CS4379_Project_3
//
//  Created by Dylan Buchheim on 4/20/20.
//  Copyright © 2020 Dylan Buchheim. All rights reserved.
//

/*----------------- DIRECTIVES ------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <getopt.h>
#include <math.h>

/*----------------- FUNCTION PROTOTYPES ------------------*/
void GaussElim(int, int);
int GetLocalRow(void);
void BackSubstitution(void);
int CheckResult(void);

/*----------------- GLOBAL VARIABLES ------------------*/
#define MAXDIM 2000
#define TOLERANCE 0.5
int N = 4;
int numThreads = 2;

int NormRow = 0, LocalRow = 1;

volatile float A[MAXDIM][MAXDIM];
volatile float B[MAXDIM];
volatile float X[MAXDIM];

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
    for (col = 0; col < N; col++) {
        for (row = 0; row < N; row++) {
            A[row][col] = (float)rand() / RAND_MAX;
        }
        B[col] = (float)rand() / RAND_MAX;
        X[col] = 0.0;
    }
}

//Print_Matricies Functions, used to report the input matricies that were generated.
void Print_Matricies() {
    int row, col;
    //Only report the matrix if it is not too large, i.e. < 10 x 10.
    if (N < 10) {
        printf("\nInput Matricies:");
        printf("\nA =\n\t");
        for (row = 0; row < N; row++) {
            for (col = 0; col < N; col++) {
                printf("%5.2f%s", A[row][col], (col < N-1) ? ", " : "\n\t");
            }
        }
        printf("\nB =\n\t");
        for (col = 0; col < N; col++) {
            printf("%5.2f%s", B[col], (col < N-1) ? ", " : "\n");
        }
    }
}

//Print_Result Function, used to report the result of the Gaussian Elimination.
void Print_Result() {
    int row;
    //Only report the result if it is not too large, i.e. < 10.
    if (N < 10) {
        printf("\nResult:");
        printf("\nX =\n\t");
        for (row = 0; row < N; row++) {
            printf("%5.2f%s", X[row], (row < N-1) ? ", " : "\n");
        }
    }
}

/*------- MAIN FUNCTION --------*/

int main(int argc, char **argv){
	//Local Variables
    int seed = GetRandomSeed();
    srand(seed);
    
    //Interpret the command line arguments.
    int ret = 0;
    while ((ret = getopt(argc, argv, "T:N:")) != -1) {
        switch (ret) {
        case 'N':
            N = atoi(optarg);
            break;
        case 'T':
            numThreads = atoi(optarg);
            break;
        case '?':
        default:
            printf("Unknown argument, please enter : -N <matrix_dim> -T <num_threads>\n");
            return -1;
            break;
        }
    }
    //Report the entered command line arguments.
    printf("Number of threads = %d, Matrix dimension = %d, Seed value = %d\n", numThreads, N, seed);
    
    //Checks to make sure the arguments hold proper values, otherwise terminates execution.
    if (numThreads < 1) {
        printf("Error: Invalid number of threads entered.\n");
        return -1;
    }
    if(N < 1 || N > MAXDIM){
        printf("Error : Matrix dimension is out of range (Should be 1-2000).\n");
        return -1;
    }
    
    //Initialize the matricies and print out the input matricies.
    Init_Matricies();
    Print_Matricies();
    
    //Start tracking time for performance analysis using GetTime().
    printf("\nPerforming Gaussian Elimination...\n");
    double startTime = GetTime();
	
	//Create the threads
	#pragma omp parallel private(LocalRow) shared(NormRow) num_threads(numThreads)
	{
		//Each thread gets it row assignment in a critical region so there is no overlap.
		#pragma omp critical (LocalLock)
			LocalRow = GetLocalRow();
		//For each row in the matrix.
		while(NormRow < N-1){
			//Eliminate all the local rows based on the current normalization row.
			GaussElim(LocalRow,NormRow);
			//Wait for every thread to finish its elimination.
			#pragma omp barrier
			//Update the normalization row counter.
			#pragma omp single
			{
				NormRow ++;
			}
		}
	}

	//Perform Back Substitution.
    BackSubstitution();
    
    //Finish tracking time for performance analysis.
    double finishTime = GetTime();
   
    //Report the results.
    Print_Result();
    printf("\nTime elapsed for %d threads: %f seconds.\n\n", numThreads, (finishTime - startTime));
}

void GaussElim(int localRow, int normRow){
    //Local Variables
    int row, col;
    float gaussMult;
    
    //Elimnate each row the thread is assigned.
    for (row = localRow; row < N; row+= numThreads){
        //If this row has been fully eliminated, continue.
        if(row <= normRow){
            continue;
        }
        gaussMult = A[row][normRow]/A[normRow][normRow];
        //Perform elimination on the current row.
        for (col = normRow; col < N; col++){
            A[row][col] -= A[normRow][col] * gaussMult;
        }
        //Update the B matrix by the elimination factor.
        B[row] -= B[normRow] * gaussMult;
    }
}

//GetRowGroup Function, used to determine which rows the thread is responsible for.
int GetLocalRow(){ 
    //Return the next unassigned row number.
    LocalRow ++;
    return LocalRow - 1;
}

/*------------- BACK SUBSTITUTION --------------*/
void BackSubstitution(){
    int row, col;
    for (row = N - 1; row >= 0; row--) {
        X[row] = B[row];
        for (col = N-1; col > row; col--) {
            X[row] -= A[row][col] * X[col];
        }
        X[row] /= A[row][row];
    }
}

/*-------- CHECK RESULT --------*/
int CheckResult(){
    int i,j;
    for(i = 0; i < N; i++){
        double total = 0.0;
        for(j = 0; j < N; j++){
            total += (A[i][j] * X[j]);
        }
        if(fabs(total - B[i]) > TOLERANCE){
            printf("\nComputed: %f Actual: %f\n",total,B[i]);
            printf("---- INCORRECT RESULT ----\n");
            return -1;
        }
    }
    printf("---- CORRECT RESULT ----\n");
    return 0;
}
