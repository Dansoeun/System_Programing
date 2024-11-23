#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//message buffer struct 
typedef struct msgbuf 
{
    long mtype;
    char mtext[1024];
}Msgbuf;

void sig_handler(int sig)
{
    printf("Receive %d signal..\n",sig);
    return;
}

void Upload_file(key_t repo_key, int msid ) //upload file in repository
{
    int n_char=0;
    Msgbuf msgbuf;

    //message recieved
    while ((n_char=msgrcv(msid,&msgbuf,1024,0,0))>0)
    {
        //repo copy code update
    }
}

//connect shared memory and create branch
void CreateBranch(char *b_name)
{
    //branch create function 

}

//parameter: av[1]: repo path , av[2]: setting IPC id , av[3]: is create branch? av[4]: create branch name
int main(int ac, char *av[])
{
    key_t repo_key;  //create IPC object to repository
    Msgbuf msgbuf;
    sigset_t mask;
    int msid; //message queue identifier
    int n_char=0;
    int shmid;
    void *shmaddr;
    char buf[100]={'\0'};

    repo_key=ftok(av[0],av[1]);
    msid=msgget(repo_key,IPC_CREAT | 0640); //permission : rw-r-----

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

    //branch connect ,write and create
    sigfillset(&mask);
    sigdelset(&mask,SIGUSR1);
    sigset(SIGUSR1,sig_handler);

    //if signal listen, connect shared memory ,create and write data at branch
    if (av[3]=='1') //crate branch bit 
    {
        shmaddr=shmat(shmid,NULL,0);
        strcpy(buf,shmaddr);
        CreateBranch(buf);
        sleep(2);
        //disconnect shared memory
        shmdt(shmaddr);
    }

    return 0;
}