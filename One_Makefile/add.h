#ifndef ADD_H
#define ADD_H

#define STAGE_DIR ".mygit_stage"
#define SENDSIZE 4096

#include "global.h"

struct file_message {
    long msg_type;          // 메시지 타입 (1로 고정)
    char file_name[256];    // 파일 이름
    char file_contents[SENDSIZE]; // 파일 내용 (한 번에 읽을 최대 크기)
};

void add(int argc, char *argv[]);

#endif