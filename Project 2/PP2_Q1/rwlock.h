//
//  rwlock.h
//  CS4379_Project_2
//
//  Created by Dylan Buchheim on 3/30/20.
//  Copyright © 2020 Dylan Buchheim. All rights reserved.
//

#include <pthread.h>

//rwlock struct -----------
typedef struct{
    int readers;
    int writer;
    pthread_cond_t readersProceed;
    pthread_cond_t writerProceed;
    int pendingWriters;
    pthread_mutex_t readWriteLock;
}rwlock_t;

//init function, sets initial values and initializes locks and conditions.
void rwlock_init (rwlock_t *lock){
    lock -> readers = 0;
    lock -> writer = 0;
    lock -> pendingWriters = 0;
    pthread_mutex_init(&(lock -> readWriteLock), NULL);
    pthread_cond_init(&(lock -> readersProceed), NULL);
    pthread_cond_init(&(lock -> writerProceed), NULL);
}

//rlock function, this is the function that a thread will use to attempt to establish a read lock.
void rwlock_rlock(rwlock_t *lock){
    /* if there is already a write lock or pending writers, perform condition
    wait, otherwise increment count of readers and grant read lock */
    
    pthread_mutex_lock(&(lock -> readWriteLock));
    while ((lock -> pendingWriters > 0) || (lock -> writer > 0)){
        pthread_cond_wait(&(lock -> readersProceed),&(lock -> readWriteLock));
    }
    lock -> readers ++;
    pthread_mutex_unlock(&(lock -> readWriteLock));
}

//wlock function, this is the function that a thread will use to attempt to establish a write lock.
void rwlock_wlock(rwlock_t *lock){
    /* if there are any readers or writers, this will increment pending writers and wait.
     Once this awoken, pending writers will decrease and writer will increase.*/
    
    pthread_mutex_lock(&(lock -> readWriteLock));
    while((lock -> writer > 0) || (lock -> readers > 0)){
        lock -> pendingWriters ++;
        pthread_cond_wait(&(lock -> writerProceed), &(lock -> readWriteLock));
    }
    lock -> pendingWriters --;
    lock -> writer ++;
    pthread_mutex_unlock(&(lock -> readWriteLock));
}

//unlock function, this is the function the threads will use to unlock the read-write lock.
void rwlock_unlock(rwlock_t *lock){
    /*If there is a write lock, then unlock the mutex. Otherwise, if there are read locks,
     decrement the count of readlocks. If the count is zero and there is a pending writer,
     signal it through. Otherwise, if there are pending readers, let them all through.*/
    
    pthread_mutex_lock(&(lock -> readWriteLock));
    if(lock -> writer > 0){
        lock -> writer = 0;
    }else if(lock -> readers > 0){
        lock -> readers --;
    }
    pthread_mutex_unlock(&(lock -> readWriteLock));
    if((lock -> readers == 0) && (lock -> pendingWriters > 0)){
        pthread_cond_signal(&(lock -> writerProceed));
    }else if(lock -> readers > 0){
        pthread_cond_broadcast(&(lock -> readersProceed));
    }
}
