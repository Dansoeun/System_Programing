#include "pushpullheader.h"
#include "global.h"
#include <libgen.h>

#define SHM_SIZE 4096
#define BUFFER_SIZE 4096
#define MESSAGE_DELIMITER "\n---END_OF_MESSAGE---\n"

void pull_from_remote(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: git pull origin [branch_fifo_path]\n");
        exit(1);
    }

    key_t repo_key = repo_key;
    int shmid = shmget(repo_key, SHM_SIZE, IPC_CREAT | 0644);
    if (shmid == -1) {
        perror("공유 메모리 접근 실패");
        exit(1);
    }

    void *shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *)-1) {
        perror("공유 메모리 연결 실패");
        exit(1);
    }

    printf("공유 메모리 연결 완료. 메모리 주소: %p\n", shmaddr);

    printf("FIFO 파일 열기 시도 중: %s\n", argv[3]);
    int fifo_fd;
    int retry_count = 0;
    
    while (retry_count < 5) {
        fifo_fd = open(argv[3], O_RDONLY | O_NONBLOCK);
        if (fifo_fd != -1) break;
        printf("재시도 %d/5...\n", retry_count + 1);
        sleep(1);
        retry_count++;
    }

    if (fifo_fd == -1) {
        perror("FIFO 파일 열기 실패");
        exit(1);
    }

    // FIFO를 blocking 모드로 변경
    int flags = fcntl(fifo_fd, F_GETFL);
    fcntl(fifo_fd, F_SETFL, flags & ~O_NONBLOCK);

    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE] = {0};
    size_t message_pos = 0;
    ssize_t bytes_read;

    printf("시작: FIFO로부터 읽기 대기 중...\n");

    while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        
        // 메시지를 처리하고 구분자를 찾음
        char *current_pos = buffer;
        char *delimiter_pos;

        while ((delimiter_pos = strstr(current_pos, MESSAGE_DELIMITER)) != NULL) {
            size_t chunk_size = delimiter_pos - current_pos;
            
            // 메시지 버퍼에 복사
            if (message_pos + chunk_size < BUFFER_SIZE) {
                memcpy(message + message_pos, current_pos, chunk_size);
                message_pos += chunk_size;
                message[message_pos] = '\0';
                
                // 완성된 메시지 출력
                printf("[Pull] Received Message:\n%s\n", message);
                
                // 메시지 버퍼 초기화
                message_pos = 0;
                memset(message, 0, BUFFER_SIZE);
            }
            
            // 다음 메시지로 이동
            current_pos = delimiter_pos + strlen(MESSAGE_DELIMITER);
        }
        
        // 남은 데이터 처리
        size_t remaining = strlen(current_pos);
        if (remaining > 0 && message_pos + remaining < BUFFER_SIZE) {
            memcpy(message + message_pos, current_pos, remaining);
            message_pos += remaining;
            message[message_pos] = '\0';
        }
    }

    if (bytes_read == -1) {
        perror("FIFO 읽기 실패");
    } else {
        printf("FIFO 파일 읽기 완료\n");
    }

    close(fifo_fd);
    
    if (shmdt(shmaddr) == -1) {
        perror("공유 메모리 분리 실패");
    }

    printf("\033[7mPull 작업 완료.\033[0m\n");
}