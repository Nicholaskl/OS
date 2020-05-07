#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  /*Header file for sleep(). man 3 sleep for details. */
#include <pthread.h> 
#include "lift_sim_A.h"
#include "circularQueue.h"

CircularQueue* buffer;
FILE* output;
pthread_mutex_t lock, fileLock;
pthread_cond_t full, empty;

int main(int argc, char* argv[])
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

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&fileLock, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    if(atoi(argv[1]) >= 1)
    {
        buffer = createCircularQueue(atoi(argv[1]));
    }
    else
    {
        printf("Error: Buffer has to be 1 or larger!\n");
    }

    if(input == NULL)
    {
        perror("Error: could not open sim_input\n");
    }
    else if(ferror(input))
    {
        perror("Error reading from sim_input\n");
    }
    else
    {
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
        fclose(input);
    }

    pthread_create(&r_id, NULL, request, (void *)(&l_Count));
    pthread_create(&l1_id, NULL, lift, (void *)(&l1));
    pthread_create(&l2_id, NULL, lift, (void *)(&l2));
    pthread_create(&l3_id, NULL, lift, (void *)(&l3));

    pthread_join(r_id, NULL);
    pthread_join(l1_id, NULL);
    pthread_join(l2_id, NULL);
    pthread_join(l3_id, NULL);

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&fileLock);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    freeQueue(buffer);
    fclose(output);

    return 0;
}

void *request(void* lCount)
{
    int source, dest, numReq;
    entry* currEnt;
    FILE* input = fopen("sim_input", "r");
    numReq = 1;

    while(!feof(input))
    {
        fscanf(input, "%d %d", &source, &dest);
        currEnt = (entry*)malloc(sizeof(entry));
        currEnt->start = source;
        currEnt->dest = dest;

        pthread_mutex_lock(&lock);
        if(isFull(buffer))
        {
            pthread_cond_wait(&full, &lock);
        }

        enqueue(buffer, *currEnt);
        printf("  Request: %d %d\n", source, dest);

        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&lock);
        /*
        pthread_mutex_lock(&fileLock);
        fprintf(output, "--------------------------------------------\n");
        fprintf(output, "  New Lift Request From Floor %d to Floor %d\n", source, dest);
        fprintf(output, "  Request No: %d\n", numReq);
        fprintf(output, "--------------------------------------------\n");
        pthread_mutex_unlock(&fileLock);*/

        numReq++;

        /*fprintf(output, "Request: %d %d\n", source, dest);*/
    }
    pthread_mutex_lock(&lock);
    setDone(buffer);
    printf("!!Done\n");
    pthread_mutex_unlock(&lock);

    printf("Request has exited\n");
    pthread_exit(0);
}

void *lift(void* tid)
{
    int prevF, done, id, req, totMove;
    entry* currEnt = NULL;
    prevF = 0;
    req = 1;
    done = 0;
    totMove = 0;
    id = *((int *)tid);

    while(!done)
    {
        pthread_mutex_lock(&lock);
        if(isEmpty(buffer) && !isDone(buffer))
        {
            pthread_cond_wait(&empty, &lock);
        }
        
        if(!isEmpty(buffer))
        {
            currEnt = dequeue(buffer);
            printf("  Lift-%d: %d %d\n", id, currEnt->start, currEnt->dest);
        }
        if(isDone(buffer) && isEmpty(buffer))
        {
            done = 1;
        }

        pthread_cond_signal(&full);
        pthread_mutex_unlock(&lock);
        /*
        pthread_mutex_lock(&fileLock);
        fprintf(output, "Lift-%d Operation\n", id);
        fprintf(output, "Previous position: Floor %d\n", prevF);
        fprintf(output, "Request: Floor %d to Floor %d\n", currEnt->start, currEnt->dest);
        fprintf(output, "Detail operations:\n");
        fprintf(output, "   Go from Floor %d to Floor %d\n", prevF, currEnt->start);
        fprintf(output, "   Go from Floor %d to Floor %d\n", currEnt->start, currEnt->dest);
        fprintf(output, "   #movement for this request: %d\n", abs(currEnt->dest - currEnt->start));
        fprintf(output, "   #request: %d\n", req);
        fprintf(output, "   Total #movement: %d\n", totMove + abs(currEnt->dest - currEnt->start));
        fprintf(output, "Current Position: Floor %d\n", currEnt->dest);
        pthread_mutex_unlock(&fileLock);*/

        prevF = currEnt->dest;
        totMove += abs(currEnt->dest - currEnt->start);
        req++;
        free(currEnt);
        sleep(1);
    }
    
    printf("Lift-%d: has exited\n", id);
    pthread_exit(0);
}
