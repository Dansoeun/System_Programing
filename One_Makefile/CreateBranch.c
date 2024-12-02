#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>  // 공유 메모리 관련 함수 사용을 위한 헤더
#include "CreateBranch.h"
#include "process.h"

#define SHM_SIZE 4096 // 공유 메모리 크기
extern process list[100];
extern int cnt;

void CreateBranch(const char *b_name, int shmid, void *shmaddr, const char *master_fifo_path) {
    pid_t pid;
    char fifo_path[256];

    if (strcmp(b_name, "master") == 0) {
        snprintf((char *)shmaddr, SHM_SIZE, "[Master branch initialized]\n");

        snprintf(fifo_path, sizeof(fifo_path), "%s", master_fifo_path);
        if (mkfifo(fifo_path, 0777) == -1 && errno != EEXIST) {
            perror("mkfifo");
            exit(1);
        }

        printf("Master branch created with FIFO '%s'.\n", fifo_path);
        return;
    }

    snprintf(fifo_path, sizeof(fifo_path), "./%s_fifo", b_name);
    if (mkfifo(fifo_path, 0777) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        unlink(fifo_path);
        exit(1);
    }

    if (pid == 0) {
        int branch_fifo_fd;
        branch_fifo_fd = open(fifo_path, O_RDONLY | O_NONBLOCK);
        if (branch_fifo_fd == -1) {
            perror("open branch fifo");
            exit(1);
        }

        while (1) {
            char buffer[256] = {0};
            if (read(branch_fifo_fd, buffer, sizeof(buffer)) > 0) {
                printf("[Branch: %s] Received: %s\n", b_name, buffer);
            }
        }
    } else {
        strcpy(list[cnt].branch, b_name);
        strcpy(list[cnt].fifo_file_name, fifo_path);
        list[cnt].pid = pid;
        cnt += 1;

        printf("Branch '%s' created with PID %d and FIFO '%s'.\n", b_name, pid, fifo_path);
    }
}
