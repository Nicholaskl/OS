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

int ftruncate(int fd, off_t length);

CircularQueue* createCircularQueue(int size);
int getCount(CircularQueue* queue);
int isEmpty();
int isFull(CircularQueue* queue);
void enqueue(int source, int dest);
entry* dequeue();
entry* peek();
void freeQueue();
void printQueue();
void writeQueue(CircularQueue* queue, FILE* output);
void writeEntry(entry* ent, FILE* output);
int isDone();
void setDone();

#endif
