//master 하위폴더로 [브랜치명]폴더 생성


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MASTER_FOLDER "master"

typedef struct Process 
{
    pid_t pid;
    char branch[512];
}process;


void create_folder(const char *folder_name) {
    mkdir();    //mkdir사용
}

//존재하는 folder인지 확인
int folder_exists(const char *folder_name) {
    struct stat st; //stat 활용
}


void create_branch(const char *branch_name) {

    //if(master_folder)가 없으면
    //master_folder생성
    
    //if(branch_name)이 존재하면
    //이미 존재한다고 알림
  
    //아니면 branch_name으로 폴더 생성
    //성공적으로 생성됐다고 출력.
}

int main(int argc, char *argv[]) {

    // ./git_branch -b [브랜치명]
    if (argc != 3 || strcmp(argv[1], "-b") != 0) {
        fprintf(stderr, "Usage: %s -b [branch_name]\n", argv[0]);
        return 1;
    }

    process p;

    p.pid = getpid();  //브랜치에 pid할당
    strncpy(p.branch, argv[2], sizeof(p.branch) - 1); //process.name에 name할당
    p.branch[sizeof(p.branch) - 1] = '\0';

    create_branch(p.branch);

    return 0;
}
