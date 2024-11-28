#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "directory_check.h"

void no_directory_message(const char* dir_path) {
    struct stat dir_stat;
    if (stat(dir_path, &dir_stat) != 0) { // 디렉터리가 없을 때
        printf("디렉터리가 존재하지 않습니다: %s\n", dir_path);
    } else if (!S_ISDIR(dir_stat.st_mode)) { // 디렉터리가 아니라면 오류
        fprintf(stderr, "%s는 디렉터리가 아닙니다.\n", dir_path);
        exit(EXIT_FAILURE);
    } else {
        printf("디렉터리가 이미 존재합니다: %s\n", dir_path);
    }
}
