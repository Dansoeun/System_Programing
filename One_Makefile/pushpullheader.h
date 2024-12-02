#ifndef PUSHPULLHEADER_H
#define PUSHPULLHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "add.h"

// 공유 메모리에 저장할 파일 정보 구조체
typedef struct file_info {
    char file_name[256];
    char action[5];  // "push" 또는 "pull"
    char local_dir[1024];
    char remote_dir[1024];
} FileInfo;



// 함수 선언
void copy_file(const char *origin_dir, const char *copy_dir);
int compare_files(const char *file1, const char *file2);
void push_to_remote(int argc, char *argv[]);
//void push_to_remote(const char *local_dir, const char *remote_dir, const char *file_name);
void pull_from_remote(const char *remote_dir, const char *local_dir, const char *file_name);
void pushpull_main(int argc, char *argv[]);


#endif // PUSHPULLHEADER_H
