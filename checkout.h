#ifndef CHECKOUT_H
#define CHECKOUT_H

#include <sys/types.h>

void checkout(int argc, char *argv[]);
void sig_to_branch(char branchname[], int signum,pid_t branch_pid);

#endif