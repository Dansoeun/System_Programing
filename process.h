#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>

typedef struct Process
{
    pid_t pid;
    char fifo_file_name[512];
}process;


void kills();
void died(pid_t pid);
pid_t Get_Branch(process list[100], char target[512]);
char* Get_FIFO_name(process list[100], pid_t target);

#endif