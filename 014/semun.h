#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/sem.h>

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};