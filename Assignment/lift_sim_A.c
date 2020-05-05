#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  /*Header file for sleep(). man 3 sleep for details. */
#include <pthread.h> 
#include "lift_sim_A.h"
#include "circularQueue.h"

CircularQueue* queue;

int main(int argc, char* argv[])
{
    /*pthread_t R_id, l1_id, l2_id, l3_id;

    pthread_create(&R_id, NULL, request, (void *)&R_id);
    pthread_create(&l1_id, NULL, lift, (void *)&l1_id);
    pthread_create(&l2_id, NULL, lift, (void *)&l2_id);
    pthread_create(&l3_id, NULL, lift, (void *)&l3_id);
    pthread_exit(NULL); */
    entry* ent1;
    entry* ent2;
    entry* ent3;

    ent1 = (entry*)malloc(sizeof(entry));
    ent1->start = 5;
    ent1->dest = 6;

    ent2 = (entry*)malloc(sizeof(entry));
    ent2->start = 1;
    ent2->dest = 3;

    ent3 = (entry*)malloc(sizeof(entry));
    ent3->start = 9;
    ent3->dest = 20;

    queue = createCircularQueue(atoi(argv[1]));
    enqueue(queue, *ent1);
    enqueue(queue, *ent2);
    enqueue(queue, *ent3);
    printQueue(queue);
    freeQueue(queue);

    free(ent1);
    free(ent2);
    free(ent3);

    return 0;
}

/* void *request(void* vargp)
{
    FILE* input = fopen("sim_inpupt", "r");
    int* id = (int *)vargp;
    printf("R: Hello, this is my thread ID: %d\n", *id);

    return NULL;
}

void *lift(void* vargp)
{
    int* id = (int *)vargp;
    printf("L: Hello, this is my thread ID: %d\n", *id);
    return NULL;
}
*/
