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
    entry* data;
} CircularQueue;

CircularQueue* createCircularQueue(int size);
int getCount(CircularQueue* queue);
int isEmpty(CircularQueue* queue);
int isFull(CircularQueue* queue);
void enqueue(CircularQueue* queue, entry ent);
entry* dequeue(CircularQueue* queue);
entry* peek(CircularQueue* queue);
void freeQueue(CircularQueue* queue);
void printQueue(CircularQueue* queue);
void writeQueue(CircularQueue* queue, FILE* output);

#endif
