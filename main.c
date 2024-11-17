#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(void)
{
    printf("Do you want to real close? then, please enter the signal one more");
    sleep(2);
}

void set_signal(int signum)
{
    struct sigaction git_handler;
    sigset_t blocked;

    git_handler.sa_handler=signal_handler;
    git_handler.sa_flags= SA_RESETHAND |SA_RESTART | SA_SIGINFO;

    sigemptyset(&blocked);

    sigaddset(&blocked,SIGINT);
    sigaddset(&blocked,SIGINT);
    sigaddset(&blocked,SIGTSTP);

    git_handler.sa_mask=blocked;

    if (sigaction(SIGINT,&git_handler,NULL)==-1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    else 
    {
        
    }




}

int main(int ac, char *av[])
{

}