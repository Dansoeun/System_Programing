#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include "cp.h"
#include "process.h"
#include "global.h"

#define SHM_SIZE 4096

//checkout시,  branch에 있는 파일 생성 
//
//message received, from exisiting branch  .. 
void sig_to_branch(char branchname[], int signum,pid_t branch_pid) 
{
    printf("Request to branch .. ");
    kill(signum,branch_pid);
}

//git checkout [branch name]
void checkout(int ac, char *av[])
{
    key_t repo_key = repo_key;
    int shmid = shmget(repo_key, SHM_SIZE, IPC_CREAT | 0644);
    if (shmid == -1) {
        perror("공유 메모리 접근 실패");
        exit(1);
    }

    void *shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *)-1) {
        perror("공유 메모리 연결 실패");
        exit(1);
    }

    printf("공유 메모리 연결 완료. 메모리 주소: %p\n", shmaddr);

    int msgid=msid;
    char mkfifoname[512]={'\0'};
    char checkoutfile[2048]={'\0'};
    char buf[2048]={'\0'};
    int command=-1;
    char ch=' ';
    int pd=0;
    int n_char=0;
    int creat=0;
    int len=0;
    pid_t exisiting_branch=-1;

    /*Branch connection*/
    exisiting_branch = (pid_t)atoi(av[4]);
    sig_to_branch(av[3],SIGUSR2,exisiting_branch);

    //message queue
    len=msgrcv(msgid,&mkfifoname,512,0,0); //message received, from exisiting branch  .. 

    sprintf(checkoutfile,"checkoutfile_%d.c",atoi(av[2])); //checkout content file name 

    Copy(mkfifoname,checkoutfile); //checkoutfile create 

    //1: pull , 2: create branch 
    printf("If you want to pull after checking out, type 1 and 2 if you want to create a branch");

    while ((command!=1) && command!=2)
    {
        scanf("%d%c", &command,&ch);

        if (command==EOF)
        {
            printf("Git Done.");
            exit(EXIT_SUCCESS);
        }
    }

    if (command==1)
    {
        char *argli[] = {checkoutfile, NULL};
        execvp(mkfifoname, argli);

        //now (branch) execvp, copy with checkout branch file 
        //execvp(mkfifoname,checkoutfile); //first parameter file name check plz
    }

    else 
    {
        //checkoutfile message push to branch 
        char file_fifoe[513]={'\0'};
        strncpy(file_fifoe,"./",2);
        strncpy(file_fifoe+2,checkoutfile,strlen(checkoutfile)+1);

        if (mkfifo(file_fifoe,0666)==-1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
        int fd = open(checkoutfile, O_RDONLY);
        if (fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
        }
        //message queue push code 
        while ((n_char=read(fd,&buf,2048))>0)
        {
            if ((msgsnd(atoi(av[1]), &buf, 2048, IPC_NOWAIT) == -1)) // msgsend to file content 
            {
                perror("msgsnd");
                exit(1);
            }
        }
    }

    return;
}