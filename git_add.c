#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
// #include <signal.h>
// #include <sys/shm.h>
// #include <sys/types.h>
// #include <errno.h>

#define STAGE_DIR ".mygit_stage"
#define SENDSIZE 4096

struct file_message {
    long msg_type;          // 메시지 타입 (1로 고정)
    char file_name[256];    // 파일 이름
    char file_contents[SENDSIZE]; // 파일 내용 (한 번에 읽을 최대 크기)
};

int is_absolute_path(const char *path);
int is_directory(const char *path);
void send_file_to_stage(int msqid, const char *file_path);
void handle_directory(int msqid, const char *dir_path);


void main(int argc, char *argv[])
{
	if (argc < 2){
		perror("argc\n");
	}
	int msqid = argv[0];
	const char *path = argv[1];	//파일 경로 받기
	if (is_directory(path)) {
        handle_directory(msqid, path);
    } else {
        send_file_to_stage(msqid, path);
    }	
}
int is_absolute_path(const char *path) {
    return path[0] == '/'; // 슬래시로 시작하면 절대 경로
}
int is_directory(const char *path){		//경로가 directory인지 확인
	struct stat path_stat;
	if (stat(path, &path_stat) != 0) return 0;
	return S_ISDIR(path_stat.st_mode);
}
void send_file_to_stage(int msqid, const char *file_path) {
    int msgid;
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




















//void create_stage_directory();
//void stage_file(const char *path, const char *stage_path);
//void add_to_stage(const char *path);
//void copy_file_to_stage(const char *src, const char *dest);
//void create_directories(const char *path);
//int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);



// void create_stage_directory(){
// 	struct stat st;
// 	if (stat(STAGE_DIR, &st) != 0){
// 		if (mkdir(STAGE_DIR, 0700) != 0){
// 			perror("");
// 			exit(1);
// 		}
		
// 	}
// }
// void stage_file(const char *path, const char *stage_path){//stage영역에 file저장
// 	copy_file_to_stage(path, stage_path);
// }
// void add_to_stage(const char *path){	//입력한 경로부터 재귀적으로 파일 저장
// 	if (is_directory(path)){
// 		DIR *dir = opendir(path);
// 		struct dirent *entry;
// 		char full_path[1024];
// 		char stage_full_path[1024];

// 		if (is_absolute_path(path)) {
// 			snprintf(stage_path, sizeof(stage_path), "%s%s", STAGE_DIR, path); // 절대 경로 그대로 사용
// 		} else {
// 			snprintf(stage_path, sizeof(stage_path), "%s/%s", STAGE_DIR, path); // 상대 경로 사용
// 		}
		
// 		if (!dir){
// 			perror("");
// 			return;
// 		}

// 		while ((entry = readdir(dir)) != NULL){
// 			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
// 				continue;
// 			}

// 			snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
// 			snprintf(stage_full_path, sizeof(stage_full_path), "%s/%s/%s", STAGE_DIR, path, entry->d_name);

// 			if (is_directory(full_path)){
// 				add_to_stage(full_path);
// 			}
// 			else{
// 				copy_file_to_stage(full_path, stage_full_path);
// 			}
// 		}
// 		closedir(dir);
// 	}
// 	else{
// 		char stage_path[1024];
// 		snprintf(stage_path, sizeof(stage_path), "%s/%s", STAGE_DIR, path);
// 		copy_file_to_stage(path, stage_path);
// 	}
// }
// void copy_file_to_stage(const char *src, const char *dest){
// 	//파일 복사
// 	int src_fd = open(src, O_RDONLY);
// 	if (src_fd < 0){
// 		perror("");
// 		exit(1);
// 	}

// 	char *dir_path = strdup(dest);
// 	char *last_slash = strrchr(dir_path, '/');
// 	if (last_slash){
// 		*last_slash = '\0';
// 		create_directories(dir_path);
// 	}
// 	free(dir_path);

// 	int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 	if (dest_fd < 0){
// 		perror("");
// 		close(src_fd);
// 		exit(1);
// 	}
// 	char buffer[4096];
// 	ssize_t bytes_read, bytes_written;
// 	while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0){
// 		bytes_written = write(dest_fd, buffer, bytes_read);
// 		if (bytes_written != bytes_read){
// 			perror("");
// 			close(src_fd);
// 			close(dest_fd);
// 			exit(1);
// 		}
// 	}
// 	msgsnd(msqid, buffer, SENDSIZE, 0);
// 	//printf("%s", buffer);
// 	if (bytes_read < 0){
// 		perror("");
// 	}
// 	close(src_fd);
// 	close(dest_fd);
// }
// void create_directories(const char *path) {	// 경로상의 모든 디렉터리를 생성
//     char temp_path[1024];
//     strcpy(temp_path, path);

//     // 경로를 하나씩 확인하며 디렉터리 생성
//     for (char *p = temp_path + 1; *p; p++) {
//         if (*p == '/') {
//             *p = '\0'; // 경로를 자른다.
//             mkdir(temp_path, 0700); // 디렉터리 생성 시도
//             *p = '/'; // 경로를 복구한다.
//         }
//     }

//     // 마지막 디렉터리 생성
//     mkdir(temp_path, 0700);
// }