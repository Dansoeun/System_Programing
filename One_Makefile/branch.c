#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>  // 공유 메모리 관련 함수 사용을 위한 헤더
#include "process.h"
#include "CreateBranch.h"

#define SHM_SIZE 4096 // 공유 메모리 크기

process list[100];
int cnt = 0;

void checkout_handler(int signum) {
    pid_t target = 0;
    char fifoname[512] = {'\0'};
    char branchname[512] = {'\0'};

    printf("Please write the name of the branch you want to check out: ");
    scanf("%s", branchname);
    target = Get_Branch(list, branchname);

    strcpy(fifoname, Get_FIFO_name(list, target));

    printf("Target FIFO: %s\n", fifoname);
}

void branch(int argc, char *argv[]) {   //int main()
    key_t repo_key;
    int shmid;
    void *shmaddr;
    char branch_name[100];
    char master_fifo_path[256] = "./master_fifo";

    signal(SIGUSR2, checkout_handler);

    if (argc != 4) {
        fprintf(stderr, "Usage: %s branch -b <branch_name>\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "branch") != 0 || strcmp(argv[2], "-b") != 0) {
        fprintf(stderr, "Invalid command. Usage: %s branch -b <branch_name>\n", argv[0]);
        exit(1);
    }

    strcpy(branch_name, argv[3]);
    if (strlen(branch_name) == 0) {
        fprintf(stderr, "Branch name cannot be empty.\n");
        exit(1);
    }

    repo_key = ftok(argv[0], 1);
    if (repo_key == -1) {
        perror("ftok");
        exit(1);
    }

    shmid = shmget(repo_key, SHM_SIZE, IPC_CREAT | 0644);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    if (access(master_fifo_path, F_OK) == -1) {
        printf("Master branch not found. Creating master branch…\n");
        CreateBranch("master", shmid, shmaddr, master_fifo_path);
    }

    CreateBranch(branch_name, shmid, shmaddr, master_fifo_path);
    return;
}
