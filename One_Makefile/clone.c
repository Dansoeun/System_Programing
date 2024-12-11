#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "file_copy.h" //clone_file_copy 부분
#include "check_size.h"
#include "clone_process.h"
#include "Path_setup.h"

#define PATH_MAX 4096 

void clone(int argc, char* argv[]) {
    if (argc < 3) { 
        fprintf(stderr, "사용법: %s clone <파일/디렉토리 경로> <저장 디렉토리>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Step 1: 파일 경로 처리
    char* file_path = argv[2];

    // Step 2: 저장 디렉토리 결정
    const char* default_dir = "~/clone_repos";
    const char* save_dir;
    if (argc > 3) {
        save_dir = argv[3];
    } else {
        save_dir = default_dir;
    }


    // Step 3: ~을 절대경로로 변환
    char expanded_save_dir[PATH_MAX];
    Path_setup(save_dir, expanded_save_dir, sizeof(expanded_save_dir));
    save_dir = expanded_save_dir;

    //Step 4: 파일 to 디렉토리 / 디렉토리 to 디렉토리 구현
    struct stat file_stat;
    if (stat(file_path, &file_stat) == 0) {
        // 파일 또는 디렉토리 처리
        clone_process(file_path, save_dir);
        return;
    }

    fprintf(stderr, "알 수 없는 파일 유형입니다: %s\n", file_path);
    //exit(EXIT_FAILURE);
}
