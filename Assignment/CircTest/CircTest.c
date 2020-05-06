#include <stdio.h>
#include <stdlib.h>
#include "circularQueue.h"

int main()
{
    CircularQueue* buffer = createCircularQueue(2);
    entry *ent1, *ent2, *ent3, *ent4;

    ent1 = (entry*)malloc(sizeof(entry));
    ent2 = (entry*)malloc(sizeof(entry));
    ent3 = (entry*)malloc(sizeof(entry));
    ent4 = (entry*)malloc(sizeof(entry));

    ent1->start = 1;
    ent1->dest = 2;
    ent1->start = 2;
    ent1->dest = 3;
    ent1->start = 3;
    ent1->dest = 4;
    ent1->start = 4;
    ent1->dest = 5;

    enqueue(buffer, *ent1);
    enqueue(buffer, *ent2);
    free(dequeue(buffer));
    free(dequeue(buffer));
    enqueue(buffer, *ent3);
    enqueue(buffer, *ent4);
    free(dequeue(buffer));
    free(dequeue(buffer));

    free(ent1);
    free(ent2);
    free(ent3);
    free(ent4);
    freeQueue(buffer);

    return 0;
}
