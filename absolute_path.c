#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_absolute_path(const char* input_path) {
    // 절대경로일 경우 그대로 반환
    if (input_path[0] == '/') {  // '/'로 시작하면 절대경로
        return strdup(input_path); // 경로를 복사해서 반환
    } else {
        // 절대경로가 아닌 경우 (실제로는 들어오지 않아야 함)
        fprintf(stderr, "입력 경로가 절대경로가 아닙니다: %s\n", input_path);
        exit(EXIT_FAILURE);
    }
}
