#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "directory_check.h"
#include "file_copy.h"
#include "file_check.h"
#include "clone_process.h"

#define PATH_MAX 4096 

void clone(int argc, char* argv[]) {
    if (argc < 3) { 
        fprintf(stderr, "사용법: %s clone <파일/디렉토리 경로> <저장 디렉토리>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Step 1: 파일 경로 처리
    char* file_path = argv[2];

    // Step 2: 저장 디렉토리 결정
    const char* default_dir = "~/clone_repos"; // 기본 저장 디렉토리
    const char* save_dir;
    if (argc > 3) {
        save_dir = argv[3];
    } else {
        save_dir = default_dir;
    }

    // Step 3: ~을 절대경로로 변환
    char expanded_save_dir[PATH_MAX];
    if (save_dir[0] == '~') {
        const char* home = getenv("HOME"); // HOME 환경 변수 가져오기
        if (home == NULL) {
            fprintf(stderr, "HOME 환경 변수 설정이 필요합니다.\n");
            exit(EXIT_FAILURE);
        }
        if (strlen(home) + strlen(save_dir) - 1 >= PATH_MAX) { //공유메모리 값 초과했을경우,
            fprintf(stderr, "저장 디렉토리 경로가 너무 깁니다.\n");
            exit(EXIT_FAILURE);
        }
        snprintf(expanded_save_dir, sizeof(expanded_save_dir), "%s%s", home, save_dir + 1);
        save_dir = expanded_save_dir;
    }

    // 기존 단일 파일 복사 로직 (Step 2, Step 4)
    struct stat file_stat;
    if (stat(file_path, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
        printf("파일 복사 작업 실행\n");
        const char* file_name = strrchr(file_path, '/');
        if (!file_name) {
            file_name = file_path; // 파일 이름이 '/'를 포함하지 않을 경우
        } else {
            file_name++; // '/' 뒤의 파일 이름
        }

        // 길이 체크 (경고 방지)
        size_t dest_length = strlen(save_dir) + strlen(file_name) + 2; // '/' 및 널 문자 포함
        if (dest_length > PATH_MAX) {
            fprintf(stderr, "경로가 너무 깁니다. 저장할 수 없습니다.\n");
            exit(EXIT_FAILURE);
        }

        // snprintf 호출
        char dest_path[PATH_MAX];
        if (snprintf(dest_path, sizeof(dest_path), "%s/%s", save_dir, file_name) >= PATH_MAX) {
            fprintf(stderr, "경로 생성 중 오류 발생: 저장 경로가 너무 깁니다.\n");
            exit(EXIT_FAILURE);
        }

        if (clone_copy_file(file_path, dest_path) == 0) {
            printf("파일 복사가 성공적으로 완료되었습니다: %s -> %s\n", file_path, dest_path);
        } else {
            perror("파일 복사 실패");
        }
        return;
    }

    // 파일 to 폴더 / 폴더 to 폴더 구현
    if (S_ISDIR(file_stat.st_mode)) {
        printf("디렉토리 복사 작업 실행\n");
        clone_process(file_path, save_dir); // 파일 to 폴더 / 폴더 to 폴더 구현함수
        return;
    }

    fprintf(stderr, "알 수 없는 파일 유형입니다: %s\n", file_path);
    exit(EXIT_FAILURE);
}
