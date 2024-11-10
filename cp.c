#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define COPYMODE 0644

void Copy(int ac, char *av[])
{
    int in_fd=1;
    int out_fd=1;
    int n_char=0;
    int w_char=0;
    char buf[1024]={'\0'};

    in_fd=open(av[1], O_RDONLY);
    out_fd=creat(av[2],COPYMODE);

    if (in_fd==-1 || out_fd==-1)
    {
        perror("open or creat error");
        exit(EXIT_FAILURE);
    }

    while ((n_char=read(in_fd,buf,1024))>0)
    {
        w_char=write(out_fd,buf,n_char);

        if (w_char!=n_char)
        {
            perror("write error");
            exit(-1);
        }
    }

}