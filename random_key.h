#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 

#define AES_KEYLEN 128

uint8_t *random_key() 
{ 
    uint8_t *aes_key = (uint8_t *)malloc(AES_KEYLEN / 8); 
    srand(time(NULL)); 

    for (int i = 0; i < AES_KEYLEN / 16; i++) 
    { 
        aes_key[i] = rand() % 256; 
        aes_key[i + AES_KEYLEN / 16] = rand() % 256;
    } 

    return aes_key; 
}