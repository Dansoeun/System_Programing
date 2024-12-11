#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include "file_copy.h"
#include "file_check.h"
#include "directory_check.h"

#define PATH_MAX 4096

void clone_process(const char* source_path, const char* destination_dir) {
    struct stat source_stat;
    if (stat(source_path, &source_stat) != 0) {
        perror("소스 경로 확인 실패");
        exit(EXIT_FAILURE);
    }

    // 대상 디렉토리가 없으면 생성
    struct stat dest_stat;
    if (stat(destination_dir, &dest_stat) != 0) {
        if (mkdir(destination_dir, 0755) != 0) {
            perror("대상 디렉토리 생성 실패");
            exit(EXIT_FAILURE);
        }
        printf("대상 디렉토리를 생성했습니다: %s\n", destination_dir);
    }

    // 소스가 파일인지 디렉토리인지 확인
    if (S_ISREG(source_stat.st_mode)) {
        // 파일 복사
        const char* file_name = strrchr(source_path, '/') + 1; // '/'단위로 나누기
        char dest_path[PATH_MAX];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", destination_dir, file_name);

        if (clone_copy_file(source_path, dest_path) == 0) {
            printf("파일 복사 완료: %s -> %s\n", source_path, dest_path);
        } else {
            perror("파일 복사 실패");
        }
    } else if (S_ISDIR(source_stat.st_mode)) {
        // 디렉토리 내부 파일 복사
        DIR* dir = opendir(source_path);
        if (dir == NULL) {
            perror("디렉토리 열기 실패");
            exit(EXIT_FAILURE);
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            // 현재 디렉토리와 상위 디렉토리는 건너뜀
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            // 소스 파일의 전체 경로 생성
            char source_file_path[PATH_MAX];
            snprintf(source_file_path, sizeof(source_file_path), "%s/%s", source_path, entry->d_name);

            // 디렉토리인지 확인하고 무시
            struct stat entry_stat;
            if (stat(source_file_path, &entry_stat) == 0 && S_ISDIR(entry_stat.st_mode)) {
                printf("디렉토리는 복사하지 않습니다: %s\n", source_file_path);
                continue;
            }

            // 대상 파일의 전체 경로 생성
            char dest_file_path[PATH_MAX];
            snprintf(dest_file_path, sizeof(dest_file_path), "%s/%s", destination_dir, entry->d_name);

            // 파일 복사
            printf("복사 중: %s -> %s\n", source_file_path, dest_file_path);
            if (clone_copy_file(source_file_path, dest_file_path) != 0) {
                perror("파일 복사 실패");
            } else {
                printf("복사 완료: %s -> %s\n", source_file_path, dest_file_path);
            }
        }
        closedir(dir);
    } else {
        fprintf(stderr, "소스가 파일이나 디렉토리가 아닙니다: %s\n", source_path);
        exit(EXIT_FAILURE);
    }
}
