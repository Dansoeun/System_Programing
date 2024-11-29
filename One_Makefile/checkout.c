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

//message received, from exisiting branch  .. 
void sig_to_branch(char branchname[], int signum,pid_t branch_pid) 
{
    printf("Request to branch .. ");
    kill(signum,branch_pid);
}

//av[0]: shmid, share memory identifier, 
//av[1]: key_t(int), message queue identifier, 
//av[2]: checkout file number (global var)
//av[3]: branch name for checkout
//av[4]: process list
void checkout(int ac, char *av[])
{
    int shmid=0;
    int msgid=0;
    void *shmaddr;
    shmid=shmget(av[0],NULL,0); //read and write permission
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
    exisiting_branch=Get_Branch(av[4],av[3]);
    sig_to_branch(av[3],SIGUSR2,exisiting_branch);

    //message queue
    msgid=msgget(av[1],0666); // 0 -> existing message queue return
    len=msgrcv(msgid,&mkfifoname,512,0,0); //message received, from exisiting branch  .. 

    sprintf(checkoutfile,"checkoutfile_%d.c",av[2]); //checkout content file name 

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
        //now (branch) execvp, copy with checkout branch file 
        execvp(mkfifoname,checkoutfile); //first parameter file name check plz
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
        //message queue push code 
        while ((n_char=read(checkoutfile,&buf,2048))>0)
        {
            if (msgsnd(av[1], &buf,2048,IPC_NOWAIT)==-1) // msgsend to file content 
            {
                perror("msgsnd");
                exit(1);
            }
        }
    }

    return;
}