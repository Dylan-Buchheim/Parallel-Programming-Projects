//
//  Example_7.3.c
//  CS4379_Project_2
//
//  Created by Dylan Buchheim on 3/28/20.
//  Copyright © 2020 Dylan Buchheim. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>
#include <getopt.h>
#include <limits.h>

//Global Variables ------------------
pthread_mutex_t minValueLock;
int minValue;
long subListSize;

//GetTime function, used to track performance, will return the current system time.
static double GetTime()
{
    struct timeval    tp;
    struct timezone    tzp;
    int i = 0;

    i = gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

//FindMin Function, finds the lowest value in the sublist and updates the global min is applicable.
void *FindMin(void *list){
    //Local variables
    int *subList = NULL;
    int localMin = 0;
    long i = 0;
    
    subList = (int *)(list);
    
    //Loop to find the localMin
    localMin = subList[0];
    for(i = 0; i < subListSize; i++){
        if(subList[i] < localMin){
            localMin = subList[i];
        }
    }
    //Thread locks the global min and updates it if necessary
    pthread_mutex_lock(&minValueLock);
    if(localMin < minValue){
        minValue = localMin;
    }
    pthread_mutex_unlock(&minValueLock);
    
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    //Local Variables
    int numThreads = 16;
    int ret = 0;
    long currentAddress = 0;
    int seed = 100;
    long numElems = 1000;
    int *list = NULL;
    pthread_t *threadIds = NULL;
    void *res = NULL;
    
    //Interpret the command line arguments.
    while ((ret = getopt(argc, argv, "S:T:N:")) != -1) {
        switch (ret) {
        case 'S':
            seed = atoi(optarg);
            break;
        case 'N':
            numElems = atol(optarg);
            break;
        case 'T':
            numThreads = atoi(optarg);
            break;
        case '?':
        default:
            printf("Unknown argument, please enter : -S <seed> -N <num_elems> -T <num_threads>\n");
            return -1;
            break;
        }
    }
    //Report the entered command line arguments.
    printf("Number of threads = %d, Number of elements = %ld, Seed value = %d\n", numThreads, numElems, seed);
    
    //Checks to make sure the arguments hold proper values, otherwise terminates execution.
    if (numThreads < 1) {
        printf("Error: Invalid number of threads entered.\n");
        return -1;
    }
    if (numElems <= (long)(numThreads)) {
        printf("Error : number of elements is less than the number of threads.\n");
        return -1;
    }
    
    //Set the minValue to the largest possible integer and set the seed.
    minValue = INT_MAX;
    srand(seed);

    //Init the list of elements and the list of threads, and check to make sure they init properly.
    list = malloc(sizeof(int) * numElems);
    threadIds = malloc(sizeof(pthread_t) * numThreads);
    
    if (list == NULL || threadIds == NULL) {
        printf("Error : problem with init.\n");
        return -1;
    }
    
    //Populate the list with random integers.
    for(long x = 0; x < numElems; x++){
        list[x] = rand();
    }
    
    //Calculate subListSize
    subListSize = (numElems / (long)numThreads) + (numElems % (long)numThreads);
    
    //Create threads ------------------------
    pthread_mutex_init(&minValueLock, NULL);
    
    //Start tracking time elapsed.
    double startTime = GetTime();
    
    for(int i = 0; i < numThreads; i++){
        //Create thread and check if it was created properly.
        if(pthread_create(&threadIds[i], NULL, &FindMin, &list[currentAddress]) != 0){
            printf("Error : could not create thread %d\n", i);
            return -1;
        }
        //Increment currentAddress by the size of the sublist.
        currentAddress += subListSize;
        //Check to make sure the next thread will not go off the end of the list.
        if((currentAddress + subListSize) > numElems){
            currentAddress = numElems - subListSize;
        }
    }
    //Join threads back together.
    for(int i = 0; i < numThreads; i++){
        if(pthread_join(threadIds[i], &res) != 0){
            printf("Error : could not join thread %d\n", i);
            return -1;
        }
    }
    //Finish tracking time elapsed.
    double finishTime = GetTime();
    
    //Report results.
    printf("Minimum value in the list: %d\n", minValue);
    printf("Time elapsed for %d threads: %f seconds.\n\n", numThreads, (finishTime - startTime));

    //Free up allocated memory.
    free(list);
    free(threadIds);
    threadIds = NULL;
    list = NULL;

    return 0;
}
