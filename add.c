#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include "global.h"

#define STAGE_DIR ".mygit_stage"
#define SENDSIZE 4096

struct file_message {
    long msg_type;
    char file_name[256];
    char file_contents[SENDSIZE];
};

int is_directory(const char *path);
void send_file_to_stage(int msqid, const char *file_path);
void handle_directory(int msqid, const char *dir_path);

void add(int argc, char *argv[]) {
    if (argc < 2) {
        perror("argc");
        return;
    }

    int msqid = msid; // 전역 변수 사용
    printf("add msqid: %d\n", msqid);

    for (int i = 2; i < argc; i++) {
        const char *path = argv[i];
        if (is_directory(path)) {
            handle_directory(msqid, path);
        } else {
            send_file_to_stage(msqid, path);
        }
    }
}

int is_directory(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) return 0;
    return S_ISDIR(path_stat.st_mode);
}

void send_file_to_stage(int msqid, const char *file_path) {
    struct file_message msg;
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("파일 열기 실패");
        return;
    }

    msg.msg_type = 1;
    strncpy(msg.file_name, file_path, sizeof(msg.file_name) - 1);

    ssize_t bytes_read;
    while ((bytes_read = read(fd, msg.file_contents, sizeof(msg.file_contents) - 1)) > 0) {
        msg.file_contents[bytes_read] = '\0';
        if (msgsnd(msqid, &msg, SENDSIZE, 0) == -1) {
            perror("메시지 전송 실패");
            close(fd);
            return;
        }
    }

    close(fd);
    printf("파일 전송 완료: %s\n", file_path);
}

void handle_directory(int msqid, const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("디렉터리 열기 실패");
        return;
    }

    struct dirent *entry;
    char full_path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        if (is_directory(full_path)) {
            handle_directory(msqid, full_path);
        } else {
            send_file_to_stage(msqid, full_path);
        }
    }

    closedir(dir);
}
