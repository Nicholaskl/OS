/*
 * File: lift_sim_B.c
 * File Created: Thursday, 7th May 2020
 * Author: Nicholas Klvana-Hooper
 * -----
 * Last Modified: Tuesday, 12th May 2020
 * Modified By: Nicholas Klvana-Hooper
 * -----
 * Purpose: Runs a lift 3 lift simulator for a 20 story building
 * Reference: 
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <string.h> 
#include <sys/mman.h>
#include <fcntl.h> 
#include "circularQueue.h"
#include "lift_sim_B.h"

int main(int argc, char* argv[])
{
    pid_t l_Count, sleepT;
    char currChar = 'a';
    FILE* input = fopen("sim_input", "r"); /* Open input file to check number of lines */ 

    if(argc != 3)
    {
        printf("Error! Usage should be ./lift_sim_B <bufferSize> <timeCount\n");
    }
    else
    {
        createCircularQueue(atoi(argv[1])); /* Initialise queue with size of input */
        sleepT = atoi(argv[2]); /* Sleep for requested amount of time */

        if(input == NULL) /* If doesn't exist */
        {
            perror("Error: could not open sim_input\n");
        }
        else if(ferror(input)) /* If error, print */
        {
            perror("Error reading from sim_input\n");
        }
        else
        {
            l_Count = 1; /* Line count starts at one*/
            while(currChar != EOF) /* Count number of lines in a file */
            {
                currChar = fgetc(input);
                if(currChar == '\n')
                {
                    l_Count++;
                }
            }
        }

        if(ferror(input)) /* If Error, print */
        {
            perror("Error when reading from sim_input\n");
        }
        else /* Or close the file */
        {
            fclose(input);
        }

        /* Must be between 50 and 100 requests */
        if(l_Count >= 50 && l_Count <= 100)
        {
            simFunc(sleepT, atoi(argv[1]));
        }    
        else
        {
            printf("Error! Input should be between 50 and 100 lines\n");
        }
    }

    return 0;
}

/*
 * SUBMODULE: simFunc
 * IMPORT: sleepT(int), buffSize(int)
 * EXPORT: void
 * ASSERTION: Runs the forking for the 4 child processes, creating semaphores
 * REFERENCE: 
 */
void simFunc(int sleepT, int buffSize)
{
    pid_t rid, l1, l2, l3;
    int full_fd, lock_fd, empty_fd, fileL_fd, totalMove_fd, totalReq_fd;
    int *totalMove, *totalReq;
    sem_t *full, *empty, *lock, *fileL;
    FILE* output;

    /* Creates shared memory for and initialises sempahores */
    full_fd = shm_open("/full", O_CREAT | O_RDWR, 0666); 
    ftruncate(full_fd, sizeof(sem_t)); 
    full = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, full_fd, 0);
    sem_init(full, 1, 0);

    lock_fd = shm_open("/lock", O_CREAT | O_RDWR, 0666); 
    ftruncate(lock_fd, sizeof(sem_t)); 
    lock = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, lock_fd, 0);
    sem_init(lock, 1, 1);

    empty_fd = shm_open("/empty", O_CREAT | O_RDWR, 0666); 
    ftruncate(empty_fd, sizeof(sem_t)); 
    empty = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, empty_fd, 0);
    sem_init(empty, 1, buffSize);

    fileL_fd = shm_open("/fileL", O_CREAT | O_RDWR, 0666); 
    ftruncate(fileL_fd, sizeof(sem_t)); 
    fileL = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fileL_fd, 0);
    sem_init(fileL, 1, 1);

    totalReq_fd = shm_open("/totalReq", O_CREAT | O_RDWR, 0666); 
    ftruncate(totalReq_fd, sizeof(int)); 
    totalReq = (int*) mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, totalReq_fd, 0);
    *totalReq = 0;

    totalMove_fd = shm_open("/totalMove", O_CREAT | O_RDWR, 0666); 
    ftruncate(totalMove_fd, sizeof(int)); 
    totalMove = (int*) mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, totalMove_fd, 0);
    *totalMove = 0;
    
    /* Forks to create the request child */
    rid = fork();
    if(!rid)
    {
        /* If this is request process, print and run the function */
        printf("Request Process Forked-> pid: %d ppid: %d\n", getpid(), getppid());
        request();
    }
    else
    {
        /* If this is the parent process, fork */
        l1 = fork();
        if(!l1)
        {
            /* If this is lift-1 process, print and run the function */
            printf("Lift-1 Process Forked-> pid: %d ppid: %d\n", getpid(), getppid());
            lift(sleepT);
        }
        else
        {
            /* If this is the parent process, fork */
            l2 = fork();
            if(!l2)
            {
                /* If this is lift-2 process, print and run the function */
                printf("Lift-2 Process Forked-> pid: %d ppid: %d\n", getpid(), getppid());
                lift(sleepT);
            }
            else
            {
                /* If this is the parent process, fork */
                l3 = fork();
                if(!l3)
                {
                    /* If this is lift-3 process, print and run the function */
                    printf("Lift-3 Process Forked-> pid: %d ppid: %d\n", getpid(), getppid());
                    lift(sleepT);
                }
                else
                {
                    /* If this is the parent process, destory semaphores and print */
                    printf("I'm the parent process-> pid: %d\n", getpid());
                    /* wait for the child processes to finish */
                    wait(NULL);
                    wait(NULL);
                    wait(NULL);
                    wait(NULL);

                    freeQueue(); /* free all shared memory in queue */

                    output = fopen("sim_output", "a");
                    fprintf(output, "Total number of requests: %d\n", *totalReq);
                    fprintf(output, "Total number of movements: %d\n", *totalMove);

                    shm_unlink("/full_sem");
                    shm_unlink("/empty_sem");
                    shm_unlink("/lock_sem");
                    shm_unlink("/fileL_sem");
                    shm_unlink("/totalMove");
                    shm_unlink("/totalReq");
                }
            }
        }
    }
}

/*
 * SUBMODULE: request
 * IMPORT: void
 * EXPORT: void
 * ASSERTION: Runs the lift request function, adding to buffer
 * REFERENCE: 
 */
void request(void)
{
    int source, dest, full_fd, lock_fd, empty_fd, fileL_fd, totalReq_fd;
    sem_t *full, *lock, *empty, *fileL;
    FILE* output;
    FILE* input = fopen("sim_input", "r");
    int numReq = 1;
    int *totalReq;
    
    /* Opens memory to write/read semaphores */
    full_fd = shm_open("/full", O_RDWR, 0666); 
    ftruncate(full_fd, sizeof(sem_t)); 
    full = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, full_fd, 0);

    lock_fd = shm_open("/lock", O_RDWR, 0666); 
    ftruncate(lock_fd, sizeof(sem_t)); 
    lock = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, lock_fd, 0);

    empty_fd = shm_open("/empty", O_RDWR, 0666); 
    ftruncate(empty_fd, sizeof(sem_t)); 
    empty = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, empty_fd, 0);

    fileL_fd = shm_open("/fileL", O_RDWR, 0666); 
    ftruncate(fileL_fd, sizeof(sem_t)); 
    fileL = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fileL_fd, 0);

    totalReq_fd = shm_open("/totalReq", O_CREAT | O_RDWR, 0666); 
    ftruncate(totalReq_fd, sizeof(int)); 
    totalReq = (int*) mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, totalReq_fd, 0); 

    /* Continuing loop through entire file */
    while(!feof(input))
    {
        /* Read next line */
        fscanf(input, "%d %d\n", &source, &dest);

        /* Wait/signal on sempahore/lock and write to buffer */
        sem_wait(empty);
        sem_wait(lock);

        enqueue(source, dest);
        printf("Enqueued: %d, %d\n", source, dest);

        sem_post(lock);
        sem_post(full);

        /* Wait/signal on semaphore/lock to write to output file */
        sem_wait(fileL);

        output = fopen("sim_output", "a"); /* open file to output to */
        #ifndef DEBUG_R /* for debugging request function */
        #ifndef DEBUG_L /* for debugging lift function */
        /* Print to file */
        fprintf(output, "--------------------------------------------\n");
        fprintf(output, "  New Lift Request From Floor %d to Floor %d\n", source, dest);
        fprintf(output, "  Request No: %d\n", numReq);
        fprintf(output, "--------------------------------------------\n");
        #endif
        #endif
        #ifdef DEBUG_R /* for debugging request function */
        fprintf(output, "%d %d\n", source, dest);
        #endif

        /* close file and post semaphore to signifiy finished outputting */
        fclose(output);
        sem_post(fileL);

        numReq++;
    }
    
    /* Once requesting has gone through input, set done to 1 */
    sem_wait(lock); 
    setDone();
    *totalReq = numReq -1; /* Export totalReq to parent process */
    sem_post(lock);
    
    /* Unmap semaphores from current function */
    munmap(full, sizeof(sem_t));
    munmap(empty, sizeof(sem_t));
    munmap(lock, sizeof(sem_t));
    munmap(fileL, sizeof(sem_t));
    munmap(totalReq, sizeof(int));

    /* Close semaphores */
    close(full_fd);
    close(lock_fd);
    close(empty_fd);
    close(fileL_fd);
    close(totalReq_fd);

    /* Close input file */
    fclose(input);
}

/*
 * SUBMODULE: lift
 * IMPORT: sleepT(int)
 * EXPORT: void
 * ASSERTION: Runs the lift functions, removing from the buffer
 * REFERENCE: 
 */
void lift(int sleepT)
{
    int full_fd, lock_fd, empty_fd, fileL_fd, buff_fd, totalMove_fd, writeNow, start, dest;
    sem_t *full, *lock, *empty, *fileL;
    Entry *ent;
    FILE* output;
    CircularQueue* buffer;
    int done = 0;
    int* totalMove;

    /* if Debugging don't include this */
    #ifndef DEBUG_R
    #ifndef DEBUG_L
    int prevF = 0;
    int req = 0;
    int totMove = 0;
    #endif
    #endif

    /* Open and map all shared memory for sempahores and buffer */
    full_fd = shm_open("/full", O_CREAT | O_RDWR, 0666); 
    ftruncate(full_fd, sizeof(sem_t)); 
    full = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, full_fd, 0);

    lock_fd = shm_open("/lock", O_CREAT | O_RDWR, 0666); 
    ftruncate(lock_fd, sizeof(sem_t)); 
    lock = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, lock_fd, 0);

    empty_fd = shm_open("/empty", O_CREAT | O_RDWR, 0666); 
    ftruncate(empty_fd, sizeof(sem_t)); 
    empty = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, empty_fd, 0);

    fileL_fd = shm_open("/fileL", O_CREAT | O_RDWR, 0666); 
    ftruncate(fileL_fd, sizeof(sem_t)); 
    fileL = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fileL_fd, 0);

    buff_fd = shm_open("/BUFFER", O_RDONLY, 0666);
    buffer = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, buff_fd, 0);

    totalMove_fd = shm_open("/totalMove", O_CREAT | O_RDWR, 0666); 
    ftruncate(totalMove_fd, sizeof(int)); 
    totalMove = (int*) mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, totalMove_fd, 0); 

    while(!done)
    {
        start = 0;
        dest = 0;
        writeNow = 0;
        prevF = 0;

        /* If buffer is empty and is not yet finished adding, wait */
        sem_wait(lock);
        if(isEmpty() && !isDone())
        {
            sem_post(lock);
            sem_wait(full);
        }
        else /* Otherwise let go of lock */
        {
            sem_post(lock);
        }

        /* lock and then deque from buffer */
        sem_wait(lock);
        if(!isEmpty())
        {
            ent = dequeue();
            start = ent->start;
            dest = ent->dest;
            printf("  Lift-%d: %d %d\n", getpid() - getppid() -1, start, dest);
            free(ent);
            writeNow = 1; /* Tells program that this iteration it should print */
                          /* So once no more requests, doesn't print anything */
        }
        sem_post(lock);

        /* If an request is dequeued tell request function its not full anymore */
        if(writeNow) 
        {
            sem_post(empty);
        }

        /* Write to file if dequeued item */
        if(writeNow)
        {   
            sem_wait(fileL); /*Lock file */
            output = fopen("sim_output", "a");
            
            #ifndef DEBUG_R /* unless debugging, then don't print this */
            #ifndef DEBUG_L
            fprintf(output, "Lift-%d Operation\n", getpid()-getppid()-1);
            fprintf(output, "Previous position: Floor %d\n", prevF);
            fprintf(output, "Request: Floor %d to Floor %d\n", start, dest);
            fprintf(output, "Detail operations:\n");
            fprintf(output, "   Go from Floor %d to Floor %d\n", prevF, start);
            fprintf(output, "   Go from Floor %d to Floor %d\n", start, dest);
            fprintf(output, "   #movement for this request: %d\n", abs(prevF - start) + abs(dest - start));
            fprintf(output, "   #request: %d\n", req);
            fprintf(output, "   Total #movement: %d\n", totMove + abs(prevF - start) + abs(dest - start));
            fprintf(output, "Current Position: Floor %d\n", dest);
            #endif
            #endif
            #ifdef DEBUG_L /* If debugging lift function, only print this */
            fprintf(output, "%d %d\n", source, dest);
            #endif

            /* Close file and unlock the file lock */
            fclose(output);
            sem_post(fileL);
        }

        /* If buffer is empty and has finished adding, exit loop */
        sem_wait(lock);
        if(isDone() && isEmpty())
        {
            done = 1;
            printf("lift-%d finished\n", getpid() - getppid() -1);
        }
        sem_post(lock);
        
        #ifndef DEBUG_R /* if debugging don't do this */
        #ifndef DEBUG_L
        totMove += abs(prevF - start) + abs(dest - start);
        prevF = dest;
        req++;
        #endif
        #endif

        sleep(sleepT); /* Sleep for specified time simulating lift movement*/
    }
    /* When finished tell other two lifts to exit */
    sem_post(full);
    sem_post(full);

    /* Total move export to parent process */
    sem_wait(lock);
    *totalMove += totMove;
    sem_post(lock);

    /* unmap seamphore and buffer from this function */
    munmap(full, sizeof(sem_t));
    munmap(empty, sizeof(sem_t));
    munmap(lock, sizeof(sem_t));
    munmap(fileL, sizeof(sem_t));
    munmap(buffer, sizeof(CircularQueue));
    munmap(totalMove, sizeof(int));

    /* Close sempahores and buffer */
    close(full_fd);
    close(lock_fd);
    close(empty_fd);
    close(fileL_fd);
    close(buff_fd);
    close(totalMove_fd);
}
