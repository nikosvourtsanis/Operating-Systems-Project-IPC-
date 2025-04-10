#ifndef EMPTY_H
#define EMPTY_H

#include <semaphore.h>
#include "sharedmem.h" 
#define MAX_CHILDREN 100 //max child processes
#define SEM_NAME_PREFIX "/sem_child_" //prefix for naming semaphores


void empty_resources(sem_t **semaphores, int max_children, shared_memory *shm, int shm_fd);

#endif // EMPTY_H
