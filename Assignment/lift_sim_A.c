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
    pthread_t R_id, l1_id, l2_id, l3_id;
    int l1, l2, l3;
    l1 = 1;
    l2 = 2;
    l3 = 3;

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&fileLock, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    buffer = createCircularQueue(atoi(argv[1]));
    output = fopen("sim_output", "a");

    pthread_create(&R_id, NULL, request, NULL);
    sleep(1);
    pthread_create(&l1_id, NULL, lift, (void *)(&l1));
    pthread_create(&l2_id, NULL, lift, (void *)(&l2));
    pthread_create(&l3_id, NULL, lift, (void *)(&l3));
    pthread_join(R_id, NULL);
    pthread_join(l1_id, NULL);
    pthread_join(l2_id, NULL);
    pthread_join(l3_id, NULL);
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&fileLock);

    freeQueue(buffer);
    fclose(output);

    return 0;
}

void *request(void* vargp)
{
    int source, dest, j;
    entry* ent;
    /*char curr = 'a';*/
    FILE* temp = fopen("sim_input", "r");
    FILE* input = fopen("sim_input", "r");
    /*int i = 1;*/
    char line[7];

    if(input == NULL)
    {
        perror("Error: could not open sim_input\n");
    }
    else if(ferror(input))
    {
        perror("Error reading from sim_input\n");
    }
    else
    {/*
        while(curr != EOF)
        {
            curr = fgetc(temp);
            if(curr == '\n')
            {
                i++;
            }
        }
        if((i >= 50)&&(i <= 100))
        {*/
            for (j = 0; j < 4; j++)
            {
                fgets(line, 7, input);
                sscanf(line, "%d %d\n", &source, &dest);
                ent = (entry*)malloc(sizeof(entry));
                ent->start = source;
                ent->dest = dest;

                pthread_mutex_lock(&lock);
                if(isFull(buffer) == 1)
                {
                    pthread_cond_wait(&full, &lock);
                }

                enqueue(buffer, *ent);

                pthread_mutex_lock(&fileLock);
                fprintf(output, "Buffer Entry Added:\n");
                printf("R: Added Entry (%d, %d)\n", ent->start, ent->dest);
                pthread_mutex_unlock(&fileLock);

                pthread_cond_signal(&empty);
                pthread_mutex_unlock(&lock);
                
                free(ent);
            }
        /*}
        else
        {
            printf("Illgal number of lines in input. (50 <= num <= 100)\n");
        }*/
    }

    if(ferror(input))
    {
        perror("Error when reading from sim_output\n");
    }
    /* otherwise close both files */
    else
    {
        fclose(input);
        fclose(temp);
    }

    pthread_exit(0);

    return NULL;
}

void *lift(void* vargp)
{
    int* id = (int *)vargp;
    entry* temp;

    do
    {
        pthread_mutex_lock(&lock);
        if(isEmpty(buffer) == 1)
        {
            pthread_cond_wait(&empty, &lock);
        }

        pthread_mutex_lock(&fileLock);
        temp = dequeue(buffer);
        writeEntry(temp, output);
        free(temp);
        pthread_mutex_unlock(&fileLock);
        printf("yeet: %d\n", *id);

        pthread_cond_signal(&full);
        pthread_mutex_unlock(&lock);
    } 
    while (isEmpty(buffer) != 1);

    pthread_exit(0);
    return NULL;
}
