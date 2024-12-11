#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "process.h"
#include "CreateBranch.h"
#include "global.h"

#define SHM_SIZE 4096

extern process list[100];
extern int cnt;

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

// 브랜치가 이미 존재하는지 확인하는 함수
int Is_Branch_Exists(const char *branch_name) {
    for (int i = 0; i < cnt; i++) {
        if (strcmp(list[i].fifo_file_name, branch_name) == 0) {
            return 1; // 이미 존재
        }
    }
    return 0; // 존재하지 않음
}

void branch(int argc, char *argv[]) {
    key_t repo = repo_key;
    int shmid;
    void *shmaddr;
    char branch_name[100];
    char master_fifo_path[256] = "./master_fifo";

    signal(SIGUSR2, checkout_handler);

    if(argc ==2 && strcmp(argv[1], "branch") == 0){
        printf("Existing branches:\n");
        for(int i=0; i<cnt; i++){
            printf("- %s\n", list[i].fifo_file_name);
        }
        return;
    }

    else if (argc != 4) {
        fprintf(stderr, "Usage: %s branch -b <branch_name>\n", argv[0]);
        return;
    }

    else if (strcmp(argv[1], "branch") != 0 || strcmp(argv[2], "-b") != 0) {
        fprintf(stderr, "Invalid command. Usage: %s branch -b <branch_name>\n", argv[0]);
        return;
    }

    else{
        strcpy(branch_name, argv[3]);
        if (strlen(branch_name) == 0) {
            fprintf(stderr, "Branch name cannot be empty.\n");
            return;
        }
    }

    // 공유 메모리 생성
    shmid = shmget(repo, SHM_SIZE, IPC_CREAT | 0644);
    if (shmid == -1) {
        perror("shmget");
        return;
    }

    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *)-1) {
        perror("shmat");
        return;
    }

    // master 브랜치가 없으면 생성
    if(!Is_Branch_Exists("master_fifo")){
        printf("master branch not found. Creating master branch...\n");
        CreateBranch("master", shmid, shmaddr, master_fifo_path);

        //Add master brnach to list
        /*strncpy(list[cnt].fifo_file_name, "master", sizeof(list[cnt].fifo_file_name)-1);
        list[cnt].fifo_file_name[sizeof(list[cnt].fifo_file_name)-1]='\0';
        cnt++;
        */
    }

      // 중복 브랜치 체크
    if (Is_Branch_Exists(branch_name)) {
        printf("Error: Branch '%s' already exists.\n", branch_name);
        return; // 중복 생성 방지
    }

    // 새로운 브랜치 생성
    CreateBranch(branch_name, shmid, shmaddr, master_fifo_path);

    // 브랜치 정보를 리스트에 추가
    /*strncpy(list[cnt].fifo_file_name, branch_name, sizeof(list[cnt].fifo_file_name) - 1);
    list[cnt].fifo_file_name[sizeof(list[cnt].fifo_file_name) - 1] = '\0'; // null-terminate
    cnt++;
    */

    return;
}
