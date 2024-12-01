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
#include "process.h"

process list[100];
int cnt=0;

//message buffer struct 
typedef struct msgbuf 
{
    long mtype;
    char mtext[1024];
}Msgbuf;

void Split_Command(char command[][1000], char demand[] , int *idx)
{
    char *ptr = NULL;

    ptr = strtok(demand, " ");
    while (ptr != NULL && *idx < 5) 
    {
        //printf("ptr:%s\n", ptr);
        strcpy(command[*idx], ptr); // ptr이 NULL이 아님을 보장
        *idx+=1;
        ptr = strtok(NULL, " "); // 다음 토큰 탐색
    }

    // 남은 공간 초기화 (필요시)
    for (int i = *idx; i < 5; i++) {
        command[i][0] = '\0';
    }
}


void Command_Exception(char *command[], int *idx)
{
    char addfilename[100]={'\0'};
    char c_msgqid[100]={'\0'};

    if (strcmp(command[0],"git")!=0)
    {
        printf("usage: git [command] [option]\n");
    }
    else 
    {
        if (strcmp(command[1], "add") == 0)
        {
            char *result = add(*idx, command); // add 함수에서 문자열 반환
            if (result != NULL)
            {
                strcpy(addfilename, result); // 복사
            }
            printf("add file name:%s\n",addfilename);
        }   

        if (strcmp(command[1],"branch")==0)
        {
            branch(*idx,command);
        }

        if (strcmp(command[1],"clone")==0)
        {
            clone(*idx,command);
        }

        if (strcmp(command[1],"checkout")==0)
        {
            checkout(*idx,command);
        }

        if (strcmp(command[1],"push")==0 || strcmp(command[1],"pull")==0)
        {
            strcpy(command[*idx],addfilename);
            //command[*idx]=addfilename;
            *idx+=1;
            pushpull_main(*idx,command);
        }
    }
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
    char *argv[5]={NULL};
    int idx=0;

    repo_key=ftok(av[0],1);
    //to setup branch, allocate 3KB share memory 
    shmid=shmget(repo_key,4096,IPC_CREAT |0644);

    msid=msgget(repo_key,IPC_CREAT | 0644); //permission : rw-r-----, id=0, 동일
    //printf("msid:%d\n",msid);

    //second parameter 0 -> now message queue identifier return
    if ((msid=msgget(repo_key,0))<0)
    {
        perror("msgget");
        exit(1);
    }
    
    if (shmid==-1)
    {
        perror("shmget");
        exit(1);
    }

    printf("input command\n");
    while (fgets(buf,1000,stdin)!=NULL)
    {
        printf("input command\n");
        buf[strlen(buf)-1]='\0';
        //printf("buf:%s\n",buf);
        idx=0;
        Split_Command(command,buf,&idx);

        for(int i=0; i<idx; i++)
        {
            argv[i]=command[i];
            //printf("command[%d]:%s\n",i,command[i]);
        }

        Command_Exception(argv,&idx);
    }

    return 0;
}