#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <utmp.h>
#include <pthread.h>
#include <unistd.h>
#include "checkout.h"
#include "pushpullheader.h"
#include "branch.h"
#include "clone.h"
#include "add.h"
#include "process.h"
#include "global.h"

process list[100];
key_t repo_key;
int cnt = 0;

// 전역 변수 정의
int msid = -1;
int isend = -1;

pthread_mutex_t mutex;
pthread_cond_t cond;

typedef struct utmp utmp;

// 현재 사용자의 터미널 정보를 저장할 구조체
typedef struct {
    char username[32];
    int is_terminal_user;
} user_info;

// 스레드 인자로 전달할 구조체
typedef struct {
    char command[100];
    user_info user;
} thread_arg;

// 안전한 문자열 비교 함수
int safe_strcmp(const char *str1, size_t len1, const char *str2) {
    char temp1[UT_NAMESIZE + 1] = {0};
    strncpy(temp1, str1, len1);
    temp1[len1] = '\0';
    return strcmp(temp1, str2);
}

// 안전한 문자열 복사 함수
void safe_strcpy(char *dest, const char *src, size_t len) {
    strncpy(dest, src, len);
    dest[len] = '\0';
}

// 현재 터미널 사용자인지 확인하는 함수
int is_terminal_user(const char* username) {
    struct utmp current_record;
    int utmpfd;
    int found = 0;
    
    if ((utmpfd = open("/var/run/utmp", O_RDONLY)) == -1) {
        perror(UTMP_FILE);
        return 0;
    }
    
    while (read(utmpfd, &current_record, sizeof(struct utmp)) == sizeof(struct utmp)) {
        if (current_record.ut_type == USER_PROCESS && 
            safe_strcmp(current_record.ut_user, UT_NAMESIZE, username) == 0) {
            found = 1;
            break;
        }
    }
    
    close(utmpfd);
    return found;
}

void set_pthread(char list[][100], int *list_idx)
{
    utmp current_record;
    int utmpfd;
    size_t rec_len=sizeof(utmp);

    if ((utmpfd=open("/var/run/utmp",O_RDONLY))==-1)
    {
        perror(UTMP_FILE);
        exit(-1);
    }

    while ((read(utmpfd,&current_record,rec_len))==rec_len)
    {
        if (current_record.ut_type!=USER_PROCESS)
            continue;
        
        safe_strcpy(list[*list_idx], current_record.ut_user, UT_NAMESIZE);
        *list_idx+=1;
    }

    close(utmpfd);
}

void Split_Command(char command[][100], char demand[], int *idx) {
    char *ptr = NULL;
    ptr = strtok(demand, " ");
    while (ptr != NULL && *idx < 5) {
        strcpy(command[*idx], ptr);
        *idx += 1;
        ptr = strtok(NULL, " ");
    }

    for (int i = *idx; i < 5; i++) {
        command[i][0] = '\0';
    }
}

void* Command_Exception(void *arg) {
    if (!arg) {
        printf("Invalid thread argument\n");
        return NULL;
    }

    thread_arg *t_arg = (thread_arg *)arg;
    
    if (!t_arg->user.is_terminal_user) {
        printf("User %s is not a terminal user. Command rejected.\n", t_arg->user.username);
        return NULL;
    }
    
    pthread_mutex_lock(&mutex);
    
    char command[5][100] = {'\0'};
    char *argv[5] = {NULL};
    int idx = 0;
    
    Split_Command(command, t_arg->command, &idx);
    
    for (int i = 0; i < idx; i++) {
        argv[i] = command[i];
    }

    if (!argv[0] || strcmp(argv[0], "git") != 0) {
        printf("usage: git [command] [option]\n");
    } else if (!argv[1]) {
        printf("usage: git [command] [option]\n");
    } else {
        if (strcmp(argv[1], "add") == 0) {
            add(idx, argv);
        } else if (strcmp(argv[1], "branch") == 0) {
            branch(idx, argv);
        } else if (strcmp(argv[1], "clone") == 0) {
            clone(idx, argv);
        } else if (strcmp(argv[1], "checkout") == 0) {
            checkout(idx, argv);
        } else if (strcmp(argv[1], "push") == 0 || strcmp(argv[1], "pull") == 0) {
            pushpull_main(idx, argv);
        } else {
            printf("usage: git [command] [option]\n");
        }
    }
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int ac, char *av[]) {
    if (ac < 1) {
        printf("Usage: %s\n", av[0]);
        return 1;
    }

    key_t repo_key;
    int shmid;
    char buf[100] = {'\0'};
    int list_idx = 0;
    pthread_t thread_list[10];
    char usrlist[100][100] = {'\0'};
    thread_arg *thread_args = malloc(10 * sizeof(thread_arg));

    if (!thread_args) {
        perror("malloc failed");
        return 1;
    }

    repo_key = ftok(av[0], 1);
    if (repo_key == -1) {
        perror("ftok failed");
        free(thread_args);
        return 1;
    }

    shmid = shmget(repo_key, 4096, IPC_CREAT | 0644);
    msid = msgget(repo_key, IPC_CREAT | 0644);

    if (msid == -1 || shmid == -1) {
        perror("IPC creation failed");
        free(thread_args);
        return 1;
    }

    printf("main msid: %d\n", msid);
    set_pthread(usrlist, &list_idx);

    // Initialize thread arguments
    for(int i = 0; i < list_idx; i++) {
        memset(&thread_args[i], 0, sizeof(thread_arg));
        safe_strcpy(thread_args[i].user.username, usrlist[i], 31);
        thread_args[i].user.is_terminal_user = is_terminal_user(usrlist[i]);
        // Don't copy command here since we don't have any command yet
    }

    while (1) {
        printf("input command\n");
        
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            break;
        }

        buf[strcspn(buf, "\n")] = '\0';
        
        char *current_user = getlogin();
        if (current_user != NULL) {
            thread_arg current_user_arg;
            memset(&current_user_arg, 0, sizeof(thread_arg));
            safe_strcpy(current_user_arg.user.username, current_user, 31);
            current_user_arg.user.is_terminal_user = is_terminal_user(current_user);
            strncpy(current_user_arg.command, buf, sizeof(current_user_arg.command) - 1);
            
            pthread_t current_thread;
            pthread_create(&current_thread, NULL, Command_Exception, &current_user_arg);
            pthread_join(current_thread, NULL);
        }
        
        sleep(1);  // Reduced sleep time for better responsiveness
    }

    free(thread_args);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}