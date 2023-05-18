#include <stdio.h>  // 标准输入输出库
#include <stdlib.h> // 标准库
#include <string.h> // 字符串库
#include <math.h>   // 数学库
#include <Windows.h>
#include <io.h>
#include "aes_define.h" // AES定义头文件，包含AES加解密的数据类型和函数声明
#include "aes_main.h"   // AES主要函数头文件，包含AES加解密的函数实现

#define AES_BLOCKLEN 16        // 定义AES块大小为16字节
#define MAX_BUFFER_LENGTH 4096 // 定义缓冲区最大长度为4096字节

// 填充函数
int padding(uint8_t *buffer, int block_size, int data_size)
{
    int pad_size = block_size - data_size % block_size;      // 计算需要填充的字节数
    memset(buffer + data_size, (uint8_t)pad_size, pad_size); // 填充字节
    return data_size + pad_size;
}

// 加密文件函数
// 加密文件函数
void encrypt_file(FILE *fp, uint8_t *key)
{
    uint8_t buffer[AES_BLOCKLEN]; // 缓冲区
    struct AES_ctx ctx;           // AES上下文
    int num_blocks, i;            // num_blocks为文件块数，i为循环计数器

    // 使用固定的iv向量
    uint8_t iv[AES_BLOCKLEN] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};

    AES_init_ctx_iv(&ctx, key, iv); // 初始化AES上下文

    // 计算文件块数
    fseek(fp, 0, SEEK_END);                                   // 将文件指针设置到文件末尾
    int file_size = ftell(fp);                                // 获取文件大小
    num_blocks = (int)ceil((double)file_size / AES_BLOCKLEN); // 计算文件块数
    fseek(fp, 0, SEEK_SET);                                   // 将文件指针设置回文件开头位置

    // 对文件中的每个AES块进行加密
    for (i = 0; i < num_blocks; i++)
    {
        fseek(fp, i * AES_BLOCKLEN, SEEK_SET);               // 将文件指针设置到当前AES块的开头
        int block_size = fread(buffer, 1, AES_BLOCKLEN, fp); // 读取当前AES块到缓冲区
        if (block_size < AES_BLOCKLEN)
        { // 如果当前块不足16字节，则进行填充
            block_size = padding(buffer, AES_BLOCKLEN, block_size);
        }
        AES_CBC_encrypt_buffer(&ctx, buffer, block_size); // 对当前AES块进行加密
        fseek(fp, i * AES_BLOCKLEN, SEEK_SET);            // 将文件指针设置到当前AES块的开头
        fwrite(buffer, 1, block_size, fp);                // 将加密后的数据写回文件
    }
    fclose(fp); // 关闭文件
}

void decrypt_file(FILE *fp, uint8_t *key)
{
    uint8_t buffer[AES_BLOCKLEN]; // 缓冲区
    struct AES_ctx ctx;           // AES上下文
    int num_blocks, i;            // num_blocks为文件块数,i为循环计数器

    // 使用固定的iv向量
    uint8_t iv[AES_BLOCKLEN] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};

    AES_init_ctx_iv(&ctx, key, iv); // 初始化AES上下文

    // 计算文件块数
    fseek(fp, 0, SEEK_END);                                   // 将文件指针设置到文件末尾
    int file_size = ftell(fp);                                // 获取文件大小
    num_blocks = (int)ceil((double)file_size / AES_BLOCKLEN); // 计算文件块数
    fseek(fp, 0, SEEK_SET);                                   // 将文件指针设置回文件开头位置

    // 对文件中的每个AES块进行解密
    for (i = 0; i < num_blocks; i++)
    {
        fseek(fp, i * AES_BLOCKLEN, SEEK_SET);               // 将文件指针设置到当前AES块的开头
        int block_size = fread(buffer, 1, AES_BLOCKLEN, fp); // 读取当前AES块到缓冲区
        AES_CBC_decrypt_buffer(&ctx, buffer, block_size);     // 对当前AES块进行解密
        
        int file_end = file_size - (num_blocks - i - 1) * AES_BLOCKLEN;// 计算未加密部分的大小
        if (i == num_blocks - 1)                             // 如果是最后一块
        {
            int pad_size = buffer[block_size - 1];           // 获取最后一个字节的填充字段值
            block_size = file_end - pad_size;                 // 计算解密后实际大小
            if (pad_size < AES_BLOCKLEN && pad_size > 0)
            {
                for (int j = 1; j <= pad_size; j++)
                {
                    if (buffer[block_size + j] != (uint8_t)pad_size)
                    {
                        block_size = AES_BLOCKLEN;           // 填充字符不正确,不进行删除
                        break; 
                    }
                }
            }
        }

        fseek(fp, i * AES_BLOCKLEN, SEEK_SET);               // 将文件指针设置到当前AES块的开头
        fwrite(buffer, 1, block_size, fp);                    // 将解密后的数据写回文件
    }

    fseek(fp, 0, SEEK_END);                                 
    int file_end = ftell(fp);
    if (file_end > file_size)
    {
        HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(fp));   // 获取文件句柄
        if (hFile != INVALID_HANDLE_VALUE)
        {
            SetEndOfFile(hFile);                              // 截断文件
        }
    }

    fclose(fp);                                              // 关闭文件 
}

// 加密文件的接口函数
void encode(char *filename, uint8_t *key)
{
    FILE *fp = fopen(filename, "rb+"); // 以读写方式打开文件
    if (fp == NULL)
    {
        printf("encode Failed to open file %s\n", filename); // 如果打开文件失败，则输出错误信息
        return;
    }
    encrypt_file(fp, key); // 对文件进行加密
    fclose(fp);            // 关闭文件
}

// 解密文件的接口函数
void decode(char *filename, uint8_t *key)
{
    FILE *fp = fopen(filename, "rb+"); // 以读写方式打开文件
    if (fp == NULL)
    {
        printf("decode Failed to open file %s\n", filename); // 如果打开文件失败，则输出错误信息
        return;
    }
    decrypt_file(fp, key); // 对文件进行解密
    fclose(fp);            // 关闭文件
}