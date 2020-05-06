#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <unistd.h>  /*Header file for sleep(). man 3 sleep for details. */
#include "lift_sim_A.h"
#include "circularQueue.h"

CircularQueue* buffer;

int main(int argc, char* argv[])
{
    pthread_t R_id, l1_id, l2_id, l3_id;
    int rid, l1, l2, l3;
    rid = 0;
    l1 = 1;
    l2 = 2;
    l3 = 3;

    buffer = createCircularQueue(atoi(argv[1]));

    pthread_create(&R_id, NULL, request, (void *)(&rid));
    pthread_create(&l1_id, NULL, lift, (void *)(&l1));
    pthread_create(&l2_id, NULL, lift, (void *)(&l2));
    pthread_create(&l3_id, NULL, lift, (void *)(&l3));
    pthread_join(R_id, NULL);
    pthread_join(l1_id, NULL);
    pthread_join(l2_id, NULL);
    pthread_join(l3_id, NULL);

    freeQueue(buffer);

    return 0;
}

void *request(void* vargp)
{
    int source, dest;
    entry* ent;
    FILE* input = fopen("sim_input", "r");
    FILE* output = fopen("sim_output", "a");
    int i = 0;
    char line[7];
    int* id = (int *)vargp;

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
        while(i < 50)
        {
            fgets(line, 7, input);
            sscanf(line, "%d %d\n", &source, &dest);
            ent = (entry*)malloc(sizeof(entry));
            ent->start = source;
            ent->dest = dest;
            enqueue(buffer, *ent);
            free(ent);
            i++;
        }
    }

    if(ferror(input))
    {
        perror("Error when reading from sim_output\n");
    }
    /* otherwise close both files */
    else
    {
        fclose(input);
    }


    if(output == NULL)
    {
        perror("Error: could not open sim_output\n");
    }
    else if(ferror(output))
    {
        perror("Error reading from sim_output\n");
    }
    else
    {
        writeQueue(buffer, output);
    }

    if(ferror(output))
    {
        perror("Error when reading from sim_output\n");
    }
    /* otherwise close both files */
    else
    {
        fclose(output);
    }

    printf("R: Hello, this is my thread ID: %d\n", *id);

    pthread_exit(0);

    return NULL;
}

void *lift(void* vargp)
{
    FILE* output = fopen("sim_output", "a");
    int* id = (int *)vargp;

    if(output == NULL)
    {
        perror("Error: could not open sim_output\n");
    }
    else if(ferror(output))
    {
        perror("Error reading from sim_output\n");
    }
    else
    {
        writeQueue(buffer, output);
    }

    if(ferror(output))
    {
        perror("Error when reading from sim_output\n");
    }
    /* otherwise close both files */
    else
    {
        fclose(output);
    }

    printf("L: Hello, this is my thread ID: %d\n", *id);
    pthread_exit(0);
    return NULL;
}
