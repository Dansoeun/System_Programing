#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>


void copy_file(const char *source, const char *destination);
void push_to_remote(const char *local_dir, const char *remote_dir, const char *file_name);
void pull_from_remote(const char *remote_dir, const char *local_dir, const char *file_name);


int main(int argc, char *argv[]) {
    

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <file_name> <push,pull> <local_dir>\n", argv[0]);
        return 1;
    }


    const char *file_name = argv[1];  // 파일 이름
    const char *action = argv[2];     // push 또는 pull
    const char *local_dir = argv[3];  // 로컬 디렉토리
    const char *remote_dir = "remote_repository";  // 원격저장소는 정해질거같아서 임의로 해놨습니다.


    // 입력된 작업이 "push"일 경우
    push_to_remote(local_dir, remote_dir, file_name);
    
    // 입력된 작업이 "pull"일 경우
    pull_from_remote(remote_dir, local_dir, file_name);


    return 1;
}



// 파일 복사 함수
void copy_file(const char *origin_dir, const char *destination_dir) {
    
    FILE *origin_fp, *dest_fp;


    // 원본 파일 열기
    origin_fp = fopen(origin_dir, "rb");
   

    // 복사 파일 만들어서 열기
    dest_fp = fopen(destination_dir, "wb");
   
    // 파일 복사

    fclose(origin_fp);
    fclose(dest_fp);
}



// push 함수: 로컬저장소 -> 원격 저장소 파일 복사
void push_to_remote(const char *local_dir, const char *remote_dir, const char *file_name) {
    
    char local_path[1024];
    char remote_path[1024];

    // 로컬 파일 경로
    snprintf(local_path, sizeof(local_path), "%s/%s", local_dir, file_name);
    // 원격 파일 경로
    snprintf(remote_path, sizeof(remote_path), "%s/%s", remote_dir, file_name);

    // 파일을 원격 저장소로 복사
    copy_file(local_path, remote_path);

    // 완료 메세지 출력 ANSI사용

}



// pull 함수: 원격 저장소 -> 로컬저장소 파일 복사
void pull_from_remote(const char *remote_dir, const char *local_dir, const char *file_name) {
    
    char local_path[1024];
    char remote_path[1024];

    // 로컬 파일 경로
    snprintf(local_path, sizeof(local_path), "%s/%s", local_dir, file_name);
    // 원격 파일 경로
    snprintf(remote_path, sizeof(remote_path), "%s/%s", remote_dir, file_name);

    // 파일을 로컬 저장소로 복사
    copy_file(remote_path, local_path);

    // 완료 메세지 출력 ANSI사용

}