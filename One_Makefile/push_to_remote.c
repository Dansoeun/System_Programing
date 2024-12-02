#include "pushpullheader.h"
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// 메시지 버퍼 구조체
typedef struct msgbuf {
    long mtype;
    char mtext[4096];
} Msgbuf;

void push_to_remote(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: git push origin [branch_fifo_path]\n");
        exit(1);
    }

    int msqid = 0; // 메시지 큐 ID (고정값 0)
    Msgbuf msg;

    // FIFO 파일 열기
    int fifo_fd = open(argv[3], O_WRONLY | O_APPEND);
    if (fifo_fd == -1) {
        perror("FIFO 파일 열기 실패");
        exit(1);
    }

    printf("FIFO 파일(%s)에 메시지 전송 준비 완료.\n", argv[3]);

    // 메시지 큐에서 메시지 수신 및 FIFO 파일로 쓰기
    while (1) {
        ssize_t len = msgrcv(msqid, &msg, sizeof(msg.mtext), 0, IPC_NOWAIT);
        if (len == -1) {
            if (errno == ENOMSG) {
                // 메시지가 없으면 종료
                printf("메시지 큐에 더 이상 메시지가 없습니다.\n");
                break;
            } else {
                perror("메시지 큐 수신 실패");
                close(fifo_fd);
                exit(1);
            }
        }

        // FIFO 파일에 메시지 쓰기
        if (write(fifo_fd, msg.mtext, len) == -1) {
            perror("FIFO 파일 쓰기 실패");
            close(fifo_fd);
            exit(1);
        }

        printf("메시지 추가: %s (길이: %zd)\n", msg.mtext, len);
    }

    // FIFO 파일 닫기
    close(fifo_fd);
    printf("\033[7m브랜치 FIFO 파일(%s) 갱신 완료.\033[0m\n", argv[3]);
}
