//#ifndef SRC_UTILS_TAESCLASS_H
#define SRC_UTILS_TAESCLASS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "aes.h"


class AesTools
{
public:
	AesTools(unsigned char* KeyBytes, unsigned int KeySize);
	~AesTools();

	unsigned int Encrypt(unsigned char* InBuffer, unsigned int InLength, unsigned char* OutBuffer);            //AES 加密数据
	unsigned int Decrypt(unsigned char* InBuffer, unsigned int InLength, unsigned char* OutBuffer);            //AES 解密数据
	unsigned int extraBytes;
	unsigned int ciphertextLength;

private:
	void InitializePrivateKey(unsigned char* KeyBytes, unsigned int KeySize); //AES 密钥初始化
	Aes* m_lpAes;
};
