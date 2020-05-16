/*
 * File: lift_sim_A.c
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
#include <pthread.h> 
#include "lift_sim_A.h"
#include "circularQueue.h"

/* Variables and data that is shared between threads */
CircularQueue* buffer;
FILE* output;
pthread_mutex_t lock, fileLock;
pthread_cond_t full, empty;
int sleepT, totReq, totalMove;

int main(int argc, char* argv[])
{
    if(argc != 3) /* needs buffer size and sleep*/
    {
        printf("Error! Usage should be ./lift_sim_A <bufferSize> <timeCount>\n");
    }
    else
    {
        if(sleepT >= 0) /* sleep must be 0 or greater */
        {
            sleepT = atoi(argv[2]);
            setup(argv);
        }
        else
        {
            printf("Error in sleep Variable! Must be 0 or bigger\n");
        }
    }
    
    return 0;
}

/*
 * SUBMODULE: setup
 * IMPORT: argv(char**)
 * EXPORT: void
 * ASSERTION: Splits into the 4 threads and runs the functions
 * REFERENCE: 
 */
void setup(char* argv[])
{   
    pthread_t r_id, l1_id, l2_id, l3_id;
    int l1, l2, l3, l_Count;
    char currChar;

    FILE* input = fopen("sim_input", "r");
    output = fopen("sim_output", "a");
    l1 = 1;
    l2 = 2;
    l3 = 3;
    l_Count = 1;
    currChar = 'a';
    totalMove = 0;

    /* initialises the mutex and conds*/
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&fileLock, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);

    /* buffer must be one or bigger */
    if(atoi(argv[1]) >= 1)
    {
        buffer = createCircularQueue(atoi(argv[1]));
    }
    else
    {
        printf("Error: Buffer has to be 1 or larger!\n");
    }

    /* if input doesn't open */
    if(input == NULL)
    {
        perror("Error: could not open sim_input\n");
    }
    else if(ferror(input)) /* if error with input file */
    {
        perror("Error reading from sim_input\n");
    }
    else
    {
        /* loop through file counting number of lines */
        while(currChar != EOF)
        {
            currChar = fgetc(input);
            if(currChar == '\n')
            {
                l_Count++;
            }
        }
    }

    if(ferror(input))
    {
        perror("Error when reading from sim_input\n");
    }
    else
    {
        /* if no errors, close the input file */
        fclose(input);
    }

    /* must be 50 to 100 requests in file to work */
    if(l_Count >= 50 && l_Count <= 100)
    {
        /* create the 4 threads */
        pthread_create(&r_id, NULL, request, (void *)(&l_Count));
        pthread_create(&l1_id, NULL, lift, (void *)(&l1));
        pthread_create(&l2_id, NULL, lift, (void *)(&l2));
        pthread_create(&l3_id, NULL, lift, (void *)(&l3));

        /* exit the threads when they're done */
        pthread_join(r_id, NULL);
        pthread_join(l1_id, NULL);
        pthread_join(l2_id, NULL);
        pthread_join(l3_id, NULL);

        fprintf(output, "Total number of requests: %d\n", totReq);
        fprintf(output, "Total number of movements: %d\n", totalMove);
    }
    else
    {
        printf("Error! Input should be between 50 and 100 lines\n");
    }
    
    /* destory mutex and conds when done */
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&fileLock);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    /* free queue and close output file when done*/
    freeQueue(buffer);
    fclose(output);
}

/*
 * SUBMODULE: request
 * IMPORT: lCount(void *)
 * EXPORT: void *
 * ASSERTION: Runs the lift request function, adding to buffer
 * REFERENCE: 
 */
void *request(void* lCount)
{
    int start, dest, numReq;
    Entry* currEnt;
    FILE* input = fopen("sim_input", "r");
    numReq = 1;

    /* loop through entire file */
    while(!feof(input))
    {
        /* get next request and store it */
        fscanf(input, "%d %d", &start, &dest);
        currEnt = (Entry*)malloc(sizeof(Entry));
        currEnt->start = start;
        currEnt->dest = dest;

        /* lock to ensure no other thread is using buffer */
        pthread_mutex_lock(&lock);
        /* if buffer is full, wait */
        if(isFull(buffer))
        {
            pthread_cond_wait(&full, &lock);
        }

        /* enqueue next request onto buffer */
        enqueue(buffer, *currEnt);
        free(currEnt);
        printf("  Request: %d %d\n", start, dest);

        /* free lock and signal to show buffer is no longer empty*/
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&lock);

        /* lock the output file so no other thread is using it */
        pthread_mutex_lock(&fileLock);

        /* don't use this output if debugging is defined */
        #ifndef DEBUG_L
        #ifndef DEBUG_R
        fprintf(output, "--------------------------------------------\n");
        fprintf(output, "  New Lift Request From Floor %d to Floor %d\n", start, dest);
        fprintf(output, "  Request No: %d\n", numReq);
        fprintf(output, "--------------------------------------------\n");
        #endif
        #endif

        /* if debugging request function, print this instead*/
        #ifdef DEBUG_R
        fprintf(output, "%d %d\n", start, dest);
        #endif
        /* unlock file lock */
        pthread_mutex_unlock(&fileLock);

        numReq++;
    }

    /* if buffer has finished, set done to true */
    pthread_mutex_lock(&lock);
    setDone(buffer);
    printf("!!Done\n");
    pthread_mutex_unlock(&lock);

    /* close file and exit thread */
    fclose(input);
    printf("Request has exited\n");
    totReq = numReq -1;
    pthread_exit(0);
}

/*
 * SUBMODULE: lift
 * IMPORT: tid(void *)
 * EXPORT: void *
 * ASSERTION: Runs the lift functions, removing from the buffer
 * REFERENCE: 
 */
void *lift(void* tid)
{
    int start, dest, prevF, totMove, req, fin;
    Entry* currEnt;
    int done = 0;
    int id = *((int *)tid); /* set id for thread to passed int*/
    totMove = 0;
    req = 0;
    currEnt = NULL;

    /* Continue doing requests until finished */
    while(!done)
    {
        fin = 0;

        /* lock so no other thread is using buffer */
        pthread_mutex_lock(&lock);
        /* if buffer is empty and buffer hasn't finished adding requests, wait */
        if(isEmpty(buffer) && !isDone(buffer))
        {
            pthread_cond_wait(&empty, &lock);
        }
        
        /* if buffer is empty don't dequeue */
        if(!isEmpty(buffer))
        {
            currEnt = dequeue(buffer);
            printf("  Lift-%d: %d %d\n", id, currEnt->start, currEnt->dest);
            start = currEnt->start;
            dest = currEnt->dest;
            free(currEnt);
            fin = 1;
        }

        /* let requested know its not full anymore */
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&lock);

        /* This is so if singalled and buffer is empty it doesn't print nothing */
        if(fin)
        {
            /* lock so no other thread is outputting */
            pthread_mutex_lock(&fileLock);
            /* if debug is defined don't do this output */
            #ifndef DEBUG_L
            #ifndef DEBUG_R
            fprintf(output, "Lift-%d Operation\n", id);
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

            /* if debugging lift function, print this instead */
            #ifdef DEBUG_L
            fprintf(output, "%d %d\n", start, dest);
            #endif
            /* unlock lock on file */
            pthread_mutex_unlock(&fileLock);

            totMove += abs(prevF - start) + abs(dest - start);
            prevF = dest;
            req++;
        }

        /* if buffer is empty and is finished, exit loop */
        pthread_mutex_lock(&lock);
        if(isDone(buffer) && isEmpty(buffer))
        {
            done = 1;
        }
        pthread_mutex_unlock(&lock);



        sleep(sleepT);
    }
    pthread_cond_signal(&empty);
    pthread_cond_signal(&empty);
    
    pthread_mutex_lock(&lock);
    totalMove += totMove;
    pthread_mutex_unlock(&lock);

    /* end thread */
    printf("Lift-%d: has exited\n", id);
    pthread_exit(0);
}
