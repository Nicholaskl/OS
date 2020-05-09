#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

typedef struct{
    int start;
    int dest;
} entry;

typedef struct{
    int max;
    int head;
    int tail;
    int count;
    int done;
    entry* data;
} CircularQueue;

CircularQueue* createCircularQueue(int size);
int getCount(CircularQueue* queue);
int isEmpty(CircularQueue* queue);
int isFull(CircularQueue* queue);
void enqueue(CircularQueue* queue, int source, int dest);
entry* dequeue(CircularQueue* queue);
entry* peek(CircularQueue* queue);
void freeQueue();
void printQueue(CircularQueue* queue);
void writeQueue(CircularQueue* queue, FILE* output);
void writeEntry(entry* ent, FILE* output);
int isDone(CircularQueue* queue);
void setDone(CircularQueue* queue);

#endif
