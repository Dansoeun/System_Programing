//master 하위폴더로 [브랜치명]폴더 생성

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MASTER_FOLDER "master"

struct Branch {
    pid_t pid;
    char name[100];
};

void create_folder(const char *folder_name) {
    if (mkdir(folder_name, 0777) == -1) {   //파일 생성시 권한 0777
        perror("Failed to create folder"); 
        exit(1);
    }
    printf("Created folder: %s\n", folder_name);
}

//존재하는 folder인지 확인
int folder_exists(const char *folder_name) {
    struct stat st;
    return stat(folder_name, &st) == 0 && S_ISDIR(st.st_mode);
}


void create_branch(const char *branch_name) {
    if (!folder_exists(MASTER_FOLDER)) {
        printf("Master folder does not exist. Creating master folder.\n");
        create_folder(MASTER_FOLDER);
    }
    
    char branch_path[200];
    snprintf(branch_path, sizeof(branch_path), "%s/%s", MASTER_FOLDER, branch_name);
    
    if (folder_exists(branch_path)) {
        printf("Branch '%s' already exists.\n", branch_name);
        return;
    }
    
    create_folder(branch_path);
    printf("Branch '%s' created successfully in master.\n", branch_name);
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-b") != 0) {
        fprintf(stderr, "Usage: %s -b [branch_name]\n", argv[0]);
        return 1;
    }

    struct Branch branch;
    branch.pid = getpid();  //브랜치에 pid할당
    strncpy(branch.name, argv[2], sizeof(branch.name) - 1); //branch.name에 name할당
    branch.name[sizeof(branch.name) - 1] = '\0';

    printf("Creating branch with PID: %d and name: %s\n", branch.pid, branch.name);
    create_branch(branch.name);

    return 0;
}
