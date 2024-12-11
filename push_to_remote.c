#include "pushpullheader.h"
#include "global.h"

#define SHM_SIZE 4096
#define MESSAGE_DELIMITER "\n---END_OF_MESSAGE---\n"

void push_to_remote(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: git push origin [branch_fifo_path]\n");
        exit(1);
    }

    key_t repo_key = repo_key;
    int shmid;
    void *shmaddr;

    // 공유 메모리 생성 및 연결
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

    printf("공유 메모리 연결 완료. 메모리 주소: %p\n", shmaddr);

    // 공유 메모리 초기화
    memset(shmaddr, 0, SHM_SIZE);
    size_t shm_offset = 0;  // 공유 메모리 쓰기 위치 추적

    int msqid = msid;
    struct file_message msg;

    printf("push msqid:%d\n", msqid);

    int fifo_fd;
    struct stat st;

    printf("FIFO 파일 열기 시도 중: %s\n", argv[3]);
    int retry_count = 0;
    while (retry_count < 5) {
        fifo_fd = open(argv[3], O_WRONLY);
        if (fifo_fd != -1) break;
        
        printf("재시도 %d/5...\n", retry_count + 1);
        sleep(1);
        retry_count++;
    }
    
    if (fifo_fd == -1) {
        perror("FIFO 파일 열기 실패");
        exit(1);
    }
    
    int flags = fcntl(fifo_fd, F_GETFL);
    fcntl(fifo_fd, F_SETFL, flags & ~O_NONBLOCK);
    
    while (1) {
        ssize_t len = msgrcv(msqid, &msg, sizeof(msg), 0, IPC_NOWAIT);
        if (len == -1 || len == 0 || len == 1) {
            if (errno == ENOMSG || len == 0 || len == 1) {
                printf("메시지 큐에 더 이상 메시지가 없습니다.\n");
                break;
            } else {
                perror("메시지 큐 수신 실패");
                close(fifo_fd);
                exit(1);
            }
        }
        size_t msg_len = strlen(msg.file_contents);
        if (msg_len > 0) {
            // FIFO 파일에 메시지 내용 쓰기
            if (write(fifo_fd, msg.file_contents, msg_len) == -1) {
                perror("FIFO 파일 쓰기 실패");
                close(fifo_fd);
                exit(1);
            }
            // FIFO 파일에 구분자 쓰기
            if (write(fifo_fd, MESSAGE_DELIMITER, strlen(MESSAGE_DELIMITER)) == -1) {
                perror("FIFO 구분자 쓰기 실패");
                close(fifo_fd);
                exit(1);
            }
            // 공유 메모리에 메시지 저장
            size_t total_len = msg_len + strlen(MESSAGE_DELIMITER);
            if (shm_offset + total_len <= SHM_SIZE) {
                // 메시지 내용 복사
                memcpy(shmaddr + shm_offset, msg.file_contents, msg_len);
                shm_offset += msg_len;
                
                // 구분자 복사
                memcpy(shmaddr + shm_offset, MESSAGE_DELIMITER, strlen(MESSAGE_DELIMITER));
                shm_offset += strlen(MESSAGE_DELIMITER);
                
                printf("공유 메모리에 메시지 저장 완료 (offset: %zu)\n", shm_offset);
            } else {
                printf("경고: 공유 메모리 공간 부족 (필요: %zu, 가능: %zu)\n", 
                       total_len, SHM_SIZE - shm_offset);
            }
            // 버퍼 즉시 비우기
            fsync(fifo_fd);
            printf("메시지 추가: %s (길이: %zd)\n", msg.file_contents, msg_len);
        }
    }

    close(fifo_fd);

    // 공유 메모리의 총 사용량을 저장
    *((size_t *)shmaddr) = shm_offset;

    if (shmdt(shmaddr) == -1) {
        perror("공유 메모리 분리 실패");
    }

    printf("\033[7m브랜치 FIFO 파일(%s) 및 공유 메모리 갱신 완료.\033[0m\n", argv[3]);
    return;
}