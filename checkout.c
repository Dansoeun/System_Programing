#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <errno.h>
#include "cp.h"
#include "process.h"
#include "global.h"

#define SHM_SIZE 4096

#define BUFFER_SIZE 4096
#define MESSAGE_DELIMITER "\n---END_OF_MESSAGE---\n"
#define PERMISSIONS 0755

void create_file_in_folder(const char *folder_name, const char *file_name, const char *content) {
    // 폴더 생성 (존재하지 않는 경우)
    if (mkdir(folder_name, 0755) == -1 && errno != EEXIST) {
        perror("폴더 생성 실패");
        exit(1);
    }

    // 파일 경로 생성
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%s", folder_name, file_name);

    // 파일 생성 및 내용 쓰기
    int fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("파일 생성 실패");
        exit(1);
    }

    if (write(fd, content, strlen(content)) == -1) {
        perror("파일 쓰기 실패");
        close(fd);
        exit(1);
    }

    close(fd);
    printf("파일 생성 완료: %s\n", file_path);
}

//git checkout [생성폴더이름] [fifo 경로]
void checkout(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: git checkout [branch_name] [fifo_path]\n");
        exit(1);
    }

    const char *branch_name = argv[2];
    const char *fifo_path = argv[3];
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE] = {0};
    size_t message_pos = 0;
    int cnt = 0;

    printf("브랜치 '%s'로 체크아웃 중...\n", branch_name);

    // 공유 메모리 접근
    key_t repo_key = ftok(".", 1); // Example key generation
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

    // FIFO 파일 열기
    int fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd == -1) {
        perror("FIFO 열기 실패");
        exit(1);
    }

    printf("FIFO로부터 데이터 수신 대기 중...\n");

    ssize_t bytes_read;
    while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';

        char *current_pos = buffer;
        char *delimiter_pos;

        // 메시지에서 구분자를 기준으로 메시지를 나눔
        while ((delimiter_pos = strstr(current_pos, MESSAGE_DELIMITER)) != NULL) {
            size_t chunk_size = delimiter_pos - current_pos;

            // 메시지 버퍼에 복사
            if (message_pos + chunk_size < BUFFER_SIZE) {
                memcpy(message + message_pos, current_pos, chunk_size);
                message_pos += chunk_size;
                message[message_pos] = '\0';

                // 파일 생성
                char file_name[256];
                snprintf(file_name, sizeof(file_name), "file_%lu.txt", time(NULL) + cnt++);
                create_file_in_folder(branch_name, file_name, message);

                // 메시지 버퍼 초기화
                message_pos = 0;
                memset(message, 0, BUFFER_SIZE);
            }

            // 다음 메시지로 이동
            current_pos = delimiter_pos + strlen(MESSAGE_DELIMITER);
        }

        // 남은 데이터 처리
        size_t remaining = strlen(current_pos);
        if (remaining > 0) {
            // 메시지의 나머지를 버퍼에 복사
            if (message_pos + remaining < BUFFER_SIZE) {
                memcpy(message + message_pos, current_pos, remaining);
                message_pos += remaining;
                message[message_pos] = '\0';
            }
        }
    }

    if (bytes_read == -1) {
        perror("FIFO 읽기 실패");
    } else {
        printf("FIFO 데이터 수신 완료\n");
    }

    // 남아있는 메시지 처리 (구분자가 없어서 마지막에 남은 메시지)
    if (message_pos > 0) {
        char file_name[256];
        snprintf(file_name, sizeof(file_name), "file_%lu.txt", time(NULL));
        create_file_in_folder(branch_name, file_name, message);
    }

    close(fifo_fd);

    if (shmdt(shmaddr) == -1) {
        perror("공유 메모리 분리 실패");
    }

    printf("브랜치 '%s'로 체크아웃 완료.\n", branch_name);
}
