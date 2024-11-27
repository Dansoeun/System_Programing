#include "pushpullheader.h"

void pushpull_main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <file_name> <push,pull> <local_dir>\n", argv[0]);
    }

    const char *file_name = argv[1];  // 파일 이름
    const char *action = argv[2];     // push 또는 pull
    const char *local_dir = argv[3];  // 로컬 디렉토리

    const char *remote_dir = "remote_repository";  // 원격저장소는 정해짐

    // 입력된 작업이 "push"일 경우
    if (strcmp(action, "push") == 0)
    {
        push_to_remote(local_dir, remote_dir, file_name);
    }
    // 입력된 작업이 "pull"일 경우
    else if (strcmp(action, "pull") == 0)
    {
        pull_from_remote(remote_dir, local_dir, file_name);
    }
    else
    {
        fprintf(stderr, "틀린 동작입니다. push나 pull을 입력하세요.\n");
    }
}