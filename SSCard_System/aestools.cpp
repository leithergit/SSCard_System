#include "aestools.h"
#include <QDebug>
#include "qstring.h"
#include <QFile>

AesTools::AesTools(unsigned char* szKeyString, unsigned int nKeySize)
{
	m_lpAes = NULL;
	InitializePrivateKey(szKeyString, nKeySize);
}

AesTools::~AesTools()
{
	if (m_lpAes != NULL)
	{
		delete m_lpAes;
	}
}

VOID AesTools::InitializePrivateKey(unsigned char* KeyBytes, unsigned int KeySize)
{
	if (m_lpAes)
	{
		delete m_lpAes;
		m_lpAes = NULL;
	}
	m_lpAes = new Aes(KeySize, KeyBytes);
}


unsigned int AesTools::Encrypt(unsigned char* InBuffer, unsigned int InLength, unsigned char* OutBuffer)
{
	//unsigned int占4个字节
	unsigned int OutLength = 0, ExtraBytes = 0;
	if (m_lpAes == NULL || OutBuffer == NULL)
	{
		return 0;
	}

	//InBuffer是一个char类型的数组，lpCurInBuff指针指向的是InBuffer char数组的第一个元素的地址
	unsigned char* lpCurInBuff = (unsigned char*)InBuffer;
	//加密后的数据往OutBuffer中写入时，从第16个字节开始写。前面空出16个字节，用于存放密文大小和额外加密的字节个数，只用到8个字节，但为了保证字节总数是16的倍数，故预留了16个字节
	unsigned char* lpCurOutBuff = (unsigned char*)OutBuffer + 16;
	long blocknum = InLength / 16;
	long leftnum = InLength % 16;
	for (long i = 0; i < blocknum; i++)
	{
		//加密时，传入的数组必须时16个字节的
		m_lpAes->Cipher(lpCurInBuff, lpCurOutBuff);
		//每次加密16个字节，循环直至所有字节均被加密
		lpCurInBuff += 16;
		lpCurOutBuff += 16;
		OutLength += 16;
	}
	//当传入的密文的字节总数不是16的倍数时，会多余出leftnum 个字节。
	//此时，需要添加16-leftnum 个字节到mingwen中。则加密得到的密文，会多出16-leftnum 个字节。
	//这16-leftnum 个字节并不是mingwen里存在的
	if (leftnum)
	{
		unsigned char inbuff[16];
		memset(inbuff, 'X', 16);
		//经过上面的for循环，此时lpCurInBuff指针指向InBuffer数组的sizeof(InBuffer)-leftnum 位置处
		memcpy(inbuff, lpCurInBuff, leftnum);
		//此次解密，实际上是多加密了16-leftnum 个字节
		m_lpAes->Cipher(inbuff, lpCurOutBuff);
		lpCurOutBuff += 16;
		OutLength += 16;
	}
	unsigned char* bytesSize = (unsigned char*)OutBuffer;
	unsigned char* extraBytesSize = (unsigned char*)(OutBuffer + 4);

	//多加密的字节个数
	ExtraBytes = (16 - leftnum) % 16;
	//将OutLength的地址复制给bytesSize的前4个字节。即将加密后的密文大小存放在miwen的前四个字节中
	memcpy(bytesSize, &OutLength, 4);
	//将多加密的字节个数存放在第4-8个字节中
	memcpy(extraBytesSize, &ExtraBytes, 4);

	//返回的OutLength只包括密文长度，不包括outBuffer中预留用来存放outBuffer字节个数和额外多加密的字节个数的16字节。
	//即OutLength = sizeof(OutBuffer)-16
	return OutLength + 16;

}

unsigned int AesTools::Decrypt(unsigned char* InBuffer, unsigned int InLength, unsigned char* OutBuffer)
{
	//传入的InLength大小是加密时返回的OutLength+16，即outBuffer的大小
	unsigned int OutLength = 0, ExtraBytes = 0;
	if (m_lpAes == NULL || OutBuffer == NULL)
	{
		return 0;
	}
	//密文是从第16个字节开始的，故解密时，前16个字节忽略，直接从第16个字节开始解密
	unsigned char* lpCurInBuff = (unsigned char*)InBuffer + 16;
	unsigned char* lpCurOutBuff = (unsigned char*)OutBuffer;
	long blocknum = InLength / 16;
	long leftnum = InLength % 16;
	blocknum--;
	if (leftnum)
	{	//传入的密文大小必须是16的整数倍个字节
		return 0;
	}

	//每次解密16个字节，循环全部解出
	for (long i = 0; i < blocknum; i++)
	{
		m_lpAes->InvCipher(lpCurInBuff, lpCurOutBuff);
		lpCurInBuff += 16;
		lpCurOutBuff += 16;
		OutLength += 16;
	}

	//ExtraBytesSize指针指向的是InBuffer的第四个字节处
	unsigned char* ExtraBytesSize = (unsigned char*)InBuffer + 4;
	//将InBuffer的第4-8个字节中的内容复制给ExtraBytes。此时ExtraBytes代表的是加密是额外加密的字节数
	memcpy(&ExtraBytes, ExtraBytesSize, 4);
	//将额外加密的那部分内容，即ExtraBytes个字节的内容置为0
	memset(lpCurOutBuff - ExtraBytes, 0, ExtraBytes);
	return (OutLength - ExtraBytes);
}
