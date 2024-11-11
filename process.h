#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>

typedef struct Process
{
    pid_t pid;
    char branch[512];
}process;


void kills();
void died(pid_t pid);
process Get_Branch(process list[100], char target[512]);

#endif