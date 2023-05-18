#include <iostream>
#include <cmath>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include "random_key.h"

using namespace std;

#define AES_KEYLEN 128

char Plaintext[128];//明文
char Ciphertext[128];//密文
unsigned char aes_key[16];
int n, e = 0, d;

//二进制转换
int BianaryTransform(int num, int bin_num[])
{

	int i = 0, mod = 0;

	//转换为二进制，逆向暂存temp[]数组中
	while (num != 0)
	{
		mod = num % 2;
		bin_num[i] = mod;
		num = num / 2;
		i++;
	}

	//返回二进制数的位数
	return i;
}

//反复平方求幂
long long Modular_Exonentiation(long long a, int b, int n)
{
	int c = 0, bin_num[1000];
	long long d = 1;
	int k = BianaryTransform(b, bin_num) - 1;

	for (int i = k; i >= 0; i--)
	{
		c = 2 * c;
		d = (d * d) % n;
		if (bin_num[i] == 1)
		{
			c = c + 1;
			d = (d * a) % n;
		}
	}
	return d;
}
//欧几里得扩展算法
int Exgcd(int m, int n, int& x)
{
	int x1, y1, x0, y0, y;
	x0 = 1; y0 = 0;
	x1 = 0; y1 = 1;
	x = 0; y = 1;
	int r = m % n;
	int q = (m - r) / n;
	while (r)
	{
		x = x0 - q * x1; y = y0 - q * y1;
		x0 = x1; y0 = y1;
		x1 = x; y1 = y;
		m = n; n = r; r = m % n;
		q = (m - r) / n;
	}
	return n;
}

//RSA初始化
void RSA_Initialize()
{
	n = 30607;
	int On = 30240;
	//用欧几里德扩展算法求e,d
	for (int j = 3; j < On; j += 1331)
	{
		int gcd = Exgcd(j, On, d);
		if (gcd == 1 && d > 0)
		{
			e = j;
			break;
		}

	}

}
//RSA加密
void RSA_Encrypt()
{
	int i = 0;
	for (i = 0; i < 100; i++)
		Ciphertext[i] = Modular_Exonentiation(Plaintext[i], e, n);//加密完成的密文
}
//算法初始化
void Initialize(char *aes_key)
{
	int i;
	for(int i=0;i<16;i++)
	{
		Plaintext[i]=aes_key[i];
	}      //明文传入此函数中
}

void rsa_encode(char *aes_key)
{
	
	Initialize(aes_key);
	
	while (!e)
		RSA_Initialize();
	RSA_Encrypt();
}

