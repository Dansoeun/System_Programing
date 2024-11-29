#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "checkout.h"
#include "pushpullheader.h"
#include "branch.h"
#include "clone.h"
#include "add.h"

//message buffer struct 
typedef struct msgbuf 
{
    long mtype;
    char mtext[1024];
}Msgbuf;

void Split_Command(char command[][1000], char *demand)
{

}


//parameter: av[1]: repo path , av[2]: setting IPC id , av[3]: is create branch? av[4]: create branch name
int main(int ac, char *av[])
{
    key_t repo_key;  //create IPC object to repository
    Msgbuf msgbuf;
    int msid; //message queue identifier
    int n_char=0;
    int shmid;
    void *shmaddr;
    char buf[1000]={'\0'};
    char command[5][1000]={'\0'};

    repo_key=ftok(av[0],av[1]);
    msid=msgget(repo_key,IPC_CREAT | 0644); //permission : rw-r-----

    //second parameter 0 -> now message queue identifier return
    if ((msid=msgget(repo_key,0))<0)
    {
        perror("msgget");
        exit(1);
    }

    //to setup branch, allocate 3KB share memory 
    shmid=shmget(repo_key,4096,IPC_CREAT |0644);
    
    if (shmid==-1)
    {
        perror("shmget");
        exit(1);
    }

    while (fgets(buf,1000,stdin)!=NULL)
    {
        printf("input command\n");
        

    }







    return 0;
}