#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "absolute_path.h"

char* get_absolute_path(const char* input_path) {
    char* abs_path = realpath(input_path, NULL);
    if (!abs_path) {
        perror("경로 변환 오류");
        exit(EXIT_FAILURE);
    }
    return abs_path;
}
