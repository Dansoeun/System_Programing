#include <sys/stat.h>
#include "file_check.h"

int check_file_exists(const char* file_path) {
    struct stat file_stat;
    return stat(file_path, &file_stat);
}
