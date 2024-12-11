#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Function to expand '~' in a given path to the user's home directory
void Path_setup(const char* input_path, char* output_path, size_t output_size) {
    if (input_path[0] == '~') {
        const char* home = getenv("HOME"); // Retrieve HOME environment variable
        if (home == NULL) {
            fprintf(stderr, "HOME 환경 변수 설정이 필요합니다.\n");
            exit(EXIT_FAILURE);
        }

        if (strlen(home) + strlen(input_path) - 1 >= output_size) { // Check if expanded path exceeds limit
            fprintf(stderr, "경로가 너무 깁니다.\n");
            exit(EXIT_FAILURE);
        }

        snprintf(output_path, output_size, "%s%s", home, input_path + 1); // Expand '~' to absolute path
    } else {
        if (strlen(input_path) >= output_size) {
            fprintf(stderr, "경로가 너무 깁니다.\n");
            exit(EXIT_FAILURE);
        }
        strncpy(output_path, input_path, output_size); // Copy input_path as-is
    }
}