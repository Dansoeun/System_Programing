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
#include "global.h"

process list[100];
key_t repo_key;
int cnt = 0;

// 전역 변수 정의
int msid = -1;
int isend = -1;

void Split_Command(char command[][1000], char demand[], int *idx) {
    char *ptr = NULL;
    ptr = strtok(demand, " ");
    while (ptr != NULL && *idx < 5) {
        strcpy(command[*idx], ptr);
        *idx += 1;
        ptr = strtok(NULL, " ");
    }

    for (int i = *idx; i < 5; i++) {
        command[i][0] = '\0';
    }
}

void Command_Exception(char *command[], int *idx) {
    if (strcmp(command[0], "git") != 0) {
        printf("usage: git [command] [option]\n");
        return;
    } else {
        if (strcmp(command[1], "add") == 0) {
            add(*idx, command); // add 함수 호출
        } else if (strcmp(command[1], "branch") == 0) {
            branch(*idx, command);
        } else if (strcmp(command[1], "clone") == 0) {
            clone(*idx, command);
        } else if (strcmp(command[1], "checkout") == 0) {
            checkout(*idx, command);
        } else if (strcmp(command[1], "push") == 0 || strcmp(command[1], "pull") == 0) {
            pushpull_main(*idx, command);
        } else {
            printf("usage: git [command] [option]\n");
        }
    }
}

int main(int ac, char *av[]) {
    key_t repo_key;
    int shmid;
    char buf[1000] = {'\0'};
    char command[5][1000] = {'\0'};
    char *argv[5] = {NULL};
    int idx = 0;

    repo_key = ftok(av[0], 1);

    shmid = shmget(repo_key, 4096, IPC_CREAT | 0644);
    msid = msgget(repo_key, IPC_CREAT | 0644);

    if (msid == -1) {
        perror("msgget");
        exit(1);
    }

    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    printf("main msid: %d\n", msid);

    while (1) {
        // 명령 입력 메시지 출력
        printf("input command\n");
        
        if (fgets(buf, 1000, stdin) == NULL) {
            break;
        }

        buf[strlen(buf) - 1] = '\0'; // 개행 문자 제거
        idx = 0;
        Split_Command(command, buf, &idx);

        for (int i = 0; i < idx; i++) {
            argv[i] = command[i];
        }

        Command_Exception(argv, &idx);
    }

    return 0;
}
