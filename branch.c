#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/msg.h>
#include "process.h"

#define SHM_SIZE 4096 // 공유 메모리 크기

typedef struct msgbuf {
    long mtype;
    char mtext[1024];
} Msgbuf;

process list[100]; // process list 추가, checkout할 때, pid signal 통신 후 메시지 수신하기 위함
int cnt=0; //list 원소 개수 


//key_t는 main에서 레포 선언 시 발생한 값을 가져옵니다
void checkout_handler(int signum)
{
    pid_t target=0;
    char fifoname[512]={'\0'};
    char branchname[100]={'\0'};
    printf("Please write the name of the branch you want to check out");
    scanf("%s",branchname);
    target=Get_Branch(list,branchname); //branch name에 해당하는 pid 받기
    strcpy(fifoname,Get_FIFO_name(list,target)); //최종 보내야할 fifo file name 받기 

    /*컴파일 에러 방지용 주석 .. 실제 실행 시 없애함
    int msgid=msgget(key,0666);
    
    if (msgsnd(msgid,&fifoname,512,IPC_NOWAIT)==-1)
    {
        perror("msgsnd");
        exit(1);
    }
    */
}

void CreateBranch(const char *b_name, int shmid, void *shmaddr, const char *master_fifo_path);


int main(int argc, char *argv[]) {
    key_t repo_key;
    int shmid;
    void *shmaddr;
    char *branch_name;
    char master_fifo_path[256] = "./master_fifo"; // master FIFO 경로
    signal(SIGUSR2,checkout_handler);

    // 명령어 확인
    if (argc != 4) {
        fprintf(stderr, "Usage: %s branch -b <branch_name>\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "branch") != 0 || strcmp(argv[2], "-b") != 0) {
        fprintf(stderr, "Invalid command. Usage: %s branch -b <branch_name>\n", argv[0]);
        exit(1);
    }

    // 브랜치 이름 가져오기
    strcpy(branch_name,argv[3]);
    branch_name = argv[3];
    if (strlen(branch_name) == 0) {
        fprintf(stderr, "Branch name cannot be empty.\n");
        exit(1);
    }

    // 공유 메모리 초기화
    repo_key = ftok(argv[0], 1); // 파일 기반 키 생성
    if (repo_key == -1) {
        perror("ftok");
        exit(1);
    }

    shmid = shmget(repo_key, SHM_SIZE, IPC_CREAT | 0644);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // master 브랜치가 없으면 생성
    if (access(master_fifo_path, F_OK) == -1) {
        printf("Master branch not found. Creating master branch…\n");
        CreateBranch("master", shmid, shmaddr, master_fifo_path); // master 브랜치 생성
    }

    // master 브랜치가 존재하면 지정된 브랜치 생성
    CreateBranch(branch_name, shmid, shmaddr, master_fifo_path);

    return 0;
}


void CreateBranch(const char *b_name, int shmid, void *shmaddr, const char *master_fifo_path) {
    pid_t pid;
    char fifo_path[256];
    void *master_shmaddr;

    // master 브랜치 처리
    if (strcmp(b_name, "master") == 0) {
        
        // 공유 메모리 연결
        master_shmaddr = shmat(shmid, NULL, 0);
        if (master_shmaddr == (void *)-1) {
            perror("shmat");
            exit(1);
        }

        // 공유 메모리 초기화
        snprintf((char *)master_shmaddr, SHM_SIZE, "[Master branch initialized]\n");

        // master 브랜치용 FIFO 생성
        snprintf(fifo_path, sizeof(fifo_path), "%s", master_fifo_path);
        if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST) {
            perror("mkfifo");
            exit(1);
        }

        printf("Master branch created, connected to shared memory, and FIFO '%s' initialized.\n", fifo_path);
        return;
    }

    // 새 브랜치 처리
    snprintf(fifo_path, sizeof(fifo_path), "./%s_fifo", b_name);
    if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        unlink(fifo_path);
        exit(1);
    }

    if (pid == 0) { // 자식 프로세스: 브랜치 관리
        int master_fifo_fd, branch_fifo_fd;
        char buffer[256];

        // 브랜치 FIFO 열기
        branch_fifo_fd = open(fifo_path, O_RDONLY);
        if (branch_fifo_fd == -1) {
            perror("open branch fifo");
            exit(1);
        }

        // master FIFO 열기 (쓰기)
        master_fifo_fd = open(master_fifo_path, O_WRONLY);
        if (master_fifo_fd == -1) {
            perror("open master fifo");
            close(branch_fifo_fd);
            exit(1);
        }

        printf("[Branch: %s] Connected to master FIFO.\n", b_name);

        // 브랜치에서 데이터를 읽고 master로 전달
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            if (read(branch_fifo_fd, buffer, sizeof(buffer)) > 0) {
                printf("[Branch: %s] Received: %s\n", b_name, buffer);

                // 데이터를 master FIFO로 전달
                if (write(master_fifo_fd, buffer, strlen(buffer)) == -1) {
                    perror("write to master fifo");
                    close(branch_fifo_fd);
                    close(master_fifo_fd);
                    exit(1);
                }
            }
        }
    } else { // 부모 프로세스
        printf("Branch '%s' created with PID %d and FIFO '%s'.\n", b_name, pid, fifo_path);
        /*process list에 pid 추가*/
        strcpy(list[cnt].branch,b_name); //브랜치명 저장
        strcpy(list[cnt].fifo_file_name,fifo_path); //FIFO 파일명 저장 
        list[cnt].pid=pid;
        cnt+=1;
    }
}
