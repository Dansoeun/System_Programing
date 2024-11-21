#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "process.h"

extern char * Get_Branch(process list[100], char target[512]);

int main(int ac, char *av[])
{
    //process list 추후에 수정 필요
    process list[100];
    char name[512]={'\0'};
    char target[512]={'\0'};
    char buf[512]={'\0'};
    strcpy(target,Get_Branch(list,name));

    int in_fd=0;

    int out_fd=0; //서버에서 클라이언트로 데이터를 전송할 것이므로 쓰기 전용 열기
    int n=0;

    if ((out_fd=open(target,O_WRONLY))==-1)
    {
        perror("server open");
        exit(1);
    }

    printf("To branch:%s\n",target);

    /*while ((n=read()))
    {

    }*/

    close(out_fd);
    return 0;


}