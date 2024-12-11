#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define PATH_MAX 4096

void check_size(const char* save_dir, const char* file_name, char* dest_path, size_t dest_size) {
    // 저장 경로와 파일 이름의 총 길이 확인
    size_t dest_length = strlen(save_dir) + strlen(file_name) + 2; // '/' 및 널 문자 포함
    if (dest_length > PATH_MAX) {
        fprintf(stderr, "경로가 너무 깁니다. 저장할 수 없습니다.\n");
        exit(EXIT_FAILURE);
    }

    // 경로 생성
    if (snprintf(dest_path, dest_size, "%s/%s", save_dir, file_name) >= dest_size) {
        fprintf(stderr, "경로 생성 중 오류 발생: 저장 경로가 너무 깁니다.\n");
        exit(EXIT_FAILURE);
    }
}
