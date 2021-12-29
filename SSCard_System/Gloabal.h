
#ifndef GLOABAL_H
#define GLOABAL_H
#pragma warning(disable:4100)

#include <Windows.h>
#include <WinSock2.h>

#pragma execution_character_set("utf-8")
#include <thread>
#include <chrono>
#include <iostream>
#include <errno.h>
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <string>
#include <QObject>
#include <QString>
#include <memory>
#include <QSettings>
#include <QFileInfo>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <winreg.h>
#include <string>
#include <map>
#include <algorithm>
#include <QCoreApplication>
#include <QDir>
#include <QPainter>
#include <QImage>
#include <QRect>
#include <QMessageBox>

#include "../utility/Utility.h"
#include "../utility/TimeUtility.h"
#include "../utility/AutoLock.h"
#include "../SDK/KT_Printer/KT_Printer.h"
#include "../SDK/KT_Reader/KT_Reader.h"
#include "../SDK/SSCardDriver/SSCardDriver.h"
#include "../SDK/SSCardHSM/KT_SSCardHSM.h"
#include "../SDK/SSCardInfo/KT_SSCardInfo.h"
#include "SDK/IDCard/idcard_api.h"
#include "SDK/PinKeybroad/XZ_F10_API.h"
#include "../SDK/libcurl/curl.h"


//宏定义
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)")"
#define Warning(strMessage) message( __LOC__ " Warning: " strMessage)

#define CardBox_Min  (1)
#define CardBox_Max  (8)

#include "logging.h"
#define QSucceed(x)      (x == 0)
#define QFailed(x)       (x != 0)
using namespace std;
using namespace chrono;
//using namespace Kingaotech;

extern const char* szPrinterTypeList[PRINTER_MAX];
extern const char* szReaderTypeList[READER_MAX + 1];

enum FaceDetectStatus
{
	FD_Succeed,
	FD_Binocularcamera_OpenFailed,
};

enum MaskStatus
{
	Success = 0,
	Information,
	Error,
	Failed,
	Fetal
};

enum Page_Index
{
	Page_ReaderIDCard,				//读取身份证
	Page_FaceCapture,				//读取社保卡	
	Page_EnsureInformation,			//信息确认
	Page_InputMobile,				//输入手机号码
	Page_Payment,					//支付页面
	Page_MakeCard,					//制卡页面	
	Page_ReadSSCard,				//读取社保卡	
	Page_InputSSCardPWD,			//输入社保卡密码	
	Page_ChangeSSCardPWD,			//修改社保卡密码	
	Page_RegisterLost,				//挂失 / 解挂
	Page_AdforFinance,				//开通金融页面
	Page_Succeed					//操作成功
};

enum class Manager_Level
{
	SuperVisor = 0,
	Manager = 1,
};

#define gInfo()      LOG(INFO)
#define gError()     LOG(ERROR)
#define gWarning()   LOG(WARNING)
#define Str(x)       #x
#define gVal(p)      #p<<" = "<<p <<"\t"
#define gQStr(p)	 #p<<" = "<<p.toLocal8Bit().data()<<"\t"
struct DeviceConfig
{
	DeviceConfig(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		Load(pSettings);
	}
	bool Load(QSettings* pSettings)
	{
		if (!pSettings)
			return false;
		gInfo() << "Try to read device configure";
		pSettings->beginGroup("Device");
		strPrinter = pSettings->value("Printer").toString().toStdString();
		strPrinterModule = pSettings->value("PrinterModule").toString().toStdString();
		strPrinterType = pSettings->value("PrinterType").toString().toStdString();
		transform(strPrinterType.begin(), strPrinterType.end(), strPrinterType.begin(), ::toupper);

		int nPrinterIndex = 0;
		for (auto& var : szPrinterTypeList)
		{
			if (strPrinterType == var)
			{
				nPrinterType = (PrinterType)(nPrinterIndex + 1);
				break;
			}
			nPrinterIndex++;
		}
		// 打印机驱动模块名称，如KT_Printer.dll
		strDPI = pSettings->value("PrinterDPI", "300*300").toString().toStdString();
		strPinBroadPort = pSettings->value("PinKeybroadPort").toString().toStdString();
		strPinBroadBaudrate = pSettings->value("PinKeybroadBaudrate", "9600").toUInt();
		nDepenseBox = pSettings->value("DepenseBox", 0).toUInt();
		if (nDepenseBox < CardBox_Min || nDepenseBox > CardBox_Max)
		{
			QString strInfo = QString("进卡箱号无效:%1,现重置为1").arg(nDepenseBox);
			gError() << gQStr(strInfo);
			nDepenseBox = 1;
		}

		// 制卡读卡器配置
		strReaderModule = pSettings->value("ReaderModule").toString().toStdString();				// 读卡器驱动模块名称,如KT_Reader.dll
		strSSCardReadType = pSettings->value("SSCardReadType", "DC_READER").toString().toStdString();
		int nReaderIndex = 0;
		for (auto var : szReaderTypeList)
		{
			if (strSSCardReadType == var)
			{
				nSSCardReaderType = (ReaderBrand)nReaderIndex;
				break;
			}
			nReaderIndex++;
		}
		QString strSSCardPowerOnType = pSettings->value("SSCardReaderPowerOnType", "Contact").toString();
		if (strSSCardPowerOnType == "Contact")
			nSSCardReaderPowerOnType = READER_CONTACT;
		else if (strSSCardPowerOnType == "Contactless")
			nSSCardReaderPowerOnType = READER_UNCONTACT;
		else
		{
			nSSCardReaderPowerOnType = READER_CONTACT;		// 默认为接触式
		}
		strSSCardReaderPort = pSettings->value("SSCardReaderPORT", "USB").toString().toStdString();

		// 桌面读卡器配置
		strDesktopReaderModule = pSettings->value("DesktopReaderModule").toString().toStdString();				// 读卡器驱动模块名称,如KT_Reader.dll
		strDesktopSSCardReadType = pSettings->value("DesktopSSCardReadType", "DC_READER").toString().toStdString();
		nReaderIndex = 0;
		for (auto var : szReaderTypeList)
		{
			if (strDesktopSSCardReadType == var)
			{
				nDesktopSSCardReaderType = (ReaderBrand)nReaderIndex;
				break;
			}
			nReaderIndex++;
		}

		strSSCardPowerOnType = pSettings->value("DesktopSSCardReaderPowerOnType", "Contact").toString();
		if (strSSCardPowerOnType == "Contact")
			nDesktopSSCardReaderPowerOnType = READER_CONTACT;
		else if (strSSCardPowerOnType == "Contactless")
			nDesktopSSCardReaderPowerOnType = READER_UNCONTACT;
		else
		{
			nDesktopSSCardReaderPowerOnType = READER_CONTACT;		// 默认为接触式
		}
		strDesktopSSCardReaderPort = pSettings->value("DesktopSSCardReaderPort", "USB").toString().toStdString();

		strTerminalCode = pSettings->value("TerminalCode", "").toString().toStdString();

		strIDCardReaderPort = pSettings->value("IDCardReaderPort", "USB").toString().toStdString();

		// 		Info() << "Device Cofnigure:\n";
		// 		Info() << "\t\Printer:" << strPrinter;
		// 		Info() << "\t\PrinterDPI:" << strPrinter;
		// 		Info() << "\t\PinKeybroadPort:" << strPrinter;
		// 		Info() << "\t\PinKeybroadBaudrate:" << strPrinter;
		// 		Info() << "\t\SSCardReadType:" << strPrinter;
		// 		Info() << "\t\SSCardReaderPORT:" << strPrinter;
		// 		Info() << "\t\IDCardReaderPort:" << strPrinter;
		pSettings->endGroup();
		return true;
	}
	bool Save(QSettings* pSettings)
	{
		if (!pSettings)
			return false;
		gInfo() << "Try to save device configure";
		pSettings->beginGroup("Device");
		pSettings->setValue("Printer", strPrinter.c_str());
		pSettings->setValue("PrinterModule", strPrinterModule.c_str());
		int nPrinterIndex = 0;
		for (auto& var : szPrinterTypeList)
		{
			if (strPrinterType == var)
			{
				nPrinterType = (PrinterType)(nPrinterIndex + 1);
				break;
			}
			nPrinterIndex++;
		}
		if (nPrinterType >= PRINTER_MIN && nPrinterType <= PRINTER_MAX)
			strPrinterType = szPrinterTypeList[nPrinterType];
		else
		{
			strPrinterType = "";
			QString strInfo = QString("打印机类型值无效:%1").arg(nPrinterType);
			gError() << gQStr(strInfo);
		}
		pSettings->setValue("PrinterType", strPrinterType.c_str());
		pSettings->setValue("PrinterDPI", strDPI.c_str());
		pSettings->setValue("PinKeybroadPort", strPinBroadPort.c_str());
		pSettings->setValue("PinKeybroadBaudrate", strPinBroadBaudrate.c_str());
		if (nDepenseBox < CardBox_Min || nDepenseBox > CardBox_Max)
		{
			nDepenseBox = 1;
			QString strInfo = QString("进卡箱值设置无效:%1,重置为1").arg(nDepenseBox);
			gError() << gQStr(strInfo);
		}
		pSettings->setValue("DepenseBox", nDepenseBox);

		// 制卡读卡器配置
		strReaderModule = pSettings->value("ReaderModule").toString().toStdString();
		if (nSSCardReaderType >= READER_MIN && nSSCardReaderType >= READER_MAX)
			strSSCardReadType = szReaderTypeList[nSSCardReaderType];
		else
		{
			strSSCardReadType = "";
			QString strInfo = QString("制卡读卡器类型值无效:%1").arg(nSSCardReaderType);
			gError() << gQStr(strInfo);
		}
		pSettings->setValue("SSCardReadType", strSSCardReadType.c_str());
		QString strSSCardPowerOnType = "";
		if (nSSCardReaderPowerOnType == READER_CONTACT)
			strSSCardPowerOnType = "Contact";
		else if (nSSCardReaderPowerOnType == READER_CONTACT)
			strSSCardPowerOnType = "Contactless";
		else
		{
			QString strInfo = QString("制卡读卡器上电类型值无效:%1").arg(nSSCardReaderPowerOnType);
			gError() << gQStr(strInfo);
			strSSCardPowerOnType = "";
		}
		pSettings->setValue("SSCardReaderPowerOnType", strSSCardPowerOnType);
		pSettings->setValue("SSCardReaderPort", strSSCardReaderPort.c_str());

		// 桌面读卡器配置
		pSettings->setValue("DesktopReaderModule", strDesktopReaderModule.c_str());
		if (nDesktopSSCardReaderType >= READER_MIN && nDesktopSSCardReaderType >= READER_MAX)
			strDesktopSSCardReadType = szReaderTypeList[nDesktopSSCardReaderType];
		else
		{
			strDesktopSSCardReadType = "";
			QString strInfo = QString("制卡读卡器类型值无效:%1").arg(nDesktopSSCardReaderType);
			gError() << gQStr(strInfo);
		}
		pSettings->setValue("DesktopSSCardReadType", strDesktopSSCardReadType.c_str());
		QString strDesktopSSCardPowerOnType = "";
		if (nDesktopSSCardReaderPowerOnType == READER_CONTACT)
			strDesktopSSCardPowerOnType = "Contact";
		else if (nDesktopSSCardReaderPowerOnType == READER_CONTACT)
			strDesktopSSCardPowerOnType = "Contactless";
		else
		{
			QString strInfo = QString("制卡读卡器上电类型值无效:%1").arg(nDesktopSSCardReaderPowerOnType);
			gError() << gQStr(strInfo);
			strSSCardPowerOnType = "";
		}
		pSettings->setValue("DesktopSSCardReaderPowerOnType", strDesktopSSCardPowerOnType);
		pSettings->setValue("DesktopSSCardReaderPort", strDesktopSSCardReaderPort.c_str());
		pSettings->setValue("TerminalCode", strTerminalCode.c_str());
		pSettings->setValue("IDCardReaderPort", strIDCardReaderPort.c_str());
		// 打印机驱动模块名称，如KT_Printer.dll
		// 		Info() << "Device Cofnigure:\n";
		// 		Info() << "\t\Printer:" << strPrinter;
		// 		Info() << "\t\PrinterDPI:" << strPrinter;
		// 		Info() << "\t\PinKeybroadPort:" << strPrinter;
		// 		Info() << "\t\PinKeybroadBaudrate:" << strPrinter;
		// 		Info() << "\t\SSCardReadType:" << strPrinter;
		// 		Info() << "\t\SSCardReaderPORT:" << strPrinter;
		// 		Info() << "\t\IDCardReaderPort:" << strPrinter;
		pSettings->endGroup();
		return true;
	}
	string		strPrinter;							   // 打印机名
	string		strPrinterModule;					   // 打印机驱动模块名称，如KT_Printer.dll
	PrinterType nPrinterType;                          // 打印机型号代码
	string      strPrinterType;                        // 打印机型号名
	int			nDepenseBox;						   // 发卡箱号
	string		strDPI;								   // 打印机DPI，300*300,300*600

	string		strReaderModule;					   // 制卡读卡器驱动模块名称,如KT_Reader.dll
	string		strSSCardReadType;					   // 制卡读卡器	DC || HD
	ReaderBrand nSSCardReaderType;					   // 制卡读卡器类型
	CardPowerType nSSCardReaderPowerOnType;			   // 制卡上电方式  1:接触 2 : 非接
	string		strSSCardReaderPort;				   // 制卡读卡器端口 COM1 = 0，COM2 = 1.....USB = 100

	string		strDesktopReaderModule;				   // 桌面读卡器驱动模块名称,如KT_Reader.dll
	string		strDesktopSSCardReadType;			   // 桌面读卡器	DC || HD
	ReaderBrand nDesktopSSCardReaderType;			   // 桌面读卡器类型
	CardPowerType nDesktopSSCardReaderPowerOnType;	   // 桌面上电方式  1:接触 2 : 非接
	string		strDesktopSSCardReaderPort;			   // 桌面读卡器端口 COM1 = 0，COM2 = 1.....USB = 100

	string		strPinBroadPort;					   // 密码键盘串口和波特率
	string		strPinBroadBaudrate;				   // 9600

	string		strIDCardReaderPort;				   // 身份证读卡器配置:USB, COM1, COM2...

	string		strTerminalCode;					   // 终端唯一识别码
};

struct RegionInfo
{
	RegionInfo(QSettings* pSettings)
	{
		Load(pSettings);
	}
	bool Load(QSettings* pSettings)
	{
		if (!pSettings)
			return false;
		gInfo() << "Try to read Region configure";
		pSettings->beginGroup("Region");
		strCityCode = pSettings->value("City").toString().toStdString();
		strCountry = pSettings->value("Country").toString().toStdString();
		strAgency = pSettings->value("Agency").toString().toStdString();

		strLicense = pSettings->value("License").toString().toStdString();
		strEMURL = pSettings->value("EMURL").toString().toStdString();
		strEMAccount = pSettings->value("EMAccount").toString().toStdString();							// 加密机帐号
		strEMPassword = pSettings->value("EMPassword").toString().toStdString();						// 加密机密码

		strCMURL = pSettings->value("CMURL").toString().toStdString();
		strCMAccount = pSettings->value("CMAccount", "").toString().toStdString();
		strCMPassword = pSettings->value("CMPassword", "").toString().toStdString();

		strCM_CA_Account = pSettings->value("CM_CA_Account", "").toString().toStdString();
		strCM_CA_Password = pSettings->value("CM_CA_Password", "").toString().toStdString();

		strCardVendor = pSettings->value("CardVendor", "1").toString().toStdString();
		/*
		SSCardDefaulutPin=123456
		PrimaryKey = 00112233445566778899AABBCCDDEEFF
		*/
		strSSCardDefaulutPin = pSettings->value("SSCardDefaulutPin", "").toString().toStdString();
		strPrimaryKey = pSettings->value("PrimaryKey", "").toString().toStdString();
		strBankCode = pSettings->value("BankCode", "").toString().toStdString();
		pSettings->endGroup();
		return true;
	}
	bool Save(QSettings* pSettings, bool bSupervisor = false)
	{
		if (!pSettings)
			return false;
		gInfo() << "Try to read Region configure";
		pSettings->beginGroup("Region");
		pSettings->setValue("Region", strCityCode.c_str());
		pSettings->setValue("Area", strCountry.c_str());
		//pSettings->setValue("Country", strCountry.c_str());
		pSettings->setValue("BankCode", strBankCode.c_str());
		pSettings->setValue("Agency", strAgency.c_str());
		pSettings->setValue("CardVendor", strCardVendor.c_str());
		if (bSupervisor)
		{
			pSettings->setValue("License", strLicense.c_str());
			pSettings->setValue("EMURL", strEMURL.c_str());
			pSettings->setValue("EMAccount", strEMAccount.c_str());							// 加密机帐号
			pSettings->setValue("EMPassword", strEMPassword.c_str());						// 加密机密码

			pSettings->setValue("CMURL", strCMURL.c_str());
			pSettings->setValue("CMAccount", strCMAccount.c_str());
			pSettings->setValue("CMPassword", strCMPassword.c_str());

			pSettings->setValue("CM_CA_Account", strCM_CA_Account.c_str());
			pSettings->setValue("CM_CA_Password", strCM_CA_Password.c_str());
			/*
			SSCardDefaulutPin=123456
			PrimaryKey = 00112233445566778899AABBCCDDEEFF
			*/
			pSettings->setValue("SSCardDefaulutPin", strSSCardDefaulutPin.c_str());
			pSettings->setValue("PrimaryKey", strPrimaryKey.c_str());
		}
		pSettings->endGroup();
		return true;
	}
	string		strCityCode;						    // 地市编码 410700
	string		strCountry;							    // 所属区县 410700
	string		strAgency;								// 经办机构
	string		strLicense;							    // LICENSE = STZOLdFrhbc
	string		strEMURL;							    // 加密机ip http://10.120.6.239:7777/
	string		strEMAccount;							// 加密机帐号
	string		strEMPassword;							// 加密机密码
	string		strCMURL;							    // 卡管ip   http://10.120.1.18:7001/hnCardService/services/CardService
	string		strCMAccount;							// 卡管帐号
	string		strCMPassword;							// 卡管中心密码
	string		strCM_CA_Account;						// 卡管ca帐号
	string		strCM_CA_Password;						// 卡管ca中心密码
	string		strSSCardDefaulutPin;					// 社保卡默认密码，可为作oldpin
	string		strPrimaryKey;							// 主控密钥
	string		strBankCode;							// 银行代码
	string		strCardVendor;							// 所属卡商
};

struct TextPosition
{
	string		strText;							   // 打印的内容
	string		strFontName;						   // 打印字体,例如"宋体"
	int			nFontSize;							   // 字体大小,单位为pt,1吋=72pt
	int			nColor;								   // 打印字体颜色,RGB颜色值
	int			nFontStyle;							   // 字体风格：1-常规；2-粗体；4-斜体；8-黑体
	float		fxPos;								   // 起始X坐标，单位毫米
	float		fyPos;								   // 起始Y坐标，单位毫米
	int			nAngle;								   // 旋转角度
};

struct ImagePosition
{
	string		strPath;							   // 图片全路径
	float		fxPos;								   // 起始X坐标，单位毫米
	float		fyPos;								   // 起始Y坐标，单位毫米	
	float		fWidth;								   // 打印宽度
	float		fHeight;							   // 打印高度
	int			nAngle;								   // 旋转角度
};

struct CardForm
{
	CardForm(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("CardForm");
		QString strTemp = pSettings->value("ImagePOS").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%f|%f|%d", &posImage.fxPos, &posImage.fyPos, &posImage.fWidth, &posImage.fHeight, &posImage.nAngle);

		strTemp = pSettings->value("NamePOS").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%d", &posName.fxPos, &posName.fyPos, &posName.nAngle);

		strTemp = pSettings->value("IDPOS").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%d", &posIDNumber.fxPos, &posIDNumber.fyPos, &posIDNumber.nAngle);

		strTemp = pSettings->value("CardNumberPos").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%d", &posSSCardID.fxPos, &posSSCardID.fyPos, &posSSCardID.nAngle);

		strTemp = pSettings->value("DatePos").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%d", &posIssueDate.fxPos, &posIssueDate.fyPos, &posIssueDate.nAngle);

		QString strFont = "宋体";//pSettings->value("Font").toString();
// 		char szFont[32] = { 0 };
// 		strcpy(szFont, strFont.toLocal8Bit().data());

		this->strFont = strFont.toLocal8Bit().data();
		nFontSize = pSettings->value("FontSize").toUInt();

		nFontColor = pSettings->value("FontColor").toUInt();

		nAngle = pSettings->value("Angle").toUInt();

		pSettings->endGroup();
	}
	TextPosition	posName;						   // 姓名
	ImagePosition	posImage;						   // 照片
	TextPosition	posIDNumber;					   // 身份证号
	TextPosition	posSSCardID;					   // 社保卡号
	TextPosition	posIssueDate;					   // 签发日期
	string			strFont = "宋体";				   // 字体名称
	UINT			nFontColor = 0;					   // 字体颜色
	UINT			nFontSize = 8;					   // 字体尺寸
	UINT			nAngle = 0;						   // 整体旋转角度
};

using CardFormPtr = std::shared_ptr<CardForm>;

struct PaymentOpt
{
	PaymentOpt(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		Load(pSettings);
	}

	void Load(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		gInfo() << "Try to load Payment configure";
		pSettings->beginGroup("Payment");
		nWaitTime = pSettings->value("WaitTime").toUInt();
		nQueryPayResultInterval = pSettings->value("QueryPayResultInterval", 1000).toUInt();
		nSocketRetryCount = pSettings->value("SocketRetryCount", 5).toUInt();
		nQueryPayFailedInterval = pSettings->value("nQueryPayFailedInterval", "5000").toUInt();
		strHost = pSettings->value("Host", "").toString().toStdString();
		strPort = pSettings->value("Port", "").toString().toStdString();
		strPayUrl = pSettings->value("Url", "").toString().toStdString();
		strFieldName = pSettings->value("FieldName", "").toString().toStdString();
		strFieldMobile = pSettings->value("FieldMobile", "").toString().toStdString();
		strFieldCardID = pSettings->value("FieldCard", "").toString().toStdString();
		strFiledamount = pSettings->value("Filedamount", "").toString().toStdString();
		strPayResultUrl = pSettings->value("PayResultUrl", "").toString().toStdString();
		strAmount = pSettings->value("amount", "").toString().toStdString();
		pSettings->endGroup();
	}

	bool Save(QSettings* pSettings)
	{
		return true;
	}
	int     nWaitTime = 300;                         // 支付页面等侍时间，单位秒
	int     nQueryPayResultInterval = 1000;          // 支付结构查询时间间隔单 毫秒
	int     nSocketRetryCount = 5;                   // 网络失败重试次数
	int		nQueryPayFailedInterval = 5000;
	string  strHost;
	string 	strPort;
	string 	strPayUrl;
	string	strFieldName;
	string	strFieldMobile;
	string	strFieldCardID;
	string	strFiledamount;
	string  strAmount;
	string  strPayResultUrl;
};
struct SysConfig
{
	SysConfig(QSettings* pSettings)
		: DevConfig(pSettings)
		, Region(pSettings)
		, PaymentConfig(pSettings)
	{
		if (!pSettings)
			return;
		Load(pSettings);
	}

	void Load(QSettings* pSettings)
	{
		LoadOther(pSettings);
		LoadPageTimeout(pSettings);
		LoadMaskPageTimeout(pSettings);
		LoadBanks(pSettings);
	}
	void SaveOther(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("Other");
		pSettings->setValue("BATCHMODE", nBatchMode);
		pSettings->setValue("DBPATH", strDBPath.c_str());
		pSettings->setValue("FaceSimilarity", dfFaceSimilarity);
		pSettings->setValue("MobilePhoneNumberLength", nMobilePhoneSize);
		pSettings->setValue("SSCardPasswordLength", nSSCardPasswordSize);
		pSettings->setValue("EnableDebug", bDebug);
		pSettings->endGroup();
	}
	void SavePageTimeout(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("PageTimeOut");
		pSettings->setValue("ReaderIDCard", nPageTimeout[Page_ReaderIDCard]);
		pSettings->setValue("FaceCapture", nPageTimeout[Page_FaceCapture]);
		pSettings->setValue("EnsureInformation", nPageTimeout[Page_EnsureInformation]);
		pSettings->setValue("InputMobile", nPageTimeout[Page_InputMobile]);
		pSettings->setValue("Payment", nPageTimeout[Page_Payment]);
		pSettings->setValue("MakeCard", nPageTimeout[Page_MakeCard]);
		pSettings->setValue("ReadSSCard", nPageTimeout[Page_ReadSSCard]);
		pSettings->setValue("InputSSCardPWD", nPageTimeout[Page_InputSSCardPWD]);
		pSettings->setValue("ChangeSSCardPWD", nPageTimeout[Page_ChangeSSCardPWD]);
		pSettings->setValue("RegisterLost", nPageTimeout[Page_RegisterLost]);
		pSettings->setValue("AdforFinance", nPageTimeout[Page_AdforFinance]);
		pSettings->endGroup();
	}

	void SaveMaskPageTimeout(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("MaskPageTimeout");
		pSettings->setValue("Success", nMaskTimeout[Success]);
		pSettings->setValue("Information", nMaskTimeout[Information]);
		pSettings->setValue("Error", nMaskTimeout[Error]);
		pSettings->setValue("Failed", nMaskTimeout[Failed]);
		pSettings->setValue("Fetal", nMaskTimeout[Fetal]);
		pSettings->endGroup();
	}

	void SaveBanks(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("Bank");
		for (auto var : strMapBank)
		{
			pSettings->setValue(var.first.c_str(), var.second.c_str());
		}
		pSettings->endGroup();
	}

	void LoadOther(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("Other");
		nBatchMode = pSettings->value("BATCHMODE").toInt();
		strDBPath = pSettings->value("DBPATH").toString().toStdString();
		dfFaceSimilarity = pSettings->value("FaceSimilarity").toDouble();
		nMobilePhoneSize = pSettings->value("MobilePhoneNumberLength", 11).toUInt();
		nSSCardPasswordSize = pSettings->value("SSCardPasswordLength", 6).toUInt();
		bDebug = pSettings->value("EnableDebug", false).toBool();
		pSettings->endGroup();
	}
	void LoadPageTimeout(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("PageTimeOut");
		nPageTimeout[Page_ReaderIDCard] = pSettings->value("ReaderIDCard", 30).toUInt();
		nPageTimeout[Page_FaceCapture] = pSettings->value("FaceCapture", 30).toUInt();
		nPageTimeout[Page_EnsureInformation] = pSettings->value("EnsureInformation", 30).toUInt();
		nPageTimeout[Page_InputMobile] = pSettings->value("InputMobile", 30).toUInt();
		nPageTimeout[Page_Payment] = pSettings->value("Payment", 300).toUInt();
		nPageTimeout[Page_MakeCard] = pSettings->value("MakeCard", 300).toUInt();
		nPageTimeout[Page_ReadSSCard] = pSettings->value("ReadSSCard", 30).toUInt();
		nPageTimeout[Page_InputSSCardPWD] = pSettings->value("InputSSCardPWD", 30).toUInt();
		nPageTimeout[Page_ChangeSSCardPWD] = pSettings->value("ChangeSSCardPWD", 30).toUInt();
		nPageTimeout[Page_RegisterLost] = pSettings->value("RegisterLost", 30).toUInt();
		nPageTimeout[Page_AdforFinance] = pSettings->value("AdforFinance", 30).toUInt();
		pSettings->endGroup();
	}

	void LoadMaskPageTimeout(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("MaskPageTimeout");
		nMaskTimeout[Success] = pSettings->value("Success", 1000).toUInt();
		nMaskTimeout[Information] = pSettings->value("Information", 1000).toUInt();
		nMaskTimeout[Error] = pSettings->value("Error", 2000).toUInt();
		nMaskTimeout[Failed] = pSettings->value("Failed", 5000).toUInt();
		nMaskTimeout[Fetal] = pSettings->value("Fetal", 10000).toUInt();
		pSettings->endGroup();
	}

	void LoadBanks(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("Bank");
		QStringList strKeyList = pSettings->allKeys();
		strMapBank.clear();
		for (auto var : strKeyList)
		{
			QString strValue = pSettings->value(var).toString();
			qDebug() << strValue.toStdString().c_str() << "=" << var.toStdString().c_str();
			//strMapBank.insert(pair<string, string>(var.toStdString(), strValue.toStdString()));
			auto [it, Inserted] = strMapBank.try_emplace(var.toStdString(), strValue.toStdString());
		}
		pSettings->endGroup();
	}
	DeviceConfig	DevConfig;							// 设备配置
	RegionInfo		Region;								// 区域信息配置
	PaymentOpt      PaymentConfig;						// 支付相关设置
	int				nBatchMode = 0;						// 批量制卡 开启：0    关闭：1
	string			strDBPath;							// 数据存储路径
	double          dfFaceSimilarity;					// 人脸认别最低相似度
	int             nMobilePhoneSize = 11;				// 手机号码长度
	int             nSSCardPasswordSize = 6;			// 社保卡密码长度
	int				nMaskTimeout[5];					// 各种遮罩层的逗留时间，单位毫秒
	int				nPageTimeout[16];					// 各功能页面超时时间，单位秒
	bool			bDebug;
	std::map<string, string> strMapBank;
};
using SysConfigPtr = shared_ptr<SysConfig>;

// struct SSCardInfo
// {
// 	string  strName;
// 	string  strIDNumber;
// 	string  strSSCardNumber;
// 	string  strIssuedDate;
// 	string  strPhotoPath;
// 	string  strBank;
// 	string  strStatus;
// 	void GetInfoFromIDCard(IDCardInfoPtr& pIDCardInfo)
// 	{
// 		strName = (const char*)pIDCardInfo->szName;
// 		strIDNumber = (const char*)pIDCardInfo->szIdentify;
// 	}
// };
using SSCardInfoPtr = shared_ptr<SSCardInfo>;
using IDCardInfoPtr = shared_ptr<IDCardInfo>;

class DataCenter
{
public:
	explicit DataCenter();
	~DataCenter();
	IDCardInfoPtr& GetIDCardInfo()
	{
		return pIDCard;
	}
	void SetIDCardInfo(IDCardInfoPtr& pCardInfo)
	{
		pIDCard = pCardInfo;
	}

	int LoadSysConfigure(QString& strError);

	SysConfigPtr& GetSysConfigure()
	{
		return pConfig;
	}
	int LoadCardForm(QString& strError);
	CardFormPtr& GetCardForm()
	{
		return pCardForm;
	}
	void ResetIDData()
	{
		strMobilePhone = "";
		strIDImageFile = "";
		strSSCardOldPassword = "";
		strSSCardNewPassword = "";
		strCardMakeProgress = "";
		strPayCode = "";
		pIDCard.reset();
		pSSCardInfo.reset();
	}
	SSCardInfoPtr& GetSSCardInfo()
	{
		return pSSCardInfo;
	}
	void FillSSCardWithIDCard()
	{
		if (pSSCardInfo && pIDCard)
		{
			strcpy_s(pSSCardInfo->strName, (const char*)pIDCard->szName);
			//strcpy_s(pSSCardInfo->strIDNumber = (const char*)pIDCard->szIdentify);
		}
	}

	void SetSSCardInfo(SSCardInfoPtr& pCardInfo)
	{
		pSSCardInfo = pCardInfo;
	}

	int GetBankName(string strBankCode, string& strBankName)
	{
		auto itFind = pConfig->strMapBank.find(strBankCode);
		if (itFind != pConfig->strMapBank.end())
		{
			strBankName = itFind->second;
			return 0;
		}
		else
		{
			strBankName = "";
			return 1;
		}
	}

	void SetMannagerLevel(Manager_Level nLevel)
	{
		nManagerLevel = nLevel;
	}

	Manager_Level GetManagerLevel()
	{
		return nManagerLevel;
	}
	// 	int GetCardStatus(string& strCardStatus)
	// 	{
	// 		if (!pSSCardInfo)
	// 			return -1;
	// 		if (strcmp(strupr(pSSCardInfo->strCardStatus), "OK") == 0)
	// 		{
	// 			strCardStatus = "正常";
	// 		}
	// 		else
	// 			strCardStatus = pSSCardInfo->strCardStatus;
	// 		return 0;
	// 	}
	string         strIDImageFile;
	string		   strSSCardPhotoFile;
	string         strMobilePhone;
	string         strSSCardOldPassword;
	string         strSSCardNewPassword;
	string		   strCardMakeProgress;
	string		   strPayCode;
	bool		   bDebug;
private:
	IDCardInfoPtr	pIDCard = nullptr;
	SysConfigPtr	pConfig = nullptr;
	CardFormPtr		pCardForm = nullptr;						  // 打印版式
	SSCardInfoPtr   pSSCardInfo = nullptr;
	Manager_Level	nManagerLevel = Manager_Level::Manager;
};

using DataCenterPtr = shared_ptr<DataCenter>;
extern DataCenterPtr g_pDataCenter;
bool SendHttpRequest(string szUrl, string& strRespond, string& strMessage);

struct HttpBuffer
{
	unsigned int nDataLength;
	unsigned int nBufferSize;
	FILE* fp;;
	byte* pBuffer;
	HttpBuffer(int nBufferSize = 8 * 1024, char* szFileName = nullptr)
	{
		ZeroMemory(this, sizeof(HttpBuffer));
		pBuffer = new (std::nothrow) byte[nBufferSize];
		if (pBuffer)
		{
			ZeroMemory(pBuffer, nBufferSize);
			this->nBufferSize = nBufferSize;
		}

		if (szFileName)
			fp = fopen(szFileName, "wb");
	}
	~HttpBuffer()
	{
		if (fp)
		{
			fflush(fp);
			fclose(fp);
			fp = nullptr;
		}

		if (pBuffer)
		{
			delete[]pBuffer;
			pBuffer = nullptr;
		}
	}
};

int QMessageBox_CN(QMessageBox::Icon nIcon, QString strTitle, QString strText, QMessageBox::StandardButtons stdButtons, QWidget* parent = nullptr);

class QWaitCursor
{
	bool bStart = false;
public:
	QWaitCursor();

	void RestoreCursor();

	~QWaitCursor();
};

string UTF8_GBK(const char* strUtf8);
string GBK_UTF8(const char* strGBK);
#define WaitCursor()  QWaitCursor qWait;

#endif // GLOABAL_H
