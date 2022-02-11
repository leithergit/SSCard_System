#ifndef KT_DEFINE_H
#define KT_DEFINE_H

#include <windows.h>

enum SSCardProvince
{
	INVALID_PROVINCE = -1,
	HN_SSCARD = 1,	//河南社保
	PROVINCE_MIN = HN_SSCARD,
	SD_SSCARD,		//山东社保
	PROVINCE_MAX = SD_SSCARD,
};

struct BaseInfo
{
	char strArea[16];				// 区域代码		
	char strLicense[64];			// 授权代码
	char strEMUrl[64];				// 加密机IP
	char strAccount[64];			// 加密机账号
	char strPassword[64];			// 加密机密码
};

struct PersonInfo
{
	char strCardID[128];			//身份证号
	char strName[128];				//姓名
	char strSex[4];					//性别
	char strNation[10];				//民族
	char strBirthday[10];			//出生日期	
	char strDisOrg[64];				//初始化机构代码
	char strRarelyWord[64];			//生僻字
	char strCountry[10];			//国别 +
	char strAddress[10];			//地址 +
	char strPhoneNumber[64];		//手机号 +
};

struct TextInfo
{
	char text[256];					//文字
	float  x;						//X坐标
	float  y;						//Y坐标
	char Font[20];					//字体
	int Size;						//字号
	int Style;						//风格
};

struct PhotoInfo
{
	char strImgPath[64];			//图片路径
	float   x;						//X坐标
	float   y;						//Y坐标
	float   Width;					//打印宽度
	float   Height;					//打印高度
};

struct HSMInfo
{
	char strSystemID[10];		//业务系统ID
	char strBusinessType[4];	//业务类型	业务类型编号填写，如001,002,003
	char strTerminalCode[16];	//终端唯一识别码
	char strRegionCode[7];		//发卡地行政区划代码(取自卡识别码的前6位)
	char strCardID[19];			//社会保障号码
	char strCardNum[10];		//社会保障卡卡号
	char strIdentifyNum[33];	// 卡识别码
	char strName[51];			//姓名
	char strSex[3];				//性别
	char strNation[3];			//民族
	char strCardATR[27];		//卡复位信息
	char stralgoCode[3];		//算法标识
	char strKeyAddr[5];			//秘钥地址
	char strReleaseDate[9];		//发卡日期
	char strValidDate[9];		//有效日期
	char strBirthday[9];		//出生日期
	char strRandom1[17];		//随机数1(内部认证用)
	char strRandom2[17];		//随机数2(内部认证用)
	char strRandom3[17];		//随机数3(外部认证用)
	char strRandom4[17];		//随机数4(外部认证用)
};

//获取卡信息
struct SSCardInfo
{
	char strOrganID[21] = { 0 };		//经办机构

	char strCardNum[10] = { 0 };		//社保卡号
	char strCardID[19] = { 0 };			//身份证号
	char strBirthday[9] = { 0 };		//出生日期
	char strName[51] = { 0 };			//姓名
	char strSex[2] = { 0 };				//性别
	char strNation[3] = { 0 };			//民族
	char strMobile[21] = { 0 };			//手机号
	char strAdress[101] = { 0 };		//通讯地址
	char strPostalCode[7] = { 0 };		//邮政编码
	char strEmail[31] = { 0 };			//电子邮箱
	char strCommunity[21] = { 0 };		//所在社区
	char strCompanyName[101] = { 0 };	//单位名称
	char strGuardianName[51] = { 0 };	//监护人姓名
	char strTransType[2] = { 0 };		//申请类型
	char strCity[7] = { 0 };			//所属城市
	char strSSQX[10] = { 0 };			//所属区县
	char strBankCode[6] = { 0 };		//银行代码
	char strCityAccTime[15] = { 0 };	//市接收时间
	char strCityInfo[101] = { 0 };		//领卡地信息
	char strCardStatus[101] = { 0 };	//制卡中(或制卡完成(待分发)待领卡或卡已领取)
	char strCard[2] = { 0 };			//卡商
	char strReleaseDate[9] = { 0 };		//发卡日期
	char strValidDate[9] = { 0 };		//有效日期
	char strIdentifyNum[33] = { 0 };	//卡识别码
	char strCardATR[27] = { 0 };		//卡复位信息
	char strBankNum[27] = { 0 };		//银行卡号
	char strPCH[20] = { 0 };			//批次号
	char* strPhoto = NULL;				//相片
};

struct CAInfo
{
	char QMZS[1024];		//签名证书
	char JMZS[1024];		//加密证书
	char JMMY[1024];		//加密密钥
	char ZKMY[256];		//主控密钥
	char GLYPIN[128];	//管理员PIN
};

#endif
