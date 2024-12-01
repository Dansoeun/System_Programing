#include "pushpullheader.h"

/*
git push origin [branch name]
git pull origin [branch name]
push:add msqid에서 수신 후 파일 전송 
브랜치명에 해당하는 fifo 파일을 열어서 add 된 내용을 작성 
*/

void push_to_remote(const char *local_dir, const char *remote_dir, const char *file_name) {
    // 공유 메모리 설정
    int shmid = shmget(IPC_PRIVATE, sizeof(FileInfo), IPC_CREAT | 0644);
    if (shmid == -1)
    {
        perror("shmget");
        exit(1);
    }

    // 공유 메모리 연결
    FileInfo *shm_ptr = (FileInfo *)shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }

        int len=0;
    //메시지 수신 테스트
    /*
    while ((len=msgrcv(msqid,&msg,SENDSIZE,0,0))>0)
    {
        printf("Received Msg=%s, len=%d\n",msg.file_contents,len); //msg.contents에 다 저장됨 
    }*/

    // 로컬파일 정보 설정
    snprintf(shm_ptr->file_name, sizeof(shm_ptr->file_name), "%s", file_name);
    snprintf(shm_ptr->action, sizeof(shm_ptr->action), "push");
    snprintf(shm_ptr->local_dir, sizeof(shm_ptr->local_dir), "%s", local_dir);
    snprintf(shm_ptr->remote_dir, sizeof(shm_ptr->remote_dir), "%s", remote_dir);

    // 로컬 파일 경로 및 원격 파일 경로 저장
    char local_path[1024];
    char remote_path[1024];

    snprintf(local_path, sizeof(local_path), "%s/%s", local_dir, file_name);
    snprintf(remote_path, sizeof(remote_path), "%s/%s", remote_dir, file_name);

    // 파일 복사
    copy_file(local_path, remote_path);

    char *local_dir_only = dirname(local_path);  // 로컬 저장소만 저장 (파일이름 빼고)
    char *remote_dir_only = dirname(remote_path); // 원격 저장소만 저장 (파일이름 빼고)

    // ANSI 코드로 배경색과 텍스트 반전
    printf("\033[7m파일 %s : %s (local) -> %s (remote) push 완료\033[0m\n", file_name, local_dir_only, remote_dir_only);

    // 공유 메모리 연결 해제
    shmdt(shm_ptr);
    // 공유 메모리 삭제
    shmctl(shmid, IPC_RMID, NULL);
}
