#include "pushpullheader.h"
#include "global.h"

#define SHM_SIZE 4096 // 공유 메모리 크기

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

    key_t repo_key = repo_key; // 공유 메모리 키
    int shmid;
    void *shmaddr;

    // 공유 메모리 접근
    shmid = shmget(repo_key, SHM_SIZE, 0644);
    if (shmid == -1) {
        perror("공유 메모리 접근 실패");
        exit(1);
    }

    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *)-1) {
        perror("공유 메모리 연결 실패");
        exit(1);
    }

    printf("공유 메모리 연결 완료. 메모리 주소: %p\n", shmaddr);

    int msqid = msid; // 메시지 큐 ID
    Msgbuf msg;

    printf("push msqid:%d\n", msqid);

    // FIFO 파일 열기 (수정된 부분)
    int fifo_fd;
    struct stat st;
    
    // FIFO 파일 존재 확인
    if (stat(argv[3], &st) == -1) {
        fprintf(stderr, "FIFO 파일이 존재하지 않습니다: %s\n", argv[3]);
        exit(1);
    }
    
    // FIFO가 실제로 FIFO 파일인지 확인
    if (!S_ISFIFO(st.st_mode)) {
        fprintf(stderr, "유효하지 않은 FIFO 파일입니다: %s\n", argv[3]);
        exit(1);
    }

    printf("FIFO 파일 열기 시도 중: %s\n", argv[3]);
    int retry_count = 0;
    while (retry_count < 5) {
        fifo_fd = open(argv[3], O_WRONLY | O_NONBLOCK);
        if (fifo_fd != -1) break;
        
        printf("재시도 %d/5...\n", retry_count + 1);
        sleep(1);
        retry_count++;
    }
    
    if (fifo_fd == -1) {
        perror("FIFO 파일 열기 실패");
        exit(1);
    }
    
    // FIFO를 blocking 모드로 전환
    int flags = fcntl(fifo_fd, F_GETFL);
    fcntl(fifo_fd, F_SETFL, flags & ~O_NONBLOCK);

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

    // 공유 메모리 분리
    if (shmdt(shmaddr) == -1) {
        perror("공유 메모리 분리 실패");
    }

    printf("\033[7m브랜치 FIFO 파일(%s) 갱신 완료.\033[0m\n", argv[3]);
}
