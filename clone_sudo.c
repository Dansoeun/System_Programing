/*
    상대경로일때 실행 x
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define PATH_MAX 4096 //임시로

// 함수 선언
char* get_absolute_path(const char* input_path);          // 절대 경로로 변환 (변환 안됨..)
void create_directory_if_not_exists(const char* dir_path); // 디렉터리 생성
int copy_file(const char* source_path, const char* dest_path); // 파일 복사
int check_file_exists(const char* file_path);             // 파일 존재 확인

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "사용법: %s <파일 경로> [저장 디렉터리]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Step 1: 파일 경로 처리
    char* file_path = argv[1];
    char* absolute_file_path = get_absolute_path(file_path);

    // Step 2: 저장 디렉터리 결정
    const char* default_dir = "~/clone_repos"; // 기본 저장 디렉터리(임시로)
    const char* save_dir;
    if (argc > 2) 
    {
        save_dir = argv[2];
    }    
    else 
    {
        save_dir = default_dir;
    }

    //디렉토리 만들기
    create_directory_if_not_exists(save_dir);

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

// 절대 경로로 변환
char* get_absolute_path(const char* input_path) {
    char* abs_path = realpath(input_path, NULL);
    if (!abs_path) {
        perror("경로 변환 오류");
        exit(EXIT_FAILURE);
    }
    return abs_path;
}

// 디렉터리가 없으면 생성
void create_directory_if_not_exists(const char* dir_path) {
    struct stat dir_stat;
    if (stat(dir_path, &dir_stat) != 0) { // 디렉터리가 없을 때
        printf("디렉터리가 존재하지 않습니다. %s를 생성합니다...\n", dir_path);
        if (mkdir(dir_path, 0755) != 0) { // 디렉터리 생성
            perror("디렉터리 생성 실패");
            exit(EXIT_FAILURE);
        }
    } else if (!S_ISDIR(dir_stat.st_mode)) { // 디렉터리가 아니라면 오류
        fprintf(stderr, "%s는 디렉터리가 아닙니다.\n", dir_path);
        exit(EXIT_FAILURE);
    }
}

// 파일 복사
int copy_file(const char* source_path, const char* dest_path) {

    //fopen 파일 읽기
    FILE* source = fopen(source_path, "rb");
    if (!source) {
        perror("소스 파일 열기 실패");
        return -1;
    }

    FILE* dest = fopen(dest_path, "wb");
    if (!dest) {
        perror("대상 파일 열기 실패");
        fclose(source);
        return -1;
    }
    //파일 쓰기
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }
    //파일 닫기
    fclose(source);
    fclose(dest);
    return 0;
}

// 복사된 파일이 실제로 존재하는지 확인
int check_file_exists(const char* file_path) {
    //stat 함수 이용해서 구현
    struct stat file_stat;
    return stat(file_path, &file_stat);
}
