
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
#include <fstream>
#include <sstream>
#include <ctime>
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
#include <filesystem>

#include <QCoreApplication>
#include <QDir>
#include <QPainter>
#include <QImage>
#include <QRect>
#include <QMessageBox>
#include <QScreen>
#include <QFileDialog>
#include <QCheckBox>
#include <QButtonGroup>
#include <QTableWidget>
#include <QStyleFactory>
#include <QColor>
#include <QCheckBox>
#include <QBrush>
#include <QHBoxLayout>
#include <QDirIterator>
#include <QSqlDatabase>
#include <QString>
#include <QTextCodec>
#include <QtXlsx/QtXlsx>
#include "DevBase.h"
#include "SimpleIni.h"
#include "AuthorizeMessage.h"

#include "../utility/Utility.h"
#include "../utility/TimeUtility.h"
#include "../utility/AutoLock.h"
#include "../SDK/KT_Printer/KT_Printer.h"
#include "../SDK/KT_Reader/KT_Reader.h"
#include "../SDK/SSCardDriver/SSCardDriver.h"
#include "../SDK/SSCardHSM/KT_SSCardHSM.h"
#include "../SDK/SSCardInfo_Henan/KT_SSCardInfo.h"
#include "../SDK/IDCard/idcard_api.h"
#include "../SDK/PinKeybroad/XZ_F31_API.h"
#include "../SDK/libcurl/curl.h"
#include "../SDK/7Z/include/bitarchiveinfo.hpp"
#include "../SDK/7Z/include/bitcompressor.hpp"
#include "../SDK/7Z/include/bitexception.hpp"
#include "../SDK/FaceCapture/DVTGKLDCamSDK.h"
#include "../SSCardService/SSCardService.h"
#include "../utility/json/CJsonObject.hpp"
#include "../Update/Update.h"

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
using namespace bit7z;
using namespace neb;
using namespace std::filesystem;
namespace fs = std::filesystem;
//using namespace Kingaotech;

extern const char* szPrinterTypeList[PRINTER_MAX];
extern const char* szReaderTypeList[READER_MAX + 1];
extern QScreen* g_pCurScreen;
extern HWND hTouchKeybroad;

using KT_PrinterLibPtr = shared_ptr<KTModule<KT_Printer>>;
using KT_ReaderLibPtr = shared_ptr<KTModule<KT_Reader>>;
using SSCardServiceLibPtr = shared_ptr<KTModule<SSCardService>>;

string UTF8_GBK(const char* strUtf8);
string GBK_UTF8(const char* strGBK);

enum ReaderSide
{
	ReaderInPrinter = 0,
	ReaderDesktop
};

enum class  PhotoType
{
	Photo_CardID = 0,
	Photo_SSCard,
	Photo_Base64,
	Photo_Guardian
};

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
	Fetal,
	Nop
};

enum Page_Index
{
	Page_ReaderIDCard,				// 读取身份证
	Page_InputIDCardInfo,			// 输入身份信息
	Page_FaceCapture,				// 人脸识别
	Page_EnsureInformation,			// 信息确认
	Page_InputMobile,				// 输入手机号码
	Page_Payment,					// 支付页面
	Page_MakeCard,					// 制卡页面	
	Page_ReadSSCard,				// 读取社保卡	
	Page_InputSSCardPWD,			// 输入社保卡密码	
	Page_ChangeSSCardPWD,			// 修改社保卡密码	
	Page_RegisterLost,				// 挂失/解挂
	Page_CommitNewInfo,				// 提交新办卡信息
	Page_QueryInformation,			// 信息查询	
	Page_Succeed,					// 操作成功
	Page_ModifyInfo,				// 修改个人信息
	Page_AdforFinance				// 开通金融页面
};

#define  Switch2Page(x)	(x - Page_ReaderIDCard + Switch_NextPage - 1)
enum class Manager_Level
{
	SuperVisor = 0,
	Manager = 1,
};

enum MakeCard_Progress
{
	MP_PreMakeCard = 0,
	MP_ReadCard,
	MP_WriteCard,
	MP_PrintCard,
	MP_EnableCard,
	MP_RejectCard
};

#define gInfo()      LOG(INFO)
#define gError()     LOG(ERROR)
#define gWarning()   LOG(WARNING)
#define Str(x)       #x
#define gVal(p)      #p<<" = "<<p <<"\t"
#define gQStr(p)	 #p<<" = "<<p.toLocal8Bit().data()<<"\t"
#define GBKString(x) QString(x).toLocal8Bit().data()

int WINAPI LDCam_EventCallback(int event_id, PVOID context, int iFrameStatus);
int WINAPI LDCam_NIR_FrameCallback(PVOID context, PUCHAR pFrameData, int nFrameLen);
int WINAPI LDCam_VIS_FrameCallback(PVOID context, PUCHAR pFrameData, int nFrameLen);
BOOL WriteBMPFile(const char* strFileName, PUCHAR pData, int nWidth, int nHeight);

struct DeviceConfig
{
	DeviceConfig(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;
		Load(pSettings);
	}
	~DeviceConfig()
	{
	}
	bool Load(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return false;
		gInfo() << "Try to read device configure";
		strPrinter = pSettings->GetValue("Device", "Printer");
		strPrinterModule = pSettings->GetValue("Device", "PrinterModule");
		strPrinterType = pSettings->GetValue("Device", "PrinterType");
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
		strDPI = pSettings->GetValue("Device", "PrinterDPI", "300*300");
		strPinBroadPort = pSettings->GetValue("Device", "PinKeybroadPort");
		strPinBroadBaudrate = pSettings->GetValue("Device", "PinKeybroadBaudrate", "9600");
		nDepenseBox = pSettings->GetLongValue("Device", "DepenseBox", 0);
		if (nDepenseBox < CardBox_Min || nDepenseBox > CardBox_Max)
		{
			QString strInfo = QString("进卡箱号无效:%1,现重置为1").arg(nDepenseBox);
			gError() << gQStr(strInfo);
			nDepenseBox = 1;
		}
		bCheckBezel = pSettings->GetBoolValue("Device", "CheckBezel", true);					   // 是否检查出卡口有卡

		// 制卡读卡器配置
		strReaderModule = pSettings->GetValue("Device", "ReaderModule");				// 读卡器驱动模块名称,如KT_Reader.dll
		strSSCardReadType = pSettings->GetValue("Device", "SSCardReadType", "DC_READER");
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
		QString strSSCardPowerOnType = pSettings->GetValue("Device", "SSCardReaderPowerOnType", "Contact");
		if (strSSCardPowerOnType == "Contact")
			nSSCardReaderPowerOnType = READER_CONTACT;
		else if (strSSCardPowerOnType == "Contactless")
			nSSCardReaderPowerOnType = READER_UNCONTACT;
		else
		{
			nSSCardReaderPowerOnType = READER_CONTACT;		// 默认为接触式
		}
		strSSCardReaderPort = pSettings->GetValue("Device", "SSCardReaderPORT", "USB");

		// 桌面读卡器配置
		strDesktopReaderModule = pSettings->GetValue("Device", "DesktopReaderModule");				// 读卡器驱动模块名称,如KT_Reader.dll
		strDesktopSSCardReadType = pSettings->GetValue("Device", "DesktopSSCardReadType", "DC_READER");
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

		strSSCardPowerOnType = pSettings->GetValue("Device", "DesktopSSCardReaderPowerOnType", "Contact");
		if (strSSCardPowerOnType == "Contact")
			nDesktopSSCardReaderPowerOnType = READER_CONTACT;
		else if (strSSCardPowerOnType == "Contactless")
			nDesktopSSCardReaderPowerOnType = READER_UNCONTACT;
		else
			nDesktopSSCardReaderPowerOnType = READER_CONTACT;		// 默认为接触式

		strDesktopSSCardReaderPort = pSettings->GetValue("Device", "DesktopSSCardReaderPort", "USB");

		strTerminalCode = pSettings->GetValue("Device", "TerminalCode", "");

		QString strPrinterCommand;
		strPrinterCommand = pSettings->GetValue("Device", "PrinterCommmand", "");
		QStringList cmdList = strPrinterCommand.split("|");
		for (auto var : cmdList)
			vecPrinterCommand.push_back(var.toStdString());

		strIDCardReaderPort = pSettings->GetValue("Device", "IDCardReaderPort", "USB");
		if (strIDCardReaderPort.size())
			transform(strIDCardReaderPort.begin(), strIDCardReaderPort.end(), strIDCardReaderPort.begin(), ::toupper);

		// 		Info() << "Device Cofnigure:\n";
		// 		Info() << "\t\Printer:" << strPrinter;
		// 		Info() << "\t\PrinterDPI:" << strPrinter;
		// 		Info() << "\t\PinKeybroadPort:" << strPrinter;
		// 		Info() << "\t\PinKeybroadBaudrate:" << strPrinter;
		// 		Info() << "\t\SSCardReadType:" << strPrinter;
		// 		Info() << "\t\SSCardReaderPORT:" << strPrinter;
		// 		Info() << "\t\IDCardReaderPort:" << strPrinter;
		return true;
	}

	bool Save(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return false;
		gInfo() << "Try to save device configure";
		pSettings->SetValue("Device", "Printer", strPrinter.c_str());
		pSettings->SetValue("Device", "PrinterModule", strPrinterModule.c_str());
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
		pSettings->SetValue("Device", "PrinterType", strPrinterType.c_str());
		pSettings->SetValue("Device", "PrinterDPI", strDPI.c_str());
		pSettings->SetValue("Device", "PinKeybroadPort", strPinBroadPort.c_str());
		pSettings->SetValue("Device", "PinKeybroadBaudrate", strPinBroadBaudrate.c_str());
		if (nDepenseBox < CardBox_Min || nDepenseBox > CardBox_Max)
		{
			nDepenseBox = 1;
			QString strInfo = QString("进卡箱值设置无效:%1,重置为1").arg(nDepenseBox);
			gError() << gQStr(strInfo);
		}
		pSettings->SetLongValue("Device", "DepenseBox", nDepenseBox);

		// 制卡读卡器配置
		strReaderModule = pSettings->GetValue("Device", "ReaderModule");
		if (nSSCardReaderType >= READER_MIN && nSSCardReaderType >= READER_MAX)
			strSSCardReadType = szReaderTypeList[nSSCardReaderType];
		else
		{
			strSSCardReadType = "";
			QString strInfo = QString("制卡读卡器类型值无效:%1").arg(nSSCardReaderType);
			gError() << gQStr(strInfo);
		}
		pSettings->SetValue("Device", "SSCardReadType", strSSCardReadType.c_str());
		string strSSCardPowerOnType = "";
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
		pSettings->SetValue("Device", "SSCardReaderPowerOnType", strSSCardPowerOnType.c_str());
		pSettings->SetValue("Device", "SSCardReaderPort", strSSCardReaderPort.c_str());

		// 桌面读卡器配置
		pSettings->SetValue("Device", "DesktopReaderModule", strDesktopReaderModule.c_str());
		if (nDesktopSSCardReaderType >= READER_MIN && nDesktopSSCardReaderType >= READER_MAX)
			strDesktopSSCardReadType = szReaderTypeList[nDesktopSSCardReaderType];
		else
		{
			strDesktopSSCardReadType = "";
			QString strInfo = QString("制卡读卡器类型值无效:%1").arg(nDesktopSSCardReaderType);
			gError() << gQStr(strInfo);
		}
		pSettings->SetValue("Device", "DesktopSSCardReadType", strDesktopSSCardReadType.c_str());
		string strDesktopSSCardPowerOnType = "";
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
		pSettings->SetValue("Device", "DesktopSSCardReaderPowerOnType", strDesktopSSCardPowerOnType.c_str());
		pSettings->SetValue("Device", "DesktopSSCardReaderPort", strDesktopSSCardReaderPort.c_str());
		pSettings->SetValue("Device", "TerminalCode", strTerminalCode.c_str());
		pSettings->SetValue("Device", "IDCardReaderPort", strIDCardReaderPort.c_str());

		// 打印机驱动模块名称，如KT_Printer.dll
		// 		Info() << "Device Cofnigure:\n";
		// 		Info() << "\t\Printer:" << strPrinter;
		// 		Info() << "\t\PrinterDPI:" << strPrinter;
		// 		Info() << "\t\PinKeybroadPort:" << strPrinter;
		// 		Info() << "\t\PinKeybroadBaudrate:" << strPrinter;
		// 		Info() << "\t\SSCardReadType:" << strPrinter;
		// 		Info() << "\t\SSCardReaderPORT:" << strPrinter;
		// 		Info() << "\t\IDCardReaderPort:" << strPrinter;
		return true;
	}
	string		strPrinter;							   // 打印机名
	string		strPrinterModule;					   // 打印机驱动模块名称，如KT_Printer.dll
	PrinterType nPrinterType;                          // 打印机型号代码
	string      strPrinterType;                        // 打印机型号名
	int			nDepenseBox;						   // 发卡箱号
	bool		bCheckBezel = true;						   // 是否检查出卡口有卡
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

	vector<string> vecPrinterCommand;					// 打印机必须的一些初始化指令
public:

private:

};

struct RegionInfo
{
	RegionInfo(CSimpleIniA* pSettings)
	{
		Load(pSettings);
	}
	bool Load(CSimpleIniA* pSettings)
	{
		auto convertToUtf8 = [](const QString& gbkstr)
		{
			QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
			QTextCodec* utf8Codec = QTextCodec::codecForName("UTF-8");

			QByteArray gbkData = gbkCodec->fromUnicode(gbkstr);
			QByteArray utf8Data = utf8Codec->fromUnicode(gbkstr);

			return utf8Codec->toUnicode(utf8Data);
		};
		auto utf8ToGbk = [](const std::string& utf8String)
		{
			QString qstr = QString::fromStdString(utf8String);
			return QTextCodec::codecForName("GBK")->fromUnicode(qstr).toStdString();
		};
		if (!pSettings)
			return false;
		gInfo() << "Try to read Region configure";
		strCityCode		 = pSettings->GetValue("Region", "City");
		strCountry		 = pSettings->GetValue("Region", "Country");
		strAgency		 = pSettings->GetValue("Region", "Agency");

		strLicense		 = pSettings->GetValue("Region", "EMLicense");
		strEMURL		 = pSettings->GetValue("Region", "EMURL");
		strEMAccount	 = pSettings->GetValue("Region", "EMAccount");							// 加密机帐号
		strEMPassword	 = pSettings->GetValue("Region", "EMPassword");						// 加密机密码
		strEMLicense	 = pSettings->GetValue("Region", "EMLicense");						// 加密机密码

		strCMURL		 = pSettings->GetValue("Region", "CMURL");
		strCMAccount	 = pSettings->GetValue("Region", "CMAccount", "");
		strCMPassword	 = pSettings->GetValue("Region", "CMPassword", "");

		strCM_CA_Account = pSettings->GetValue("Region", "CM_CA_Account", "");
		strCM_CA_Password= pSettings->GetValue("Region", "CM_CA_Password", "");

		strCardVendor	 = pSettings->GetValue("Region", "CardVendor", "1");

		strJJRName		 = pSettings->GetValue("Region", "JJR_Name");
		strJJRName		 = utf8ToGbk(strJJRName);
		//strJJRName		 = QString(strJJRName.c_str()).toUtf8();
		strJJRMobile	 = pSettings->GetValue("Region", "JJR_Mobile");
		strJJRId		 = pSettings->GetValue("Region", "JJR_ID");

		strBankNodeName	 = pSettings->GetValue("Region", "BankNodeName");
		//
		strBankNodeName = utf8ToGbk(strBankNodeName);
		//strBankNodeName = QString(strBankNodeName.c_str()).toUtf8();
		/*
		SSCardDefaulutPin=123456
		PrimaryKey = 00112233445566778899AABBCCDDEEFF
		*/
		strSSCardDefaulutPin		 = pSettings->GetValue("Region", "SSCardDefaulutPin", "");
		strPrimaryKey				 = pSettings->GetValue("Region", "PrimaryKey", "");
		strBankCode					 = pSettings->GetValue("Region", "BankCode", "");
		strSSCardSeriveModule		 = pSettings->GetValue("Region", "SSCardServiceModule", "");
		strSSCardServiceDescription	 = pSettings->GetValue("Region", "SSCardServiceDescription", "");
		nProvinceCode				 = (SSCardProvince)pSettings->GetLongValue("Region", "ProvinceCode");
		strOperator					 = pSettings->GetValue("Region", "Operator");
		return true;
	}
	bool Save(CSimpleIniA* pSettings, bool bSupervisor = false)
	{

		if (!pSettings)
			return false;
		gInfo() << "Try to read Region configure";

		pSettings->SetValue("Region", "City", strCityCode.c_str());
		pSettings->SetValue("Region", "Area", strCountry.c_str());
		//pSettings->SetValue("Country", strCountry.c_str());
		pSettings->SetValue("Region", "BankCode", strBankCode.c_str());
		pSettings->SetValue("Region", "Agency", strAgency.c_str());
		pSettings->SetValue("Region", "CardVendor", strCardVendor.c_str());
		if (bSupervisor)
		{
			pSettings->SetValue("Region", "License", strLicense.c_str());
			pSettings->SetValue("Region", "EMURL", strEMURL.c_str());
			pSettings->SetValue("Region", "EMAccount", strEMAccount.c_str());							// 加密机帐号
			pSettings->SetValue("Region", "EMPassword", strEMPassword.c_str());						// 加密机密码

			pSettings->SetValue("Region", "CMURL", strCMURL.c_str());
			pSettings->SetValue("Region", "CMAccount", strCMAccount.c_str());
			pSettings->SetValue("Region", "CMPassword", strCMPassword.c_str());

			pSettings->SetValue("Region", "CM_CA_Account", strCM_CA_Account.c_str());
			pSettings->SetValue("Region", "CM_CA_Password", strCM_CA_Password.c_str());
			/*
			SSCardDefaulutPin=123456
			PrimaryKey = 00112233445566778899AABBCCDDEEFF
			*/
			pSettings->SetValue("Region", "SSCardDefaulutPin", strSSCardDefaulutPin.c_str());
			pSettings->SetValue("Region", "PrimaryKey", strPrimaryKey.c_str());
		}
		pSettings->SetValue("Region", "JJR_Name", strJJRName.c_str());

		pSettings->SetValue("Region", "JJR_Mobile", strJJRMobile.c_str());
		pSettings->SetValue("Region", "JJR_ID", strJJRId.c_str());
		
		strBankNodeName = pSettings->SetValue("Region", "BankNodeName", strBankNodeName.c_str());
		
		return true;
	}
	string		strCityCode;						    // 地市编码 410700
	string		strCountry;							    // 所属区县 410700
	string		strAgency;								// 经办机构
	string		strLicense;							    // LICENSE = STZOLdFrhbc
	string		strEMURL;							    // 加密机ip http://10.120.6.239:7777/
	string		strEMAccount;							// 加密机帐号
	string		strEMPassword;							// 加密机密码
	string		strEMLicense;							// 加密机密码
	string		strCMURL;							    // 卡管ip   http://10.120.1.18:7001/hnCardService/services/CardService
	string		strCMAccount;							// 卡管帐号
	string		strCMPassword;							// 卡管中心密码
	string		strCM_CA_Account;						// 卡管ca帐号
	string		strCM_CA_Password;						// 卡管ca中心密码
	string		strSSCardDefaulutPin;					// 社保卡默认密码，可为作oldpin
	string		strPrimaryKey;							// 主控密钥
	string		strBankCode;							// 银行代码
	string		strCardVendor;							// 所属卡商
	string		strSSCardSeriveModule;					// 社保卡服务模块
	string		strSSCardServiceDescription;			// 社保卡服务描述文件，用于服务初始化
	string		strOperator;							// 操作人员
	string		strJJRName;								// 经办人名称
	string		strJJRMobile;							// 经办人联系方式
	string		strJJRId;								// 经办人身份证号
	string		strBankNodeName;						// 银行网点名称
	SSCardProvince	nProvinceCode;						// 省市代码
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
	CardForm()
	{}
	CardForm(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;

		string strTemp = pSettings->GetValue("CardForm", "ImagePOS");
		sscanf_s(strTemp.c_str(), "%f|%f|%f|%f|%d", &posImage.fxPos, &posImage.fyPos, &posImage.fWidth, &posImage.fHeight, &posImage.nAngle);

		strTemp = pSettings->GetValue("CardForm", "NamePOS");
		sscanf_s(strTemp.c_str(), "%f|%f|%d", &posName.fxPos, &posName.fyPos, &posName.nAngle);

		strTemp = pSettings->GetValue("CardForm", "IDPOS");
		sscanf_s(strTemp.c_str(), "%f|%f|%d", &posIDNumber.fxPos, &posIDNumber.fyPos, &posIDNumber.nAngle);

		strTemp = pSettings->GetValue("CardForm", "CardNumberPos");
		sscanf_s(strTemp.c_str(), "%f|%f|%d", &posSSCardID.fxPos, &posSSCardID.fyPos, &posSSCardID.nAngle);

		strTemp = pSettings->GetValue("CardForm", "DatePos");
		sscanf_s(strTemp.c_str(), "%f|%f|%d", &posIssueDate.fxPos, &posIssueDate.fyPos, &posIssueDate.nAngle);

		QString strFont = "宋体";//pSettings->GetValue("Font").toString();
// 		char szFont[32] = { 0 };
// 		strcpy(szFont, strFont.toLocal8Bit().data());

		this->strFont = strFont.toLocal8Bit().data();
		nFontSize = pSettings->GetLongValue("CardForm", "FontSize");

		nFontColor = pSettings->GetLongValue("CardForm", "FontColor");

		nAngle = pSettings->GetLongValue("CardForm", "Angle");

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
	PaymentOpt(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;
		Load(pSettings);
	}

	void Load(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;
		gInfo() << "Try to load Payment configure";

		nWaitTime				 = pSettings->GetLongValue("Payment", "WaitTime");
		nQueryPayResultInterval	 = pSettings->GetLongValue("Payment", "QueryPayResultInterval", 1000);
		nSocketRetryCount		 = pSettings->GetLongValue("Payment", "SocketRetryCount", 5);
		nQueryPayFailedInterval	 = pSettings->GetLongValue("Payment", "nQueryPayFailedInterval", 5000);
		strHost					 = pSettings->GetValue("Payment", "Host", "");
		strPort					 = pSettings->GetValue("Payment", "Port", "");
		strPayUrl				 = pSettings->GetValue("Payment", "Url", "");
		strFieldName			 = pSettings->GetValue("Payment", "FieldName", "");
		strFieldMobile			 = pSettings->GetValue("Payment", "FieldMobile", "");
		strFieldCardID			 = pSettings->GetValue("Payment", "FieldCard", "");
		strFiledamount			 = pSettings->GetValue("Payment", "Filedamount", "");
		strPayResultUrl			 = pSettings->GetValue("Payment", "PayResultUrl", "");
		strAmount				 = pSettings->GetValue("Payment", "amount", "");
	}

	bool Save(CSimpleIniA* pSettings)
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
	SysConfig(CSimpleIniA* pSettings)
		: DevConfig(pSettings)
		, Region(pSettings)
		, PaymentConfig(pSettings)
	{
		if (!pSettings)
			return;
		Load(pSettings);
	}

	void Load(CSimpleIniA* pSettings)
	{
		LoadOthers(pSettings);
		LoadPageTimeout(pSettings);
		LoadMaskPageTimeout(pSettings);
		LoadBanks(pSettings);
	}

	void SavePageTimeout(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;

		pSettings->SetLongValue("PageTimeOut", "ReaderIDCard", nPageTimeout[Page_ReaderIDCard]);
		pSettings->SetLongValue("PageTimeOut", "FaceCapture", nPageTimeout[Page_FaceCapture]);
		pSettings->SetLongValue("PageTimeOut", "EnsureInformation", nPageTimeout[Page_EnsureInformation]);
		pSettings->SetLongValue("PageTimeOut", "InputMobile", nPageTimeout[Page_InputMobile]);
		pSettings->SetLongValue("PageTimeOut", "Payment", nPageTimeout[Page_Payment]);
		pSettings->SetLongValue("PageTimeOut", "MakeCard", nPageTimeout[Page_MakeCard]);
		pSettings->SetLongValue("PageTimeOut", "ReadSSCard", nPageTimeout[Page_ReadSSCard]);
		pSettings->SetLongValue("PageTimeOut", "InputSSCardPWD", nPageTimeout[Page_InputSSCardPWD]);
		pSettings->SetLongValue("PageTimeOut", "ChangeSSCardPWD", nPageTimeout[Page_ChangeSSCardPWD]);
		pSettings->SetLongValue("PageTimeOut", "RegisterLost", nPageTimeout[Page_RegisterLost]);
		pSettings->SetLongValue("PageTimeOut", "AdforFinance", nPageTimeout[Page_AdforFinance]);
		pSettings->SetLongValue("PageTimeOut", "ModifyPersonInfo", nPageTimeout[Page_ModifyInfo]);
	}

	void SaveMaskPageTimeout(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;

		pSettings->SetLongValue("MaskPageTimeout", "Success", nMaskTimeout[Success]);
		pSettings->SetLongValue("MaskPageTimeout", "Information", nMaskTimeout[Information]);
		pSettings->SetLongValue("MaskPageTimeout", "Error", nMaskTimeout[Error]);
		pSettings->SetLongValue("MaskPageTimeout", "Failed", nMaskTimeout[Failed]);
		pSettings->SetLongValue("MaskPageTimeout", "Fetal", nMaskTimeout[Fetal]);

	}

	void SaveBanks(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;
		for (auto var : MapBankSupported)
		{
			pSettings->SetValue("Bank", var.first.c_str(), var.second.c_str());
		}
	}

	void SaveOthers(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;

		//nBatchMode = pSettings->GetValue("BATCHMODE").toInt();
		pSettings->SetValue("Other", "DBPATH", strDBPath.c_str());
		pSettings->SetDoubleValue("Other", "FaceSimilarity", dfFaceSimilarity);

		pSettings->SetLongValue("Other", "logUpload", bUpoadlog);
		pSettings->SetLongValue("Other", "DeltelogUploaded", bDeletelogUploaded);		// 上传成功后删除日志
		pSettings->SetLongValue("Other", "EnableDebug", nDiskFreeSpace);				// 保留磁盘空间，超过时，删除最早一天的日志
		pSettings->SetValue("Other", "logServer", strLogServer.c_str());			// 日志服务器
		pSettings->SetLongValue("Other", "logServerPort", nLogServerPort);				// 日志服务器端口
		pSettings->SetLongValue("Other", "logSavePeroid", nLogSavePeroid);				// 日志保存天数
		pSettings->SetLongValue("Other", "EnableDebug", bDebug);

	}

	void LoadOthers(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;

		//nBatchMode = pSettings->GetValue("BATCHMODE").toInt();
		strDBPath			 = pSettings->GetValue("Other", "DBPATH");
		dfFaceSimilarity	 = pSettings->GetDoubleValue("Other", "FaceSimilarity");
		nMobilePhoneSize	 = pSettings->GetLongValue("Other", "MobilePhoneNumberLength", 11);
		nSSCardPasswordSize	 = pSettings->GetLongValue("Other", "SSCardPasswordLength", 6);
		bDebug				 = pSettings->GetLongValue("Other", "EnableDebug", false);
		bEnableUpdate		 = pSettings->GetLongValue("Other", "EnableUpdate", true);
		bTestCard			 = pSettings->GetLongValue("Other", "EnalbeCardTest", false);
		bSkipWriteCard		 = pSettings->GetLongValue("Other", "SkipWriteCard", false);
		bSkipPrintCard		 = pSettings->GetLongValue("Other", "SkipPrintCard", false);
		bWriteTest			 = pSettings->GetLongValue("Other", "WriteTest", false);
		nNetTimeout			 = pSettings->GetLongValue("Other", "NetTimeout", 1500);

		bUpoadlog			 = pSettings->GetLongValue("Other", "logUpload", false);
		bDeletelogUploaded	 = pSettings->GetLongValue("Other", "DeltelogUploaded", false);		// 上传成功后删除日志
		nDiskFreeSpace		 = pSettings->GetLongValue("Other", "DiskFreeSpace", 10);            // 保留磁盘空间，超过时，删除最早一天的日志		
		nLogServerPort		 = pSettings->GetLongValue("Other", "logServerPort", 80);            // 日志服务器端口
		nLogSavePeroid		 = pSettings->GetLongValue("Other", "logSavePeroid", 30);            // 日志保存天数
		nTimeWaitForPrinter	 = pSettings->GetLongValue("Other", "TimeWaitForPrinter", 300);		// 等待打印机上电超时
		bPreSetBankCard		 = pSettings->GetLongValue("Other", "PresetBankCard", true);			// 批量制卡时预先绑定解行卡号
		strLogServer		 = pSettings->GetValue("Other", "logServer", "");					// 日志服务器

	}

	void LoadPageTimeout(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;

		nPageTimeout[Page_ReaderIDCard]		 = pSettings->GetLongValue("PageTimeOut","ReaderIDCard", 30);
		nPageTimeout[Page_InputIDCardInfo]	 = pSettings->GetLongValue("PageTimeOut","InputIDCard", 600);
		nPageTimeout[Page_FaceCapture]		 = pSettings->GetLongValue("PageTimeOut","FaceCapture", 30);
		nPageTimeout[Page_EnsureInformation] = pSettings->GetLongValue("PageTimeOut","EnsureInformation", 30);
		nPageTimeout[Page_InputMobile]		 = pSettings->GetLongValue("PageTimeOut","InputMobile", 30);
		nPageTimeout[Page_Payment]			 = pSettings->GetLongValue("PageTimeOut","Payment", 300);
		nPageTimeout[Page_MakeCard]			 = pSettings->GetLongValue("PageTimeOut","MakeCard", 300);
															 
		nPageTimeout[Page_ReadSSCard]		 = pSettings->GetLongValue("PageTimeOut","ReadSSCard", 30);
		nPageTimeout[Page_InputSSCardPWD]	 = pSettings->GetLongValue("PageTimeOut","InputSSCardPWD", 30);
		nPageTimeout[Page_ChangeSSCardPWD]	 = pSettings->GetLongValue("PageTimeOut","ChangeSSCardPWD", 30);
		nPageTimeout[Page_RegisterLost]		 = pSettings->GetLongValue("PageTimeOut","RegisterLost", 30);
		nPageTimeout[Page_QueryInformation]	 = pSettings->GetLongValue("PageTimeOut","QueryInfo", 30);
		nPageTimeout[Page_AdforFinance]		 = pSettings->GetLongValue("PageTimeOut","AdforFinance", 30);
		nPageTimeout[Page_CommitNewInfo]	 = pSettings->GetLongValue("PageTimeOut","CommitNewInfo", 30);
		nPageTimeout[Page_ModifyInfo]	 = pSettings->GetLongValue("PageTimeOut", "ModifyPersonInfo", 300);
	}

	void LoadMaskPageTimeout(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;
	
		nMaskTimeout[Success]		 = pSettings->GetLongValue("MaskPageTimeout","Success", 1000);
		nMaskTimeout[Information]	 = pSettings->GetLongValue("MaskPageTimeout","Information", 1000);
		nMaskTimeout[Error]			 = pSettings->GetLongValue("MaskPageTimeout","Error", 2000);
		nMaskTimeout[Failed]		 = pSettings->GetLongValue("MaskPageTimeout","Failed", 5000);
		nMaskTimeout[Fetal]			 = pSettings->GetLongValue("MaskPageTimeout","Fetal", 10000);
	}

	void LoadBanks(CSimpleIniA* pSettings)
	{
		if (!pSettings)
			return;
		
		CSimpleIniA::TNamesDepend AllKeys;
		pSettings->GetAllKeys("Bank", AllKeys);
		MapBankSupported.clear();
		for (auto var : AllKeys)
		{
			string strValue = pSettings->GetValue("Bank",var.pItem);
			qDebug() << strValue.c_str() << "=" << var.pItem;
			auto [it, Inserted] = MapBankSupported.try_emplace(var.pItem, strValue);
		}
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
	bool            bPageEnable[16];
	bool            bUpoadlog = false;
	bool            bDeletelogUploaded = true;          // 上传成功后删除日志
	int             nDiskFreeSpace = 10;                // 保留磁盘空间，超过时，删除最早一天的日志
	string          strLogServer = "";                  // 日志服务器
	int             nLogServerPort = 80;                // 日志服务器端口
	int             nLogSavePeroid = 30;                // 日志保存天数
	int             nTimeWaitForPrinter = 180;          // 等待打印机上电超时
	bool			bPreSetBankCard = true;				// 批量制卡时使用预置银行卡号
	bool			bDebug = false;
	bool			bTestCard = false;
	bool			bSkipWriteCard = false;
	bool			bSkipPrintCard = false;
	bool			bEnableUpdate = true;
	bool			bWriteTest = false;
	int				nNetTimeout = 1500;
	map<string, string> MapBankSupported;
};

using SysConfigPtr = shared_ptr<SysConfig>;

using SSCardBaseInfoPtr = shared_ptr<SSCardBaseInfo>;

using IDCardInfoPtr = shared_ptr<IDCardInfo>;

struct NationaltyCode
{
	string strCode;
	string strNationalty;
};

struct DocType
{
	string strCode;
	string strDocName;
};

struct HukouCode
{
	string strCode;
	string strHukou;
};

struct EductionCode
{
	string  strCode;
	string	strEducationLevel;
};

struct MarrageCode
{
	string strCode;
	string strMarriage;
};

struct GuojiCode
{
	string strCode;
	string strGuoji;
};

using ReadIDCard_CallBack = std::function<void(QWidget* pWidget, IDCardInfo* pIDCard)>;

class DataCenter
{
public:
	explicit DataCenter();
	~DataCenter();
	IDCardInfoPtr& GetIDCardInfo()
	{
		gInfo() << __FUNCTION__;
		return pIDCard;
	}
	void SetIDCardInfo(IDCardInfoPtr& pCardInfo)
	{
		gInfo() << __FUNCTION__;
		pIDCard = pCardInfo;
	}

	int LoadSysConfigure(QString& strError);

	SysConfigPtr& GetSysConfigure()
	{
		return pSysConfig;
	}
	int LoadCardForm(QString& strError);
	CardFormPtr& GetCardForm()
	{
		return pCardForm;
	}
	void ResetIDData()
	{
		gInfo() << __FUNCTION__;
		strMobilePhone = "";
		strIDImageFile = "";
		strSSCardOldPassword = "";
		strSSCardNewPassword = "";
		strCardMakeProgress = "";
		strPayCode = "";
		pIDCard.reset();
		pSSCardInfo.reset();
		strSSCardPhotoFile = "";
		strIDImageFile = "";
		strSSCardPhotoBase64File = "";
		bGuardian = false;
		bWithoutIDCard = false;
		strGuardianPhotoFile = "";
	}
	SSCardBaseInfoPtr& GetSSCardInfo()
	{
		return pSSCardInfo;
	}
	void FillSSCardWithIDCard()
	{
		if (pSSCardInfo && pIDCard)
		{
			pSSCardInfo->strName = (const char*)pIDCard->szName;
			//strcpy_s(pSSCardInfo->strIDNumber = (const char*)pIDCard->szIdentify);
		}
	}

	void SetSSCardInfo(SSCardBaseInfoPtr& pCardInfo)
	{
		pSSCardInfo = pCardInfo;
	}

	int GetBankName(string strBankCode, string& strBankName)
	{
		auto itFind = pSysConfig->MapBankSupported.find(strBankCode);
		if (itFind != pSysConfig->MapBankSupported.end())
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
	string			strIDImageFile;
	string			strSSCardPhotoFile;
	string			strGuardianPhotoFile;
	string			strSSCardPhotoBase64File;
	string			strMobilePhone;
	string			strSSCardOldPassword;
	string			strSSCardNewPassword;
	string			strCardMakeProgress;
	CardStatus		nCardStratus;
	ServiceType		nCardServiceType = ServiceType::Service_Unknown;
	string			strPayCode;
	string			strCardVersion;
	QSqlDatabase	SQLiteDB;
	bool			bGuardian;	// 启用监护人
	bool			bSwitchBank = false;// 跨行换卡
	bool			bDebug;
	bool			bEnableUpdate = true;
	bool			bSkipWriteCard = false;
	bool			bSkipPrintCard = false;
	bool            bWriteTest = false;
	bool			bTestCard = false;
	bool			bWithoutIDCard = false;
	bool			bBatchMode = false;	// 批量制卡模式
	bool		    bPreSetBankCard = true;	// 批量制卡时使用预置银行卡号
	int				nNetTimeout = 1500;
public:
	bool			m_bDetectStarted = false;
	bool			m_bVideoStarted = false;
	HANDLE			m_hCamera = nullptr;
	int				m_nWidth = 640;
	int				m_nHeight = 480;
	int				nWndIndex = 0;
	byte* pImageBuffer = nullptr;
	int				nBufferSize = 0;
	string			strFullCapture;
	string			strFaceCapture;
	map<string, string>	mapBankCode;
	//int FaceCamera_EventCallback(int nEventID, int nFrameStatus);

	bool IsVideoStart();

	bool OpenCamera();

	bool StartVideo(HWND hWnd);

	void StopVideo();

	void CloseCamera();

	bool StartFaceDetect(void* pContext, int nDetectMilliSeconds = 2000, int nTimeoutMilliSeconds = 15000);

	bool SaveFaceImage(string strPhotoFile, bool bFull = true);

	bool FaceCompareByImage(string strFacePhoto1, string strFacePhoto2, float& dfSimilarity);

	void StopFaceDetect();

	bool SwitchVideoWnd(HWND hWnd);

	bool Snapshot(string strFilePath);

	bool InitializeDB(QString& strMessage);

public:
	int  ReaderIDCard(IDCardInfo* pIDCard);

	int OpenDevice(QString& strMessage);

	int OpenSSCardService(SSCardService** ppService, QString& strMessage, IDCardInfoPtr pInputIDCard = nullptr);

	int CloseSSCardService(QString& strMessage);

	int OpenPrinter(QString strPrinterLib, PrinterType nPrinterType, int& nDepenseBox, QString& strDPI, QString& strMessage);
	//KT_Printer* OpenPrinter(QString strPrinterLib, PrinterType nPrinterType, int& nDepenseBox, QString& strDPI, QString& strMessage);

	void CloseDevice();

	void CloseSSCardReader();

	int OpenPrinter(QString& strMessage);

	int OpenSSCardReader(QString& strMessage);

	int OpenSSCardReader(QString strLib, ReaderBrand nReaderType, QString& strMessage);

	int TestPrinter(QString& strMessage);

	int TestCard(QString& strMessage);

	int Depense(QString& strMessage);

	int Depense(int nDepenseBox, CardPowerType nPowerOnType, QString& strMessage);

	string MakeCardInfo(SSCardBaseInfoPtr& pSSCardInfo);

	int PrintCard(SSCardBaseInfoPtr& pSSCardInfo, QString strPhoto, QString& strMessage, bool bPrintText = true);
	int PrintTestCard(SSCardBaseInfoPtr& pSSCardInfo, QString strPhoto, QString& strMessage, bool bPrintText = true);

	int PrintExtraText(QString strText, int nAngle, float fxPos, float fyPos, QString strFont, int nFontSize, int nColor);

	int WriteCard(SSCardBaseInfoPtr& pSSCardInfo, QString& strMessage);

	int WriteCardTest(SSCardBaseInfoPtr& pSSCardInfo, QString& strMessage, QWidget* pParent = nullptr);

	int ReadCard(SSCardBaseInfoPtr& pSSCardInfo, QString& strMessage);

	int MoveCard(QString& strMessage);

	bool LoadBankCode(QString& strError);

	int CheckBankCode(string& strBankName, QString& strMessage)
	{
		if (QFailed(GetBankName(GetSysConfigure()->Region.strBankCode, strBankName)))
		{
			strMessage = "银行代码配置有误或未配置银行代码!";
			return 1;
		}

		return 0;
	}

	KT_Reader* GetSSCardReader()
	{
		return m_pSSCardReader;
	}
	KT_Printer* GetPrinter()
	{
		TraceMsgA("%s Printer Address = %p", __FUNCTION__, &m_pPrinter);
		return m_pPrinter;
	}
	SSCardService* GetSSCardService()
	{
		return pSScardSerivce;
	}

public:
	int  GetCardStatus(QString& strMessage);
	int  RegisterLost(QString& strMessage);
	int  PremakeCard(QString& strMessage);
	int	modifyPersonInfo(string& strBase64Photo, QString& strMessage);
	int  CommitPersionInfo(QString& strMessage);
	int	 SafeWriteCard(QString& strMessage);
	int GenerateAuthorization(QString& strMessage);
	int	 SafeWriteCardTest(QString& strMessage);
	int	 SafeReadCard(QString& strMessage);
	int  DownloadPhoto(string& strBase64Photo, QString& strMessage);
	int  LoadPhoto(string& strPhoto, QString& strMessage, PhotoType nType = PhotoType::Photo_CardID);
	int  LoadGuardianPhoto(string& strPhoto, QString& strMessage);
	int  EnsureData(QString& strMessage);
	int ReplaceCardSwitchCard(QString& strMessage);
	int  ActiveCard(QString& strMessage);
	void RemoveTempPersonInfo();
private:
	IDCardInfoPtr	pIDCard = nullptr;

	SysConfigPtr	pSysConfig = nullptr;
	CardFormPtr		pCardForm = nullptr;						  // 打印版式
	SSCardBaseInfoPtr   pSSCardInfo = nullptr;
	KT_PrinterLibPtr	m_pPrinterlib = nullptr;
	KT_ReaderLibPtr		m_pSSCardReaderLib = nullptr;
	SSCardServiceLibPtr pSServiceLib = nullptr;
	vector<TextPosition> vecExtraText;

	KT_Printer* m_pPrinter = nullptr;
	KT_Reader* m_pSSCardReader = nullptr;
	SSCardService* pSScardSerivce = nullptr;
	Manager_Level	nManagerLevel = Manager_Level::Manager;

};

using DataCenterPtr = shared_ptr<DataCenter>;
extern DataCenterPtr g_pDataCenter;
extern vector<NationaltyCode> g_vecNationCode;
extern vector<HukouCode> g_vecHukouCode;
extern vector<EductionCode> g_vecEducationCode;
extern vector<MarrageCode> g_vecMarriageCode;
extern vector<GuojiCode> g_vecGuojiCode;
extern vector<DocType> g_vecDocCode;
int QMessageBox_CN(QMessageBox::Icon nIcon, QString strTitle, QString strText, QMessageBox::StandardButtons stdButtons, QWidget* parent = nullptr);

class QWaitCursor
{
	bool bStart = false;
public:
	QWaitCursor();

	void RestoreCursor();

	~QWaitCursor();
};

void SetTableWidgetItemChecked(QTableWidget* pTableWidget, int nRow, int nCol, QButtonGroup* pButtonGrp, int nItemID, bool bChecked = false);

QStringList SearchFiles(const QString& dir_path, QDateTime* pStart = nullptr, QDateTime* pStop = nullptr);

bool GetModuleVersion(QString strModulePath, WORD& nMajorVer, WORD& nMinorVer, WORD& nBuildNum, WORD& nRevsion);

#define WaitCursor()  QWaitCursor qWait;

void EnableWidgets(QWidget* pUIObj, bool bEnable = true);

void ShowWidgets(QWidget* pUIObj, bool bShow = true);

char VerifyCardID(const char* pszSrc);

HWND GetTouchKeybroadWnd();

QString CardStatusString(CardStatus nCardStratus);
// 需提前把要处理的图片放在./PhotoProcess目录下，并命名为1.jpg
int ProcessHeaderImage(QString& strHeaderPhoto, QString& strMessage);

struct _CareerType
{
	QString strCode;
	QString strName;
};

extern vector<_CareerType> vecCareer;
#endif // GLOABAL_H


