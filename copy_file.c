#include "pushpullheader.h"

void copy_file(const char *origin_dir, const char *copy_dir) {
    FILE *origin_fp, *copy_fp;
    char buffer[1024];
    size_t bytes;

    // 원본 파일 열기
    origin_fp = fopen(origin_dir, "rb");
    if (origin_fp == NULL)
    {
        perror("Origin file opening failed");
        return; // 원본 파일 오류
    }

    // 복사 파일 만들어서 열기
    copy_fp = fopen(copy_dir, "wb");
    if (copy_fp == NULL)
    {
        perror("copy file opening failed");
        fclose(origin_fp);
        return; // 복사 파일 오류
    }

    // 파일 복사
    while ((bytes = fread(buffer, 1, sizeof(buffer), origin_fp)) > 0)
    {
        fwrite(buffer, 1, bytes, copy_fp);
    }

    fclose(origin_fp);
    fclose(copy_fp);
}
