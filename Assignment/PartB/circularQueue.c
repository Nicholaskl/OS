/*
 * File: circularQueue.c
 * File Created: Thursday, 7th May 2020
 * Author: Nicholas Klvana-Hooper
 * -----
 * Last Modified: Tuesday, 12th May 2020
 * Modified By: Nicholas Klvana-Hooper
 * -----
 * Purpose: Contains methods for a circular queue implementation
 * Reference: 
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <string.h> 
#include <sys/mman.h>
#include <fcntl.h> 
#include "circularQueue.h"

/*
 * SUBMODULE: createCircularQueue
 * IMPORT: size(int)
 * EXPORT: void
 * ASSERTION: Creates the shared memory for and initialises the queue
 * REFERENCE: 
 */
void createCircularQueue(int size)
{
    int buff_fd, data_fd, ent_fd, i;
    CircularQueue* queue;
    Entry* temp;
    char num[3];

    /* Open buffer shared memory and map it */
    buff_fd = shm_open("/BUFFER", O_CREAT | O_RDWR, 0666); 
    ftruncate(buff_fd, sizeof(CircularQueue)); 
    queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ | PROT_WRITE, MAP_SHARED, buff_fd, 0);

    /* Initialise the queue */
    queue->max = size;
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    queue->done = 0;

    /* Open data shared memory and map it */
    data_fd = shm_open("/DATA", O_CREAT | O_RDWR, 0666); 
    ftruncate(data_fd, size * sizeof(Entry)); 
    queue->data = (Entry*) mmap(0, size * sizeof(Entry), PROT_READ | PROT_WRITE, MAP_SHARED, data_fd, 0);
    
    for(i = 0; i < size; i++)
    {
        /* has name "/<num>" ie. "/0", "/1" etc. */
        sprintf(num, "/%d", i);

        /* Open entry shared memory and map it */
        ent_fd = shm_open(num, O_CREAT | O_RDWR, 0666); 
        ftruncate(ent_fd, sizeof(Entry)); 
        temp = (Entry*) mmap(0, sizeof(Entry), PROT_READ | PROT_WRITE, MAP_SHARED, ent_fd, 0);

        /* initialise the blank entry */
        temp->start = 0;
        temp->dest = 0;

        /* unmap the temp shared memory from this function */
        munmap(temp, sizeof(Entry));
    }
    
    /* unmap the queue shared memory from this function */
    munmap(queue, sizeof(CircularQueue));
}

/*
 * SUBMODULE: isEmpty
 * IMPORT: void
 * EXPORT: bool(int)
 * ASSERTION: Returns an int representing a boolean saying if the queue is empty
 * REFERENCE: 
 */
int isEmpty()
{
    int buff_fd;
    CircularQueue* queue;
    int bool = 0;

    /* Open buffer shared memory and map it */
    buff_fd = shm_open("/BUFFER", O_RDONLY, 0666);
    queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, buff_fd, 0); 

    /* if queue is empty, count = 0 */
    if(queue->count == 0)
    {
        bool = 1;
    }

    /* unmap the queue shared memory from this function */
    munmap(queue, sizeof(CircularQueue));
    return bool;
}

/*
 * SUBMODULE: isFull
 * IMPORT: queue(CircularQueue*)
 * EXPORT: bool(int)
 * ASSERTION: Returns an int representing a boolean saying if the queue is full
 * REFERENCE: 
 */
int isFull(CircularQueue* queue)
{
    int bool = 0;

    if(queue->count == queue->max)
    {
        bool = 1;
    }
    return bool;
}

/*
 * SUBMODULE: enqueue
 * IMPORT: start(int), dest(int)
 * EXPORT: void
 * ASSERTION: Adds a new entry object into the queue
 * REFERENCE: 
 */
void enqueue(int start, int dest)
{
    int entry_fd, buff_fd;
    Entry* ent;
    char num[3]; 
    CircularQueue* queue;

    /* Open buffer shared memory and map it */
    buff_fd = shm_open("/BUFFER", O_CREAT | O_RDWR, 0666); 
    queue = (CircularQueue*) mmap(0, sizeof(CircularQueue),  PROT_READ | PROT_WRITE, MAP_SHARED, buff_fd, 0);

    if(isFull(queue))
    {
        printf("Error: Queue is full!\n");
    }
    else
    {
        /* has name "/<num>" ie. "/0", "/1" etc. */
        sprintf(num, "/%d", queue->tail);

        /* Open/create array shared memory and map it */
        entry_fd = shm_open(num, O_CREAT | O_RDWR, 0666); 
        ftruncate(entry_fd, sizeof(Entry)); 
        ent = (Entry*) mmap(0, sizeof(Entry),  PROT_READ | PROT_WRITE, MAP_SHARED, entry_fd, 0);
        
        /* Set start and dest of new entry */
        ent->start = start;
        ent->dest = dest;

        /* perform circular queue function */
        queue->data[queue->tail] = *ent; 
        queue->tail = (queue->tail + 1) % queue->max;
        queue->count += 1;
    }

    /* unmap the queue shared memory from this function */
    munmap(queue, sizeof(CircularQueue));
}

/*
 * SUBMODULE: dequeue
 * IMPORT: void
 * EXPORT: ent(Entry*)
 * ASSERTION: Removes the next entry object from the queue
 * REFERENCE: 
 */
Entry* dequeue()
{
    int buff_fd;
    CircularQueue* queue;
    Entry* ent;
    Entry* blank;

    /* Open buffer shared memory and map it */
    buff_fd = shm_open("/BUFFER", O_CREAT | O_RDWR, 0666); 
    ftruncate(buff_fd, sizeof(CircularQueue));
    queue = (CircularQueue*) mmap(0, sizeof(CircularQueue),  PROT_READ | PROT_WRITE, MAP_SHARED, buff_fd, 0);

    /* create a blank entry to replace the deqeueu-d one */
    blank = (Entry*)malloc(sizeof(Entry));
    blank->start = 0;
    blank->dest = 0;

    /* get next entry from queue, then set head to next index */
    ent = peek();
    queue->data[queue->head] = *blank;
    queue->head = (queue->head + 1) % queue->max;

    /* count is one less */
    queue->count -= 1;

    free(blank);

    /* unmap the queue shared memory from this function */
    munmap(queue, sizeof(CircularQueue));
    return ent;
}

/*
 * SUBMODULE: peek
 * IMPORT: void
 * EXPORT: ent (Entry *)
 * ASSERTION: Take a copy of the next queue in the queue
 * REFERENCE: 
 */
Entry* peek()
{
    int buff_fd;
    Entry* ent;
    CircularQueue* queue;

    /* Open buffer shared memory and map it */
    buff_fd = shm_open("/BUFFER", O_RDONLY, 0666);
    queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, buff_fd, 0); 

    ent = (Entry*)malloc(sizeof(Entry));
    
    if(isEmpty() == 1)
    {
        printf("Error: Queue is empty!\n");
    }
    else
    {
        /* get next entry from the queue */
        *ent = queue->data[queue->head];
    }

    /* unmap the queue shared memory from this function */
    munmap(queue, sizeof(CircularQueue));
    return ent;
}

/*
 * SUBMODULE: freeQueue
 * IMPORT: void
 * EXPORT: void
 * ASSERTION: Removes all of the shared memory of the queue
 * REFERENCE: 
 */
void freeQueue()
{
    int buff_fd, data_fd, i;
    CircularQueue* queue;
    char num[3];

    /* Open buffer shared memory and map it */
    buff_fd = shm_open("/BUFFER", O_RDONLY, 0666);
    queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, buff_fd, 0); 

    for (i = 0; i < queue->max; i++)
    {
        /* has name "/<num>" ie. "/0", "/1" etc. */
        sprintf(num, "/%d", i);

        /* unmap each entry shared memory from this function as well as closing and */
        /* removing it completely */
        data_fd = shm_open(num, O_RDONLY, 0666);
        close(data_fd);
        shm_unlink(num);
    }

    /* unmap the data shared memory from this function as well as closing and */
    /* removing it completely */
    data_fd = shm_open("/DATA", O_RDONLY, 0666);
    close(data_fd);
    shm_unlink("/DATA");

    /* unmap the queue shared memory from this function as well as closing and */
    /* removing it completely */
    munmap(queue, sizeof(CircularQueue));
    close(buff_fd);
    shm_unlink("/BUFFER");
}

/*
 * SUBMODULE: isDone
 * IMPORT: void
 * EXPORT: void
 * ASSERTION: Returns an int representing a boolean of whether the queue is done inputting
 * REFERENCE: 
 */
int isDone()
{
    int buff_fd, bool;
    CircularQueue* queue;

    /* Open buffer shared memory and map it */
    buff_fd = shm_open("/BUFFER", O_RDONLY, 0666);
    queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, buff_fd, 0); 

    /* gets value of done */
    bool = queue->done;

    /* unmap the queue shared memory from this function */
    munmap(queue, sizeof(CircularQueue));
    return bool;
}

/*
 * SUBMODULE: setDone
 * IMPORT: void
 * EXPORT: void
 * ASSERTION: Sets the done int within the queue
 * REFERENCE: 
 */
void setDone()
{
    int buff_fd;
    CircularQueue * queue;

    /* Open buffer shared memory and map it */
    buff_fd = shm_open("/BUFFER", O_CREAT | O_RDWR, 0666); 
    ftruncate(buff_fd, sizeof(CircularQueue));
    queue = (CircularQueue*) mmap(0, sizeof(CircularQueue),  PROT_READ | PROT_WRITE, MAP_SHARED, buff_fd, 0);

    /* sets done within the queue */
    queue->done = 1;

    /* unmap the queue shared memory from this function */
    munmap(queue, sizeof(CircularQueue));
}
