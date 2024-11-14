#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

void stage_file(const char *path); 	//stage영역에 file저장
int is_directory(const char *path);	//경로가 directory인지 확인
void add_to_stage(const char *path);//입력한 경로부터 재귀적으로 파일 저장

int main(int argc, char *argv[])
{
	if (argc < 2){
		//error
		return 1;
	}
	
	const char *path = argv[1];	//파일 경로 받기
	add_to_stage(path);			

	return 0;
}

void stage_file(const char *path){
	//stage영역에 file copy
}

int is_directory(const char *path){
	//stat을 통해 디렉토리인지 확인
}

void add_to_stage(const char *path){
	if (is_directory(path)){
		//경로가 디렉토리일 경우 재귀적으로 하위 폴더의 파일을 add
	}
	else{
		//경로가 그냥 파일일 경우 바로 add
		stage_file(path);
	}
}