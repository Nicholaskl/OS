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
#include "circularQueue.h"

/*
 * SUBMODULE: createCircularQueue
 * IMPORT: size(int)
 * EXPORT: queue (CircularQueue*)
 * ASSERTION: Creates and initialises the queue
 * REFERENCE: 
 */
CircularQueue* createCircularQueue(int size)
{
    CircularQueue* queue;

    /* creates memory for and initialises the queue */
    queue = (CircularQueue*)malloc(sizeof(CircularQueue));
    queue->max = size;
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    queue->done = 0;
    queue->data = (Entry*)malloc(size * sizeof(Entry));

    return queue;
}

/*
 * SUBMODULE: isEmpty
 * IMPORT: queue (CircularQueue*)
 * EXPORT: bool(int)
 * ASSERTION: Returns an int representing a boolean saying if the queue is empty
 * REFERENCE: 
 */
int isEmpty(CircularQueue* queue)
{
    int bool = 0;

    if(queue->count == 0)
    {
        bool = 1;
    }
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
 * IMPORT: queue(CircularQueue*), ent(Entry)
 * EXPORT: void
 * ASSERTION: Adds a new Entry object into the queue
 * REFERENCE: 
 */
void enqueue(CircularQueue* queue, Entry ent)
{
    if(isFull(queue)==1)
    {
        printf("Error: Queue is full!\n");
    }
    else
    {
        /* perform the increment for circular queue tail */
        queue->data[queue->tail] = ent; 
        queue->tail = (queue->tail + 1) % queue->max;
        queue->count += 1;
    }
}

/*
 * SUBMODULE: dequeue
 * IMPORT: queue(CircularQueue*)
 * EXPORT: ent(Entry*)
 * ASSERTION: Removes the next entry object from the queue
 * REFERENCE: 
 */
Entry* dequeue(CircularQueue* queue)
{
    Entry* ent;
    Entry* blank;

    blank = (Entry*)malloc(sizeof(Entry));

    /* Create copy of next entry in queue */
    ent = peek(queue);

    /* Reduce count and head */
    queue->data[queue->head] = *blank;
    queue->head = (queue->head + 1) % queue->max;
    queue->count -= 1;

    free(blank);
    return ent;
}

/*
 * SUBMODULE: peek
 * IMPORT: queue(CircularQueue *)
 * EXPORT: ent (Entry*)
 * ASSERTION: Take a copy of the next queue in the queue
 * REFERENCE: 
 */
Entry* peek(CircularQueue* queue)
{
    Entry* ent;

    ent = (Entry*)malloc(sizeof(Entry));
    
    if(isEmpty(queue) == 1)
    {
        printf("Error: Queue is empty!\n");
    }
    else
    {
        *ent = queue->data[queue->head];
    }
    return ent;
}

/*
 * SUBMODULE: freeQueue
 * IMPORT: queue(CircularQueue*)
 * EXPORT: void
 * ASSERTION: Frees all the segements and the queue itself
 * REFERENCE: 
 */
void freeQueue(CircularQueue* queue)
{
    free(queue->data);
    free(queue);
}

/*
 * SUBMODULE: isDone
 * IMPORT: queue(CircularQueue*)
 * EXPORT: int
 * ASSERTION: Returns an int representing a boolean of whether the queue is done inputting
 * REFERENCE: 
 */
int isDone(CircularQueue* queue)
{
    return queue->done;
}

/*
 * SUBMODULE: setDone
 * IMPORT: queue(CircularQueue*)
 * EXPORT: void
 * ASSERTION: Sets the done int within the queue
 * REFERENCE: 
 */
void setDone(CircularQueue* queue)
{
    queue->done = 1;
}
