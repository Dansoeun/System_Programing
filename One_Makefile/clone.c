#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> // for realpath() to expand ~
#include "directory_check.h"
#include "file_copy.h"
#include "file_check.h"

#define PATH_MAX 4096 // 임시로

void clone(int argc, char* argv[]) {
    // argv[2] 파일경로, argv[3] : 레퍼지토리 경로 (?)
    printf("경로 명 %s %s",argv[2],argv[3]);
    if (argc < 2) {
        fprintf(stderr, "사용법: %s <파일 경로> [저장 디렉터리]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    

    // Step 1: 파일 경로 처리
    char* file_path = argv[2];

    // Step 2: 저장 디렉터리 결정
    const char* default_dir = "~/clone_repos"; // 기본 저장 디렉터리(추후 수정?)
    const char* save_dir;
    if (argc > 2) {
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
        snprintf(expanded_save_dir, sizeof(expanded_save_dir), "%s%s", home, save_dir + 1);
        save_dir = expanded_save_dir;
    }

    // 디렉터리 존재 여부 메시지 출력
    no_directory_message(save_dir);

    // 디렉터리가 없으면 생성
    struct stat dir_stat;
    if (stat(save_dir, &dir_stat) != 0) {
        if (mkdir(save_dir, 0755) != 0) {
            perror("디렉터리 생성 실패");
            exit(EXIT_FAILURE);
        }
        printf("디렉터리가 없어서 새로 생성했습니다: %s\n", save_dir);
    }

    // Step 4: 파일 존재 여부 및 복사
    struct stat file_stat;
    if (stat(file_path, &file_stat) == 0 && (file_stat.st_mode & S_IRUSR)) {
        printf("파일이 존재합니다\n");

        // 파일 이름만 추출
        const char* file_name = strrchr(file_path, '/') + 1;

        char dest_path[PATH_MAX];
        // snprintf로 경로 결합
        int ret = snprintf(dest_path, sizeof(dest_path), "%s/%s", save_dir, file_name);

        // 경로가 버퍼 크기를 초과하면 종료
        if (ret >= sizeof(dest_path)) {
            fprintf(stderr, "경로가 너무 깁니다. 저장할 수 없습니다.\n");
            exit(EXIT_FAILURE);
        }

        if (clone_copy_file(file_path, dest_path) == 0) {
            printf("파일 복사가 성공적으로 완료되었습니다.\n");
            printf("저장 경로: %s\n", dest_path);

            // 최종 확인
            if (check_file_exists(dest_path) == 0) {
                printf("복사한 파일을 확인했습니다: %s\n", dest_path);
            } else {
                fprintf(stderr, "복사된 파일이 확인되지 않습니다.\n");
            }
        } else {
            perror("파일 복사 실패");
        }
    } else {
        fprintf(stderr, "파일이 존재하지 않거나 접근할 수 없습니다.\n");
    }

    return;
}
