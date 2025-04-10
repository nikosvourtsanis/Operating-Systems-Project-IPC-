/*  sdi2200022 Nikolaos Vourtsanis 
    Operating Systems Course 
    Exercise 1 - 2024-25        */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_CHILDREN 100 //max child processes
#define SEM_NAME_PREFIX "/sem_child_" //prefix for naming semaphores
#include "sharedmem.h"

void child(int child_id, sem_t *sem, shared_memory *shm)
{
    char sem_name[32];
    sprintf(sem_name, "%s%d", SEM_NAME_PREFIX, child_id); //sprintf used to form a string and store the result in a char array
                                                          //creating a unique semaphore name for this child
    int count = 0; //counter for messages recieved by the child
    /*time_t start = time(NULL); //we get the exact timestamp that the child started
    time_t end;  //variable to keep track of the end time of the child*/
    clock_t start_t, end_t;
    double time_total;
    start_t = clock();

    printf("[Child %d] Process started.\n", child_id); // Debuging: Child process started
    while(1){
        sem_wait(sem); //wait for the parent process

        if(shm->terminate){
            end_t = clock();
            time_total = (double)(end_t - start_t)/CLOCKS_PER_SEC;
            printf("child %d terminating..\n messages recieved: %d\n time: %lf sec\n", child_id, count, time_total);

            if (sem_close(sem) == -1) perror("sem_close failed in child");

            exit(0); //exit the child process
        }

        //printing the message the child/client got
        printf("child %d recieved message: %s\n", child_id, shm->message);
        count++; //added one more message
    }
}