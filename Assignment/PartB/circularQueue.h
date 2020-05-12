/*
 * File: CircularQueue.h
 * File Created: Tuesday, 5th May 2020
 * Author: Nicholas Klvana-Hooper
 * -----
 * Last Modified: Tuesday, 12th May 2020
 * Modified By: Nicholas Klvana-Hooper
 * -----
 * Purpose: Contains header information for CircularQueue.h
 * Reference: 
 */
#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

/* Entry struct definition */
typedef struct{
    int start;
    int dest;
} Entry;

/* CircularQueue struct definition */
typedef struct{
    int max;
    int head;
    int tail;
    int count;
    int done;
    Entry* data;
} CircularQueue;

/* Didn't work so I have prototyped this function */
int ftruncate(int fd, off_t length);

void createCircularQueue(int size);
int isEmpty();
int isFull(CircularQueue* queue);
void enqueue(int source, int dest);
Entry* dequeue();
Entry* peek();
void freeQueue();
void printQueue();
int isDone();
void setDone();

#endif
