#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include "cp.h"

//av[0]: shmid, share memory identifier, av[1]: key_t, message queue identifier, 
//av[2]: checkout file number (global var)
void main(int ac, char *av[])
{
    int shmid=0;
    int msgid=0;
    void *shmaddr;
    shmid=shmget(av[0],NULL,0); //read and write permission
    char mkfifoname[512]={'\0'};
    char checkoutfile[2048]={'\0'};
    int command=-1;
    char ch=' ';
    int pd=0;

    int creat=0;
    int len=0;

    msgid=msgget(av[1],0); // 0 -> existing message queue return

    len=msgrcv(msgid,&mkfifoname,512,0,0);

    sprintf(checkoutfile,"checkoutfile_%d.c",av[2]);

    Copy(mkfifoname,checkoutfile);

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
        return; 
    }
    else 
    {
        //checkoutfile message push to branch 
        char file_fifoe[513]={'\0'};
        strncpy(file_fifoe,"./",2);
        strnpy(file_fifoe+2,checkoutfile,strlen(checkoutfile)+1);

        if (mkfifo(file_fifoe,0666)==-1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
        //message queue push code 
    }



}