#include "pushpullheader.h"

/*
git push origin [branch name]
git pull origin [branch name]
push:add msqid에서 수신 후 파일 전송 
브랜치명에 해당하는 fifo 파일을 열어서 add 된 내용을 작성 
*/

void pull_from_remote(const char *remote_dir, const char *local_dir, const char *file_name) {
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

    // 공유 메모리에서 파일 정보 읽기
    snprintf(shm_ptr->file_name, sizeof(shm_ptr->file_name), "%s", file_name);
    snprintf(shm_ptr->action, sizeof(shm_ptr->action), "pull");
    snprintf(shm_ptr->local_dir, sizeof(shm_ptr->local_dir), "%s", local_dir);
    snprintf(shm_ptr->remote_dir, sizeof(shm_ptr->remote_dir), "%s", remote_dir);

    // 로컬 파일 경로 및 원격 파일 경로 저장
    char local_path[1024];
    char remote_path[1024];

    snprintf(local_path, sizeof(local_path), "%s/%s", local_dir, file_name);
    snprintf(remote_path, sizeof(remote_path), "%s/%s", remote_dir, file_name);

    // 로컬 파일과 원격 파일 비교 후 다르면 복사
    if (compare_files(local_path, remote_path) != 0)
    {
        // 파일이 다르면 원격 파일을 로컬로 복사
        copy_file(remote_path, local_path);

        char *local_dir_only = dirname(local_path);  // 로컬 저장소만 저장 (파일이름 빼고)
        char *remote_dir_only = dirname(remote_path); // 원격 저장소만 저장 (파일이름 빼고)

        // ANSI 코드로 배경색과 텍스트 반전
        printf("\033[7m파일 %s : %s (remote)  -> %s (local) pull 완료\033[0m\n", file_name, remote_dir_only, local_dir_only);
    }
    else
    {
        // 파일이 동일하면 pull 하지않음
        printf("파일 %s가 이미 동일합니다. pull 하지않습니다.\n", local_path);
    }

    // 공유 메모리 연결 해제
    shmdt(shm_ptr);
    // 공유 메모리 삭제
    shmctl(shmid, IPC_RMID, NULL);
}
