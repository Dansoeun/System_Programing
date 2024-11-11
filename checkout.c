#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cp.h"
#include "process.h"

int main(int ac, char *av[])
{
    int fd1=1;
    int fd2=1;
    char message[2048]={'\0'};
    int n=0;

    Copy(ac,av);

    close(fd1);
    close(fd2);
    return 0;
}