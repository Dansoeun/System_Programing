#include "pushpullheader.h"

int compare_files(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "r");
    FILE *fp2 = fopen(file2, "r");

    if (fp1 == NULL || fp2 == NULL)
    {
        perror("Failed to open files");
        return 1; // 파일 열기 실패
    }

    char str1[1024];
    char str2[1024];
    while (fgets(str1, sizeof(str1), fp1) != NULL && fgets(str2, sizeof(str2), fp2) != NULL)
    {
        // 각 줄을 비교
        if (strcmp(str1, str2) != 0)
        {
            fclose(fp1);
            fclose(fp2);
            return 1; // 파일이 다르면 1 반환
        }
    }

    fclose(fp1);
    fclose(fp2);
    return 0; // 파일이 같으면 0 반환
}
