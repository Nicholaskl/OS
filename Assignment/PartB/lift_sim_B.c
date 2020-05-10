#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  /*Header file for sleep(). man 3 sleep for details. */
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h> 
#include <sys/mman.h>
#include <sys/shm.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include "circularQueue.h"
#include "lift_sim_B.h"

int sleepT;

int main(int argc, char* argv[])
{
    pid_t rid, l1, l2, l3;
    sem_t* full;
    sem_t* empty;
    sem_t* lock;
    createCircularQueue(atoi(argv[1]));
    full = (sem_t *) sem_open("/full_sem", O_CREAT | O_EXCL, 0644, 0);
    lock = (sem_t *) sem_open("/lock_sem", O_CREAT | O_EXCL, 0644, 1);
    empty = (sem_t *) sem_open("/empty_sem", O_CREAT | O_EXCL, 0644, atoi(argv[1]));

    rid = fork();

    if(!rid)
    {
        request(argv);
    }
    else
    {
        l1 = fork();
        if(!l1)
        {
            lift();
        }
        else
        {
            l2 = fork();
            if(!l2)
            {
                lift();
            }
            else
            {
                l3 = fork();
                if(!l3)
                {
                    lift();
                }
                else
                {
                    wait(NULL);
                    wait(NULL);
                    wait(NULL);
                    wait(NULL);
                    freeQueue();
                    printf("I'm the parent-> pid: %d\n", getpid());
                }
            }
        }
    }

    sem_unlink("/full_sem");
    sem_unlink("/empty_sem");
    sem_unlink("/lock_sem");

    return 0;
}

void request(char* argv[])
{
    int source, dest;
    FILE* input = fopen("sim_input", "r");
    sem_t* full;
    sem_t* lock;
    sem_t* empty;

    full = (sem_t *) sem_open("/full_sem", 0);
    lock = (sem_t *) sem_open("/lock_sem", 0);
    empty = (sem_t *) sem_open("/empty_sem", 0);

    while(!feof(input))
    {
        fscanf(input, "%d %d", &source, &dest);

        sem_wait(empty);
        sem_wait(lock);

        enqueue(source, dest);
        printf("Enqueued: %d, %d\n", source, dest);

        sem_post(lock);
        sem_post(full);
    }
    sem_wait(lock);
    setDone();
    sem_post(lock);

    sem_close(full);
    sem_close(empty);
    sem_close(lock);

    fclose(input);

    printf("Lift requester-> pid: %d and ppid: %d\n", getpid(), getppid());
}

void lift(void)
{
    entry* ent;
    sem_t* full;
    sem_t* lock;
    sem_t* empty;
    int done;


    full = (sem_t *) sem_open("/full_sem", 0);
    lock = (sem_t *) sem_open("/lock_sem", 0);
    empty = (sem_t *) sem_open("/empty_sem", 0);

        sem_wait(full);
        sem_wait(lock);

        if(!isEmpty())
        {
            ent = dequeue();
            printf("Done: %d %d\n", ent->start, ent->dest);
            free(ent);
        }
 
        printQueue();
        printf("IS DONE? %d\n", isEmpty());
        printf("IS DONE??? %d\n", isDone());

        if(isDone() && isEmpty())
        {
            printf("ghetto\n");
            done = 1;
        }

        sem_post(lock);
        sem_post(empty);


        /*sem_wait(lock);
        if(isDone() && isEmpty())
        {
            done = 1;
        }
        sem_post(lock);*/
    

    sem_close(full);
    sem_close(empty);
    sem_close(lock);

    printf("I am lift-%d-> pid: %d and ppid: %d\n",getpid()-getppid()-1, getpid(), getppid());
}
