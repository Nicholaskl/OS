#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 

int main (void)
{
    return 0;
}

void lift(void)
{
    printf("Hello, this is my thread ID\n");
}

void request(void)
{
}