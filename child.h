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
#include "sharedmem.h"

#define MAX_CHILDREN 100 //max child processes
#define SEM_NAME_PREFIX "/sem_child_" //prefix for naming semaphores

//child process function prototype
void child(int child_id, sem_t *sem, shared_memory *shm);
