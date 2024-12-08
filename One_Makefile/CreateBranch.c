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
#include <sys/wait.h>
#include "global.h"

#define SHM_SIZE 4096 // 공유 메모리 크기
extern process list[100];
extern int cnt;

void CreateBranch(const char *b_name, int shmid, void *shmaddr, const char *master_fifo_path) {
    pid_t pid;
    char fifo_path[256];
    int parent_pid=-1;

    if (strcmp(b_name, "master") == 0) {
        // master branch 처리
        snprintf((char *)shmaddr, SHM_SIZE, "[Master branch initialized]\n");
        snprintf(fifo_path, sizeof(fifo_path), "%s", master_fifo_path);
        
        if (mkfifo(fifo_path, 0777) == -1 && errno != EEXIST) {
            perror("mkfifo");
            exit(1);
        }

        // master branch를 위한 읽기 프로세스 생성
        /*
        pid = fork();
        if (pid == 0) {
            while (1) {
                int master_fd = open(fifo_path, O_RDONLY);
                if (master_fd != -1) {
                    char buffer[4096] = {0};
                    ssize_t bytes_read = read(master_fd, buffer, sizeof(buffer));
                    if (bytes_read > 0) {
                        printf("[Master Branch] Received: %s\n", buffer);
                    }
                    close(master_fd);
                }
                sleep(1);
            }
            exit(0);
        }*/
       parent_pid=getpid();
       list[cnt].pid=parent_pid+1;
       strcpy(list[cnt].fifo_file_name,"master");
       cnt+=1;
        
        printf("Master branch created with FIFO '%s'.\n", fifo_path);
        return;
    }

    else 
    {
        if (mkfifo(b_name,0777)==-1)
        {
            perror("branch mkfifo");
            exit(1);
        }

        //pid=fork();

        parent_pid=getpid();
        list[cnt].pid=parent_pid+1;
        strcpy(list[cnt].fifo_file_name,b_name);
        cnt+=1;

        printf("%s branch created with FIFO\n", b_name);
    }
}