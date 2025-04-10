/*  sdi2200022 Nikolaos Vourtsanis 
    Operating Systems Course 
    Exercise 1 - 2024-25        */
#ifndef SHAREDMEM_H
#define SHAREDMEM_H

#include <stddef.h> // For size_t

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

#define SHM_SIZE 1024   /* Maximum size for exchanged string */
//struct for shared memory
typedef struct {
    char message[SHM_SIZE]; //buffer for message passing through the shared memory segment
    int terminate;          //flag for termination 
} shared_memory;


#endif // SHAREDMEM_H