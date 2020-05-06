#include <stdio.h>
#include <stdlib.h>
#include "circularQueue.h"

    CircularQueue* createCircularQueue(int size)
    {
        CircularQueue* queue;

        queue = (CircularQueue*)malloc(sizeof(CircularQueue));
        queue->max = size;
        queue->head = 0;
        queue->tail = 0;
        queue->count = 0;
        queue->data = (entry*)malloc(size * sizeof(entry));

        return queue;
    }

    int getCount(CircularQueue* queue)
    {
        return queue->count;
    }

    int isEmpty(CircularQueue* queue)
    {
        int bool = 0;

        if(queue->count == 0)
        {
            bool = 1;
        }
        return bool;
    }

    int isFull(CircularQueue* queue)
    {
        int bool = 0;

        if(queue->count == queue->max)
        {
            bool = 1;
        }
        return bool;
    }

    void enqueue(CircularQueue* queue, entry ent)
    {
        if(isFull(queue)==1)
        {
            printf("Error: Queue is full!\n");
        }
        else
        {
            queue->data[queue->tail] = ent; 
            queue->tail = (queue->tail + 1) % queue->max;
            queue->count += 1;
        }
    }

    entry* dequeue(CircularQueue* queue)
    {
        entry* ent;
        entry* blank;

        blank = (entry*)malloc(sizeof(entry));

        ent = peek(queue);
        queue->data[queue->head] = *blank;
        queue->head = (queue->head + 1) % queue->max;

        queue->count -= 1;

        free(blank);

        return ent;
    }

    entry* peek(CircularQueue* queue)
    {
        entry* ent;

        ent = (entry*)malloc(sizeof(entry));
        
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

    void freeQueue(CircularQueue* queue)
    {
        free(queue->data);
        free(queue);
    }

    void printQueue(CircularQueue* queue)
    {
        int i = 0;
        entry* ent = NULL;

        while((isEmpty(queue) != 1) && (i < queue->count))
        {
            ent = &(queue->data[(queue->head + i) % queue->max]);
            printf("(%d, %d) ", ent->start, ent->dest);
            i++;
        }
        printf("\n");
    }

    void writeQueue(CircularQueue* queue, FILE* output)
    {
        int i = 0;
        entry* ent = NULL;
        
        while((isEmpty(queue) != 1) && (i < queue->count))
        {
            ent = &(queue->data[(queue->head + i) % queue->max]);
            fprintf(output, "%d %d\n", ent->start, ent->dest);
            i++;
        }
    }

    void writeEntry(entry* ent, FILE* output)
    {
        fprintf(output, "%d %d\n", ent->start, ent->dest);
    }
