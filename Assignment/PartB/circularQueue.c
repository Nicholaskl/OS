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

    void createCircularQueue(int size)
    {
        int shm_fd1, shm_fd2, shm_fd3, i;
        CircularQueue* queue;
        Entry* temp;
        char num[3];

        shm_fd1 = shm_open("/BUFFER", O_CREAT | O_RDWR, 0666); 
        ftruncate(shm_fd1, sizeof(CircularQueue)); 
        queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);

        queue->max = size;
        queue->head = 0;
        queue->tail = 0;
        queue->count = 0;
        queue->done = 0;
        shm_fd2 = shm_open("/DATA", O_CREAT | O_RDWR, 0666); 
        ftruncate(shm_fd2, size * sizeof(Entry)); 
        queue->data = (Entry*) mmap(0, size * sizeof(Entry), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
        
        for(i = 0; i < size; i++)
        {
            sprintf(num, "/%d", i);
            shm_fd3 = shm_open(num, O_CREAT | O_RDWR, 0666); 
            ftruncate(shm_fd3, sizeof(Entry)); 

            temp = (Entry*) mmap(0, sizeof(Entry), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd3, 0);
            temp->start = 0;
            temp->dest = 0;

            munmap(temp, sizeof(Entry));
        }
        
        munmap(queue, sizeof(CircularQueue));
    }

    int getCount(CircularQueue* queue)
    {
        return queue->count;
    }

    int isEmpty()
    {
        int shm_fd1;
        CircularQueue* queue;
        int bool = 0;

        shm_fd1 = shm_open("/BUFFER", O_RDONLY, 0666);
        queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, shm_fd1, 0); 

        if(queue->count == 0)
        {
            bool = 1;
        }

        munmap(queue, sizeof(CircularQueue));
        
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
        Entry* ent;
        char num[3]; 
        CircularQueue* queue;

        shm_fd2 = shm_open("/BUFFER", O_CREAT | O_RDWR, 0666); 
        queue = (CircularQueue*) mmap(0, sizeof(CircularQueue),  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

        if(isFull(queue)==1)
        {
            printf("Error: Queue is full!\n");
        }
        else
        {
            sprintf(num, "/%d", queue->tail);
            shm_fd1 = shm_open(num, O_CREAT | O_RDWR, 0666); 
            ftruncate(shm_fd1, sizeof(Entry)); 
            ent = (Entry*) mmap(0, sizeof(Entry),  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
            
            ent->start = source;
            ent->dest = dest;

            queue->data[queue->tail] = *ent; 
            queue->tail = (queue->tail + 1) % queue->max;
            queue->count += 1;
        }
        munmap(queue, sizeof(CircularQueue));
    }

    Entry* dequeue()
    {
        int shm_fd1;
        CircularQueue* queue;
        Entry* ent;
        Entry* blank;

        shm_fd1 = shm_open("/BUFFER", O_CREAT | O_RDWR, 0666); 
        ftruncate(shm_fd1, sizeof(CircularQueue));
        queue = (CircularQueue*) mmap(0, sizeof(CircularQueue),  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);

        blank = (Entry*)malloc(sizeof(Entry));
        blank->start = 0;
        blank->dest = 0;

        ent = peek();
        queue->data[queue->head] = *blank;
        queue->head = (queue->head + 1) % queue->max;

        queue->count -= 1;

        free(blank);

        munmap(queue, sizeof(CircularQueue));
        return ent;
    }

    Entry* peek()
    {
        int shm_fd1;
        Entry* ent;
        CircularQueue* queue;

        shm_fd1 = shm_open("/BUFFER", O_RDONLY, 0666);
        queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, shm_fd1, 0); 

        ent = (Entry*)malloc(sizeof(Entry));
        
        if(isEmpty() == 1)
        {
            printf("Error: Queue is empty!\n");
        }
        else
        {
            *ent = queue->data[queue->head];
        }

        munmap(queue, sizeof(CircularQueue));
        return ent;
    }

    void freeQueue()
    {
        int shm_fd1, shm_fd2, i;
        CircularQueue* buffer;
        char num[3];

        shm_fd1 = shm_open("/BUFFER", O_RDONLY, 0666);
        buffer = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, shm_fd1, 0); 

        for (i = 0; i < buffer->max; i++)
        {
            sprintf(num, "/%d", i);
            shm_fd2 = shm_open(num, O_RDONLY, 0666);
            close(shm_fd2);
            shm_unlink(num);
        }

        shm_fd2 = shm_open("/DATA", O_RDONLY, 0666);
        close(shm_fd2);
        shm_unlink("/DATA");

        munmap(buffer, sizeof(CircularQueue));
        close(shm_fd1);
        shm_unlink("/BUFFER");
    }

    void printQueue()
    {
        int shm_fd2, shm_fd3;
        char num[3];
        int i = 0;
        Entry* ent = NULL;
        CircularQueue* buffer;

        shm_fd3 = shm_open("/BUFFER", O_RDONLY, 0666);
        buffer = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, shm_fd3, 0); 

        while((isEmpty() != 1) && (i < buffer->count))
        {
            sprintf(num, "/%d", (buffer->head + i) % buffer->max);
            shm_fd2 = shm_open(num, O_RDONLY, 0666);
            ent = (Entry*) mmap(0, sizeof(Entry), PROT_READ, MAP_SHARED, shm_fd2, 0); 
            printf("(%d, %d) ", ent->start, ent->dest);
            i++;
            munmap(ent, sizeof(Entry));
        }
        printf("\n");
        munmap(buffer, sizeof(CircularQueue));
    }

    void writeQueue(CircularQueue* queue, FILE* output)
    {
        int i = 0;
        Entry* ent = NULL;
        
        while((isEmpty() != 1) && (i < queue->count))
        {
            ent = &(queue->data[(queue->head + i) % queue->max]);
            fprintf(output, "%d %d\n", ent->start, ent->dest);
            i++;
        }
    }

    void writeEntry(Entry* ent, FILE* output)
    {
        fprintf(output, "%d %d\n", ent->start, ent->dest);
    }

    int isDone()
    {
        int shm_fd1, output;
        CircularQueue* queue;

        shm_fd1 = shm_open("/BUFFER", O_RDONLY, 0666);
        queue = (CircularQueue*) mmap(0, sizeof(CircularQueue), PROT_READ, MAP_SHARED, shm_fd1, 0); 
        output = queue->done;

        munmap(queue, sizeof(CircularQueue));
        return output;
    }

    void setDone()
    {
        int shm_fd1;
        CircularQueue * queue;

        shm_fd1 = shm_open("/BUFFER", O_CREAT | O_RDWR, 0666); 
        ftruncate(shm_fd1, sizeof(CircularQueue));
        queue = (CircularQueue*) mmap(0, sizeof(CircularQueue),  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);

        queue->done = 1;

        munmap(queue, sizeof(CircularQueue));
    }
