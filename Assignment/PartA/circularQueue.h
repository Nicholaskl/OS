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

CircularQueue* createCircularQueue(int size);
int isEmpty(CircularQueue* queue);
int isFull(CircularQueue* queue);
void enqueue(CircularQueue* queue, Entry ent);
Entry* dequeue(CircularQueue* queue);
Entry* peek(CircularQueue* queue);
void freeQueue(CircularQueue* queue);
int isDone(CircularQueue* queue);
void setDone(CircularQueue* queue);

#endif
