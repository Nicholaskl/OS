#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  /*Header file for sleep(). man 3 sleep for details. */
#include <pthread.h>
#include <string.h> 
#include <sys/mman.h>
#include <sys/shm.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include "circularQueue.h"

    CircularQueue* createCircularQueue(int size)
    {
        int shm_fd1, shm_fd2;
        CircularQueue* queue;

        shm_fd1 = shm_open("BUFFER", O_CREAT | O_RDWR, 0666); 
        ftruncate(shm_fd1, sizeof(CircularQueue)); 
        queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);

        queue->max = size;
        queue->head = 0;
        queue->tail = 0;
        queue->count = 0;
        queue->done = 0;
        shm_fd2 = shm_open("DATA", O_CREAT | O_RDWR, 0666); 
        ftruncate(shm_fd2, size * sizeof(entry)); 
        queue->data = (entry*) mmap(0, size * sizeof(entry), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
        shmdt(queue->data);
        shmdt(queue);

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

    void enqueue(int source, int dest)
    {
        int shm_fd1, shm_fd2;
        entry* ent;
        char num[2]; 
        CircularQueue* queue;

        shm_fd2 = shm_open("BUFFER", O_CREAT | O_RDWR, 0666); 
        ftruncate(shm_fd2, sizeof(CircularQueue)); 
        queue = (CircularQueue*) mmap(0, sizeof(CircularQueue),  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

        if(isFull(queue)==1)
        {
            printf("Error: Queue is full!\n");
        }
        else
        {
            sprintf(num, "%d", queue->tail);
            shm_fd1 = shm_open(num, O_CREAT | O_RDWR, 0666); 
            ftruncate(shm_fd1, sizeof(entry)); 
            ent = (entry*) mmap(0, sizeof(entry),  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
            
            ent->start = source;
            ent->dest = dest;

            queue->data[queue->tail] = *ent; 
            queue->tail = (queue->tail + 1) % queue->max;
            queue->count += 1;
            shmdt(ent);
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

    void freeQueue()
    {
        int shm_fd1, shm_fd2, i;
        CircularQueue* buffer;
        char num[2];

        shm_fd1 = shm_open("BUFFER", O_RDONLY, 0666);
        buffer = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, shm_fd1, 0); 

        for (i = 0; i < buffer->max; i++)
        {
            sprintf(num, "%d", (buffer->head + i) % buffer->max);
            shm_fd2 = shm_open(num, O_RDONLY, 0666);
            close(shm_fd2);
            shm_unlink(num);
        }

        shm_fd2 = shm_open("DATA", O_RDONLY, 0666);
        close(shm_fd2);
        shm_unlink("DATA");

        close(shm_fd1);
        shm_unlink("BUFFER");
    }

    void printQueue()
    {
        int shm_fd2, shm_fd3;
        char num[2];
        int i = 0;
        entry* ent = NULL;
        CircularQueue* buffer;

        shm_fd3 = shm_open("BUFFER", O_RDONLY, 0666);
        buffer = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, shm_fd3, 0); 

        while((isEmpty(buffer) != 1) && (i < buffer->count))
        {
            sprintf(num, "%d", (buffer->head + i) % buffer->max);
            shm_fd2 = shm_open(num, O_RDONLY, 0666);
            ent = (entry*) mmap(0, sizeof(entry), PROT_READ, MAP_SHARED, shm_fd2, 0); 
            printf("(%d, %d) ", ent->start, ent->dest);
            i++;
        }
        printf("\n");
        munmap(buffer, sizeof(CircularQueue));
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

    int isDone(CircularQueue* queue)
    {
        return queue->done;
    }

    void setDone(CircularQueue* queue)
    {
        queue->done = 1;
    }
