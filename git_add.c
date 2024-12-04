#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>

#define STAGE_DIR ".mygit_stage"
#define SENDSIZE 4096

struct file_message {       // 메시지를 통해 보낼 파일 구조체
    long msg_type;          
    char file_name[256];    // 파일 이름
    char file_contents[SENDSIZE]; // 파일 내용
};

//int is_absolute_path(const char *path);
int is_directory(const char *path);                         // 경로에 있는 파일이 디렉토리인지 확인
void send_file_to_stage(int msqid, const char *file_path);  // msgsnd를 통해 파일 전송
void handle_directory(int msqid, const char *dir_path);     // 경로가 디렉토리인 경우 재귀적으로 하위 파일 전송

void add(int argc, char *argv[])
{
	if (argc < 2){
		perror("argc\n");
	}
	int msqid = argv[1]; //메세지 큐
    for (int i = 2; i < argc; i++) {  // 모든 인자를 처리하도록 루프
	    const char *path = argv[i];	//파일 경로 받기
        if (is_directory(path)) {
            handle_directory(msqid, path); // 디렉터리 처리
        } else {
            send_file_to_stage(msqid, path); // 파일 처리
        }
    }
}
// int is_absolute_path(const char *path) {
//     return path[0] == '/'; // 슬래시로 시작하면 절대 경로
// }
int is_directory(const char *path){		//경로가 directory인지 확인
	struct stat path_stat;
	if (stat(path, &path_stat) != 0) return 0;
	return S_ISDIR(path_stat.st_mode);
}

void send_file_to_stage(int msqid, const char *file_path) {
    struct file_message msg;

    // 파일 열기
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("파일 열기 실패");
        exit(1);
    }

    // 파일 이름 및 초기화
    msg.msg_type = 1; // 메시지 타입 설정
    strncpy(msg.file_name, file_path, sizeof(msg.file_name) - 1);

    // 파일 내용 읽기 및 메시지 전송
    ssize_t bytes_read;
    while ((bytes_read = read(fd, msg.file_contents, sizeof(msg.file_contents) - 1)) > 0) {
        msg.file_contents[bytes_read] = '\0'; // NULL 종료
        if (msgsnd(msqid, &msg, SENDSIZE, 0) == -1) {
            perror("메시지 전송 실패");
            close(fd);
            exit(1);
        }
    }

    close(fd);
    printf("파일 전송 완료: %s\n", file_path);
}
void handle_directory(int msqid, const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("디렉터리 열기 실패");
        exit(1);
    }

    struct dirent *entry;
    char full_path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if (is_directory(full_path)) {
            handle_directory(msqid, full_path); // 디렉터리 재귀 호출
        } else {
            send_file_to_stage(msqid, full_path); // 파일 전송
        }
    }

    closedir(dir);
}