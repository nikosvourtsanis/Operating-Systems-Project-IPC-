/*  sdi2200022 Nikolaos Vourtsanis 
    Operating Systems Course 
    Exercise 1 - 2024-25        */
//main program..
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
#include "child.h"
#include "empty_resources.h"

#define MAX_CHILDREN 100 //max child processes
#define SEM_NAME_PREFIX "/sem_child_" //prefix for naming semaphores

int main(int argc, char **argv){
    if(argc < 4){
        fprintf(stderr, "correct usage of program: %s <configuration file> <text file> <max children>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *config_file = argv[1]; //configuration file given to us
    char *text_file = argv[2]; //text file
    int max = atoi(argv[3]); //max number of child processes from the command line

    if(max > MAX_CHILDREN){
        fprintf(stderr, "Max children error.They should not be > %d\n", MAX_CHILDREN);
        exit(EXIT_FAILURE);
    }

    //opening the configuration file for reading 
    FILE *config = fopen(config_file, "r");
    if(!config){
        perror("error openning the configuration file");
        exit(EXIT_FAILURE);
    }


    //opening the text file 
    FILE *text = fopen(text_file, "r");
    if(!text){
        perror("error openning the text file");
        exit(EXIT_FAILURE);
    }

    //shared memory segment..
    int shm_fd = shm_open("/shm_ipc", O_CREAT | O_RDWR, 0666); //fd for file descriptor
    if(shm_fd == -1){
        perror("shm_open");
        fclose(config);
        fclose(text);
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(shared_memory)) == -1){
        perror("ftruncate");
        close(shm_fd);
        shm_unlink("/shm_ipc"); //removing the shared memory segment
        fclose(config);
        fclose(text);
        exit(EXIT_FAILURE);
    }

    //mapping the shared memory segment
    shared_memory *shm = mmap(NULL, sizeof(shared_memory), PROT_READ | PROT_WRITE,
                             MAP_SHARED, shm_fd, 0);
    if(shm == MAP_FAILED){
        perror("mmap");
        close(shm_fd); //'fd' is no longer needed
        shm_unlink("/shm_ipc"); //removing the shared memory segment
        fclose(config);
        fclose(text);
        exit(EXIT_FAILURE);
    }

    //initializing the semaphores
    sem_t *semaphores[MAX_CHILDREN]; //semaphores array
    for(int i = 0; i < max; i++){
        char sem_name[32];
        sprintf(sem_name, "%s%d", SEM_NAME_PREFIX, i); //created a sem name for each child/client
        semaphores[i] = sem_open(sem_name, O_CREAT | O_RDWR, 0666, 0); //connects to, and optionally creates, a named semaphore
        if(semaphores[i] == SEM_FAILED){
            perror("sem_open");
            empty_resources(semaphores, i, shm, shm_fd);  //cleanup
            fclose(config);
            fclose(text);
            exit(EXIT_FAILURE);
        }
    }

    //array to store the Ids od child/client processes
    pid_t children[MAX_CHILDREN] = {0};

    char line[256];
    //char *line =  NULL;
    //int line_len = 0;

    while (fgets(line, sizeof(line), config)) {
        if (strlen(line) == 0 || line[0] == '\n') {
            fprintf(stderr, "skipping empty or newline-only line in configuration file\n");
            continue;
        }
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0'; //trim newline for cleaner parsing
        

        /*line_len = strlen(line) + 1;
        char *dynamicLine = (char *)malloc(line_len * sizeof(char));
        if(!dynamicLine){
            perror("failed to allocate memory for the line\n");
            break;
        }*/

        //strncpy(dynamicLine, line, line_len);

        int timestamp;
        int childID;
        char command;

        //parsing the config file line
        if (sscanf(line, "%d C%d %c", &timestamp, &childID, &command) != 3) {
            fprintf(stderr, "Invalid command format: '%s' in line '%s'. correct format: '<timestamp> C<child_id> <command>'.\n", line, config_file);
            continue;
        }

        //S = SPAWN command..
        if(command == 'S'){
            if(children[childID] == 0){
                pid_t pid = fork(); //create new process
                if(pid == 0){
                    child(childID, semaphores[childID], shm); //starting the child process
                }
                else{
                    children[childID] = pid;
                    printf("[parent] Spawned child %d with pid = %d\n", childID, pid); //child spawned--debugging
                }
            }
        }   else if(command == 'T'){//T = TERMINATE command
                if(children[childID] != 0){ //checking if child/client process is active
                    shm->terminate = 1; //termination flag set in shared memory
                    sem_post(semaphores[childID]); //telling the child it should now terminate
                    waitpid(children[childID], NULL, 0); // waitind for the child to exit
                    printf("[Parent] Terminated child %d\n", childID); //child terminated-- debuggging
                    children[childID] = 0; //child process marked as INACTIVE
                }
        }

        //free(dynamicLine);        

          //randomly send a line from the text file to an active child/client process
        if (fgets(line, sizeof(line), text)) {
            //int active_child = rand() % max; //getting a random child/client process
           // if (children[active_child] != 0) { //first, ensure the selected child is active
           int active_child = -1;
           for (int i = 0; i < max; i++) {
               if(children[i] != 0) { // Find the first active child
                   active_child = i;
                   break;
            }
        }

            if (active_child != -1) { // Ensure an active child was found
                snprintf(shm->message, SHM_SIZE, "%s", line);
                shm->terminate = 0;
                sem_post(semaphores[active_child]);
                printf("[Parent] Sending message to child %d: %s\n", active_child, line);
            } else {
                fprintf(stderr, "No active child available to send message.\n");
            }

                    }
                }

    //free all the resources
    for (int i = 0; i < max; i++) {
        if (children[i] != 0) { //if the child process is still active
            shm->terminate = 1; //setting the termination flag
            sem_post(semaphores[i]); //telling  the child to terminate
            waitpid(children[i], NULL, 0); // now wait for the child to exit
        }
    }

    empty_resources(semaphores, max, shm, shm_fd);
    //closing finally the config and text files
    fclose(config);
    fclose(text);

    //debugging print
    printf("[parent] all resources are now EMPTY\n");

    return 0;

}


    

