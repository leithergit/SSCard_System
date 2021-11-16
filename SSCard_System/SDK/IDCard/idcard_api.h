#ifndef IDCARD_API_H
#define IDCARD_API_H

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define Q_DECL_EXPORT __declspec(dllexport)
#  define Q_DECL_IMPORT __declspec(dllimport)
#else
#  define Q_DECL_EXPORT     __attribute__((visibility("default")))
#  define Q_DECL_IMPORT     __attribute__((visibility("default")))
#endif

#if defined(IDCARD_API_LIBRARY)
#  define IDCARD_API_EXPORT Q_DECL_EXPORT
#else
#  define IDCARD_API_EXPORT Q_DECL_IMPORT
#endif

#include <memory.h>
#include <memory>

#ifndef     IN
#define     IN
#endif

#ifndef     OUT
#define     OUT
#endif

#ifndef     INOUT
#define     INOUT
#endif

enum SettingItem
{
	ComPort1 = 1,
	ComPort2 = 2,
	ComPort3 = 3,
	ComPort4 = 4,
	ComPort5 = 5,
	ComPort6 = 6,
	ComPort7 = 7,
	ComPort8 = 8,
	ComPort9 = 9,
	ComPort10 = 10,
	ComPort11 = 11,
	ComPort12 = 12,
	ComPort13 = 13,
	ComPort14 = 14,
	ComPort15 = 15,
	ComPort16 = 16,
	UsbPort1 = 1001,
	UsbPort2 = 1002,
	UsbPort3 = 1003,
	UsbPort4 = 1004,
	UsbPort5 = 1005,
	UsbPort6 = 1006,
	UsbPort7 = 1007,
	UsbPort8 = 1008,
	UsbPort9 = 1009,
	UsbPort10 = 1010,
	UsbPort11 = 1011,
	UsbPort12 = 1012,
	UsbPort13 = 1013,
	UsbPort14 = 1014,
	UsbPort15 = 1015,
	UsbPort16 = 1016
};

enum IDCard_Status
{
	IDCard_Succeed = 0,
	IDCard_Reader_NotOpen,
	IDCard_Invalid_Port,
	IDCard_Invalid_Parameter,
	IDCard_Library_Notload,
	IDCard_Reader_NotConnected,
	IDCard_Failed_OpenPort,
	IDCard_Failed_Find,
	IDCard_Failed_Select,
	IDCard_Failed_Read,
	IDCard_Error_InsufficentMemory,
	IDCard_Error_Unknow = 99
};

#pragma pack(push,1)
struct _FingerPrint
{
	unsigned char    szReserver[4];
	unsigned char    nFlag;                  // 必须为1，否则无效
	unsigned char    nNumber;                // 指纹序号
	unsigned char    nQuality;               // 指纹质量
	unsigned char    szData[505];
};
#pragma pack(pop)

struct IDCardInfo
{
	unsigned char     szName[32];             // 姓名
	unsigned char     szGender[8];            // 性别
	unsigned char     szNation[20];           // 民族
	unsigned char     szNationCode[20];       // 民族代码
	unsigned char     szBirthday[24];         // 出生
	unsigned char     szAddress[80];          // 户籍
	unsigned char     szIdentify[36];         // 身份证号
	unsigned char     szIszssueAuthority[32]; // 发证机关
	unsigned char     szExpirationDate1[16];  // 有效期起始日
	unsigned char     szExpirationDate2[16];  // 有效期结束日
	unsigned char     szPhoto[40 * 1024];       // 照片数据，不超过38862字节
	unsigned short    nPhotoSize;
    _FingerPrint      FingerPrint[2];

	IDCardInfo()
	{
		memset(this, 0, sizeof(IDCardInfo));
	}
	void Reset()
	{
		memset(this, 0, sizeof(IDCardInfo));
	}
};

using namespace std;
using IDCardInfoPtr = shared_ptr<IDCardInfo>;
// 打开读卡器
// szPort 设备所在端口，如COM1,COM2,USB2,USB2,若szPort为nullptr则自动检测端口（不推荐）
// 成功时返回0，否则返加相应的错误代码，GetErrorMessage可取得进一步的描述信息
int IDCARD_API_EXPORT OpenReader(IN const char* szPort = nullptr);

//关闭读卡器
void IDCARD_API_EXPORT CloseReader();

// 寻身份证
// 返回值描述参见@OpenReader
int IDCARD_API_EXPORT FindIDCard();

//读身份证
// 返回值描述参见@OpenReader
int IDCARD_API_EXPORT ReadIDCard(OUT IDCardInfo& CardInfo);

// 取得返回值或错误代码的具体描述信息
// 返回相信息描述信息的字节长度
int IDCARD_API_EXPORT GetErrorMessage(int nCode, OUT char* szMessage, int IN nBufferSize);

#endif // IDCARD_API_H
