#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "process.h"
//#include "cp.h"

extern void Copy(int ac, char *av[]);

// 좀비 프로세스 처리 
void kills()
{
    int status;
    waitpid(-1,&status,WNOHANG);
    exit(0);
}

//SIGINT (시그널) 처리 
void died(pid_t pid)
{
    printf("프로세스 강제 종료\n");
    kill(pid, 9);
}

// 해당하는 [브랜치명(경로)] 있는 process 탐색 
process Get_Branch(process list[100], char target[512])
{
    for(int i=0; i<100; i++)
    {
        if (strcmp(list[i].branch,target)==0)
        {
            return list[i];
        }
    }

    printf("No have branch ! \n");
    exit(EXIT_FAILURE);
}