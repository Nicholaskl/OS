#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  /*Header file for sleep(). man 3 sleep for details. */
#include <sys/wait.h>
#include <pthread.h>
#include <string.h> 
#include <sys/mman.h>
#include <sys/shm.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include "circularQueue.h"
#include "lift_sim_B.h"

FILE* output;
pthread_mutex_t lock, fileLock;
pthread_cond_t full, empty;
int sleepT;
const int SIZE = sizeof(CircularQueue);
const char* NAME = "Buffer";

int main(int argc, char* argv[])
{
    pid_t rid, l1, l2, l3;

    output = fopen("sim_output", "a");
    
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&fileLock, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);

    rid = fork();

    if(!rid)
    {
        request(argv);
    }
    else
    {
        sleep(3);
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
    
    
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&fileLock);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    fclose(output);

    return 0;
}

void request(char* argv[])
{
    int source, dest;
    FILE* input = fopen("sim_input", "r");

    createCircularQueue(atoi(argv[1]));

    while(!feof(input))
    {
        fscanf(input, "%d %d", &source, &dest);
        enqueue(source, dest);
    }

    printQueue();

    printf("Lift requester-> pid: %d and ppid: %d\n", getpid(), getppid());
}

void lift(void)
{
    printQueue();

    if(abs(getpid()-getppid()) == 4)
    {
        shm_unlink(NAME);
    }

    printf("I am lift-%d-> pid: %d and ppid: %d\n",getpid()-getppid()-1, getpid(), getppid());
}
