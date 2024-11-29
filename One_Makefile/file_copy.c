#include <stdio.h>
#include <stdlib.h>
#include "file_copy.h"

int clone_copy_file(const char* source_path, const char* dest_path) {
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

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(source);
    fclose(dest);
    return 0;
}
