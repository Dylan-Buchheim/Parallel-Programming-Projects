//
//  Gauss-Elim.c
//  CS4379_Project_2
//
//  Created by Dylan Buchheim on 4/2/20.
//  Copyright © 2020 Dylan Buchheim. All rights reserved.
//

/*----------------- DIRECTIVES ------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <getopt.h>
#include <math.h>

/*----------------- FUNCTION PROTOTYPES ------------------*/
void *GaussElim(void *);
void GetLocalRow(int *);
void IterationBarrier(int * );
void BackSubstitution(void);
int CheckResult(void);

/*----------------- GLOBAL VARIABLES ------------------*/
#define MAXDIM 2000
#define TOLERANCE 0.5
int N = 4;
int numThreads = 2;

int NormRow = 0, currentRow, barrierCount;

volatile float A[MAXDIM][MAXDIM];
volatile float B[MAXDIM];
volatile float X[MAXDIM];

pthread_mutex_t currentRowLock;
pthread_mutex_t barrierLock;
pthread_cond_t nextRow;

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

/*----------------- MAIN ------------------*/
int main(int argc, char **argv)
{
    //Local Variables
    int seed = GetRandomSeed();
    srand(seed);
    
    pthread_t *threadIds = NULL;
    
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
    
    //Initialize counter variables.
    currentRow = NormRow + 1;
    barrierCount = numThreads - 1;
    
    //Allocate memory for the threadIds and init the mutexes and condition variable.
    threadIds = malloc(sizeof(pthread_t) * numThreads);
    
    pthread_mutex_init(&currentRowLock, NULL);
    pthread_mutex_init(&barrierLock, NULL);
    pthread_cond_init(&nextRow, NULL);
    
    //Start tracking time for performance analysis using GetTime().
    printf("\nPerforming Gaussian Elimination...\n");
    double startTime = GetTime();
    
    /*--Gaussian Elimination Here--*/
    for(int i = 0; i < numThreads; i++){
        //Create thread and check if it was created properly.
        if(pthread_create(&threadIds[i], NULL, GaussElim, NULL) != 0){
            printf("Error : could not create thread %d\n", i);
            return -1;
        }
    }
    //Join threads back together.
    for(int i = 0; i < numThreads; i++){
        if(pthread_join(threadIds[i], NULL) != 0){
            printf("Error : could not join thread %d\n", i);
            return -1;
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

/*----------------- GAUSSIAN ELIMINATION ------------------*/
void *GaussElim(void *ptr){
    //Local Variables
    int localRow = 0, normRow = 0;
    int row, col;
    
    float gaussMult;
    //Assign this thread its local row.
    GetLocalRow(&localRow);
    //While there are still rows to be normalized.
    while(normRow < N-1){
        //Elimnate each row the thread is assigned.
        for (row = localRow; row < N; row+= numThreads){
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
        
        //Wait at the barrier once the thread has elimnated the rows its responsible for.
        IterationBarrier(&normRow);
    }
    pthread_exit(0);
}

//GetLocalRow Function, used to determine which rows the thread is responsible for.
void GetLocalRow(int *localRow){
    //Lock the currentRowLock so that only one thread can be assigned the current row.
    pthread_mutex_lock(&currentRowLock);
    
    //Assign the current row to the thread.
    *localRow = currentRow;
    
    //Jump to the next local row for the thread to be assigned.
    currentRow ++;
 
    pthread_mutex_unlock(&currentRowLock);
}

//IterrationBarrier Function, used to synchronize all the threads for each iterration and update counter variables.
void IterationBarrier(int *normRow){
    //Lock the barrier mutex so only one thread can check and update the count variable at a time.
    pthread_mutex_lock(&barrierLock);
    
    //If this is the last thread at the barrier, update the counter variables and signal the next iteration.
    if(barrierCount == 0){
        NormRow ++;
        barrierCount = numThreads - 1;
        currentRow = NormRow + 1;
        
        pthread_cond_broadcast(&nextRow);
    }
    //Otherwise, report that this thread is at the barrier and block the thread on the nextRow condition.
    else{
        barrierCount --;
        pthread_cond_wait(&nextRow, &barrierLock);
    }
    //Update the threads local count of the normRow and unlock the barrierLock.
    *normRow = NormRow;
    pthread_mutex_unlock(&barrierLock);
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
