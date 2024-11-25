#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "absolute_path.h"
#include "directory_check.h"
#include "file_copy.h"
#include "file_check.h"

#define PATH_MAX 4096 // 임시로

void clone(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "사용법: %s <파일 경로> [저장 디렉터리]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Step 1: 파일 경로 처리
    char* file_path = argv[1];
    char* absolute_file_path = get_absolute_path(file_path);

    // Step 2: 저장 디렉터리 결정
    const char* default_dir = "~/clone_repos"; // 기본 저장 디렉터리
    const char* save_dir = (argc > 2) ? argv[2] : default_dir;

    // 디렉터리 존재 여부 메시지 출력
    no_directory_message(save_dir);

    // Step 3: 파일 존재 여부 및 복사
    struct stat file_stat;
    if (stat(absolute_file_path, &file_stat) == 0 && (file_stat.st_mode & S_IRUSR)) {
        printf("파일이 존재합니다\n");

        char dest_path[PATH_MAX];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", save_dir, strrchr(absolute_file_path, '/') + 1);

        if (copy_file(absolute_file_path, dest_path) == 0) {
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

    free(absolute_file_path);
    return EXIT_SUCCESS;
}
