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
    int shm_fd1, shm_fd2, shm_fd3, shm_fd4;
    sem_t* full;
    sem_t* empty;
    sem_t* lock;
    sem_t* fileL;
    createCircularQueue(atoi(argv[1]));

    shm_fd1 = shm_open("/full", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd1, sizeof(sem_t)); 
    full = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);

    shm_fd2 = shm_open("/lock", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd2, sizeof(sem_t)); 
    lock = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

    shm_fd3 = shm_open("/empty", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd3, sizeof(sem_t)); 
    empty = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd3, 0);

    shm_fd4 = shm_open("/fileL", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd4, sizeof(sem_t)); 
    fileL = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd4, 0);

    sem_init(full, 1, 0);
    sem_init(lock, 1, 1);
    sem_init(empty, 1, atoi(argv[1]));
    sem_init(fileL, 1, 1);

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

    return 0;
}

void request(char* argv[])
{
    int source, dest, shm_fd1, shm_fd2, shm_fd3, shm_fd4;
    FILE* input = fopen("sim_input", "r");
    FILE* output;
    sem_t* full;
    sem_t* lock;
    sem_t* empty;
    sem_t* fileL;
    int numReq = 1;
    
    shm_fd1 = shm_open("/full", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd1, sizeof(sem_t)); 
    full = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);

    shm_fd2 = shm_open("/lock", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd2, sizeof(sem_t)); 
    lock = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

    shm_fd3 = shm_open("/empty", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd3, sizeof(sem_t)); 
    empty = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd3, 0);

    shm_fd4 = shm_open("/fileL", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd4, sizeof(sem_t)); 
    fileL = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd4, 0);

    while(!feof(input))
    {
        fscanf(input, "%d %d\n", &source, &dest);

        sem_wait(empty);
        sem_wait(lock);

        enqueue(source, dest);
        printf("Enqueued: %d, %d\n", source, dest);

        sem_post(lock);
        sem_post(full);

        sem_wait(fileL);
        output = fopen("sim_output", "a");
        #ifndef DEBUG_R
        #ifndef DEBUG_L
        fprintf(output, "--------------------------------------------\n");
        fprintf(output, "  New Lift Request From Floor %d to Floor %d\n", source, dest);
        fprintf(output, "  Request No: %d\n", numReq);
        fprintf(output, "--------------------------------------------\n");
        #endif
        #endif

        #ifdef DEBUG_R
        fprintf(output, "%d %d\n", source, dest);
        #endif
        fclose(output);
        sem_post(fileL);

        numReq++;
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
    int shm_fd1, shm_fd2, shm_fd3, shm_fd4, writeNow;
    entry* ent;
    sem_t* full;
    sem_t* lock;
    sem_t* empty;
    sem_t* fileL;
    FILE* output;
    int source=0;
    int dest = 0;
    int done = 0;
    #ifndef DEBUG_R
    #ifndef DEBUG_L
    int prevF = 0;
    int req = 0;
    int totMove = 0;
    #endif
    #endif

    shm_fd1 = shm_open("/full", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd1, sizeof(sem_t)); 
    full = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);

    shm_fd2 = shm_open("/lock", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd2, sizeof(sem_t)); 
    lock = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

    shm_fd3 = shm_open("/empty", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd3, sizeof(sem_t)); 
    empty = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd3, 0);

    shm_fd4 = shm_open("/fileL", O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd4, sizeof(sem_t)); 
    fileL = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd4, 0);

    while(!done)
    {
        writeNow = 0;
        sem_wait(lock);
        if(isEmpty() && !isDone())
        {
            sem_post(lock);
            sem_wait(full);
        }
        else
        {
            sem_post(lock);
        }
        sem_wait(lock);

        if(!isEmpty())
        {
            ent = dequeue();
            source = ent->start;
            dest = ent->dest;
            printf("Done: %d %d\n", source, dest);
            free(ent);
            writeNow = 1;
        }
 
        sem_post(lock);
        sem_post(empty);

        if(writeNow)
        {   
            sem_wait(fileL);
            output = fopen("sim_output", "a");
            #ifndef DEBUG_R
            #ifndef DEBUG_L
            fprintf(output, "Lift-%d Operation\n", getpid()-getppid()-1);
            fprintf(output, "Previous position: Floor %d\n", prevF);
            fprintf(output, "Request: Floor %d to Floor %d\n", source, dest);
            fprintf(output, "Detail operations:\n");
            fprintf(output, "   Go from Floor %d to Floor %d\n", prevF, source);
            fprintf(output, "   Go from Floor %d to Floor %d\n", source, dest);
            fprintf(output, "   #movement for this request: %d\n", abs(dest - source));
            fprintf(output, "   #request: %d\n", req);
            fprintf(output, "   Total #movement: %d\n", totMove + abs(dest - source));
            fprintf(output, "Current Position: Floor %d\n", dest);
            #endif
            #endif
            #ifdef DEBUG_L
            fprintf(output, "%d %d\n", source, dest);
            #endif
            fclose(output);
            sem_post(fileL);
        }

        sem_wait(lock);
        if(isDone() && isEmpty())
        {
            done = 1;
            printf("lift finished\n");
        }
        sem_post(lock);
        
        #ifndef DEBUG_R
        #ifndef DEBUG_L
        prevF = dest;
        totMove += abs(dest-source);
        req++;
        #endif
        #endif
    }
    sem_post(full);
    sem_post(full);

    sem_close(full);
    sem_close(empty);
    sem_close(lock);

    printf("I am lift-%d-> pid: %d and ppid: %d\n",getpid()-getppid()-1, getpid(), getppid());
}
