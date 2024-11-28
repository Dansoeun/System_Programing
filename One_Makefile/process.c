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

static int pipe_num=0;

//extern void Copy(int ac, char *av[]);

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
pid_t Get_Branch(process list[100], char target[512])
{
    for(int i=0; i<100; i++)
    {
        if (strcmp(list[i].branch,target)==0)
        {
            //return list[i];
            return list[i].pid;
        }
    }

    printf("No have branch ! \n");
    exit(EXIT_FAILURE);
}


char * Make_Pipe(process target)
{
    //int pipefd[2] = {0};
    //char buffer[1024] = {'\0'};
    char *pipe_name = malloc(1024 * sizeof(char));  // 동적 메모리 할당

    if (pipe_name == NULL) {
        perror("malloc failed");
        exit(1);
    }

    sprintf(pipe_name, "myPipe%d", pipe_num);

    if ((mkfifo(pipe_name, 0666)) == -1) {
        perror("make pipe error");
        free(pipe_name);  // 메모리 해제
        exit(1);
    }

    return pipe_name;  // 동적 메모리 주소 반환
}


char* Get_FIFO_name(process list[100], pid_t target)
{
    for(int i=0; i<100; i++)
    {
        if (list[i].pid==target)
        {
            return list[i].fifo_file_name;
        }
    }

    return NULL;
}