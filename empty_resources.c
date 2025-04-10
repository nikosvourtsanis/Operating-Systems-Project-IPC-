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
#include "empty_resources.h"

void empty_resources(sem_t **semaphores, int max_children, shared_memory *shm, int shm_fd){
    for(int i = 0; i < max_children; i++){
        if(semaphores[i] != SEM_FAILED ){
            char sem_name[32];
            sprintf(sem_name, "%s%d", SEM_NAME_PREFIX, i);
            if(sem_close(semaphores[i]) == -1) perror("sem_close failure");
            if(sem_unlink(sem_name) == -1) perror("sem_unlink failure");
        }
    }
    if (munmap(shm, sizeof(shared_memory)) == -1) perror("munmap failed");
    if (shm_fd != -1) close(shm_fd);
    if (shm_unlink("/shm_ipc") == -1) perror("shm_unlink failed"); 
}
    

