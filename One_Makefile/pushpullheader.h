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
#include <errno.h>
#include <sys/msg.h>
#include "add.h"


// 함수 선언
void copy_file(const char *origin_dir, const char *copy_dir);
int compare_files(const char *file1, const char *file2);
void push_to_remote(int argc, char *argv[]);
void pull_from_remote(int ac, char *av[]);
void pushpull_main(int argc, char *argv[]);


#endif // PUSHPULLHEADER_H
