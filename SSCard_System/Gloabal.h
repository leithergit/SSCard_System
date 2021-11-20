
#ifndef GLOABAL_H
#define GLOABAL_H
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
#include <Windows.h>
#include <winreg.h>
#include <string>
#include <algorithm>

#include "SDK/IDCard/idcard_api.h"
#include "SDK/PinKeybroad/XZ_F10_API.h"
#include "../utility/Utility.h"
#include "../utility/TimeUtility.h"
#include "../utility/AutoLock.h"
#include "../SDK/Printer/KT_Printer.h"
#include "../SDK/Reader/KT_Reader.h"


#include "logging.h"
#define QSucceed(x)      (x == 0)
#define QFailed(x)       (!x)
using namespace std;

using namespace chrono;
using namespace Kingaotech;

enum FaceDetectStatus
{
    FD_Succeed,
    FD_Binocularcamera_OpenFailed,
};

#define gInfo()      LOG(INFO)
#define gError()     LOG(ERROR)
#define gWarning()   LOG(WARNING)
#define Str(x)       #x
struct DeviceConfig
{
	DeviceConfig(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		gInfo() << "Try to read device configure";
		pSettings->beginGroup("Device");
		strPrinter = pSettings->value("Printer").toString().toStdString();
        strPrinterType = pSettings->value("PrinterType").toString().toStdString();
        transform(strPrinterType.begin(),strPrinterType.end(),strPrinterType.begin(),::toupper);
        const char* szTypeList[] = {
            Str(EVOLIS_KC200),
            Str(EVOLIS_ZENIUS),
            Str(EVOLIS_AVANSIA),
            Str(HITI_CS200),
            Str(HITI_CS220),
            Str(HITI_CS290),
            Str(ENTRUCT_EM1),
            Str(ENTRUCT_EM2),
            Str(ENTRUCT_CD809)
        };
        for (int nIndex = 0;nIndex < sizeof (szTypeList)/sizeof(char*);nIndex ++)
        {
            if (strPrinterType == szTypeList[nIndex])
            {
                nPrinterType = (Printer)nIndex;
                break;
            }
        }

		strDPI = pSettings->value("PrinterDPI", "300*300").toString().toStdString();
		PinKeybroadPort = pSettings->value("PinKeybroadPort").toString().toStdString();
		nPinKeybroadBaudrate = pSettings->value("PinKeybroadBaudrate", "9600").toUInt();
		strSSCardReadType = pSettings->value("SSCardReadType", "DC").toString().toStdString();
		nSSCardReaderPowerOnType = pSettings->value("SSCardReaderPowerOnType").toUInt();
		strSSCardReaderPort = pSettings->value("SSCardReaderPORT", "100").toString().toStdString();
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
	}
	string		strPrinter;							   // 打印机名
    Printer     nPrinterType;                          // 打印机型号代码
    string      strPrinterType;                        // 打印机型号名
	string		strDPI;								   // 打印机DPI，300*300,300*600
	string		PinKeybroadPort;					   // 密码键盘串口和波特率
	string		nPinKeybroadBaudrate;				   // 9600
	string		strSSCardReadType;					   // 读卡器	DC || HD
	string		nSSCardReaderPowerOnType;			   // 上电方式  1:接触 2 : 非接
	string		strSSCardReaderPort;				   // 读卡器端口 COM1 = 0，COM2 = 1.....USB = 100
	string		strIDCardReaderPort;				   // 身份证读卡器配置:USB, COM1, COM2...
};

struct RegionInfo
{
	RegionInfo(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		gInfo() << "Try to read Region configure";
		pSettings->beginGroup("Region");
		strRegionCode = pSettings->value("Region").toString().toStdString();
		strArea = pSettings->value("Area").toString().toStdString();
		strLicense = pSettings->value("License").toString().toStdString();
		strEMURL = pSettings->value("EMURL").toUInt();
		strCMURL = pSettings->value("CMURL").toString().toStdString();

		// 		_info() << "Region Cofnigure:\n";
		// 		_info() << "\t\Region:" << strRegionCode;
		// 		_info() << "\t\Area:" << strArea;
		// 		_info() << "\t\License:" << strLicense;
		// 		_info() << "\t\EMURL:" << strEMURL;
		// 		_info() << "\t\CMURL:" << strCMURL;
		pSettings->endGroup();
	}
	string		strRegionCode;						   // 地市编码 410700
	string		strArea;							   // 区域编码 41070000
	string		strLicense;							   // LICENSE = STZOLdFrhbc
	string		strEMURL;							   // 加密机ip http://10.120.6.239:7777/
	string		strCMURL;							   // 卡管ip   http://10.120.1.18:7001/hnCardService/services/CardService
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

		QByteArray byteFont = pSettings->value("Font").toString().toLatin1();

		int nDataLength = 0;
		strFont = UTF8StringA(byteFont.data(), nDataLength).get();
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
        gInfo() << "Try to read Payment configure";
        pSettings->beginGroup("Payment");
        nWaitTime                = pSettings->value("WaitTime").toUInt();
        nQueryPayResultInterval  = pSettings->value("QueryPayResultInterval").toUInt();
        nSocketRetryCount        = pSettings->value("SocketRetryCount").toUInt();
        pSettings->endGroup();
    }
    int     nWaitTime = 300;                         // 支付页面等侍时间，单位秒
    int     nQueryPayResultInterval = 500;            // 支付结构查询时间间隔单 毫秒
    int     nSocketRetryCount = 5;                    // 网络失败重试次数
};
struct SysConfig
{
	SysConfig(QSettings* pSettings)
		: DevConfig(pSettings)
		, RegInfo(pSettings)
        , PaymentConfig(pSettings)
	{
		pSettings->beginGroup("Other");
        nBatchMode          = pSettings->value("BATCHMODE").toInt();
        strDBPath           = pSettings->value("DBPATH").toString().toStdString();
        dfFaceSimilarity    = pSettings->value("FaceSimilarity").toDouble();
        nMobilePhoneSize    = pSettings->value("MobilePhoneNumberLength",11).toUInt();
        nSSCardPasswordSize = pSettings->value("SSCardPasswordLength",6).toUInt();
		pSettings->endGroup();
	}
	DeviceConfig	DevConfig;						  // 设备配置
	RegionInfo		RegInfo;						  // 区域信息配置
    PaymentOpt      PaymentConfig;                    // 支付相关设置
	int				nBatchMode = 0;					  // 批量制卡 开启：0    关闭：1
	string			strDBPath;						  // 数据存储路径
    double          dfFaceSimilarity;                 // 人脸认别最低相似度
    int             nMobilePhoneSize = 11;            // 手机号码长度
    int             nSSCardPasswordSize = 6;          // 社保卡密码长度
};
using SysConfigPtr  = shared_ptr<SysConfig>;

struct SSCardInfo
{
    string  strName;
    string  strIDNumber;
    string  strSSCardNumber;
    string  strIssuedDate;
    string  strPhotoPath;
    string  strBank;
    string  strStatus;
    void GetInfoFromIDCard(IDCardInfoPtr &pIDCardInfo)
    {
        strName      = (const char *)pIDCardInfo->szName;
        strIDNumber  = (const char *)pIDCardInfo->szIdentify;
    }
};
using SSCardInfoPtr = shared_ptr<SSCardInfo>;

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
        pIDCard.reset();
        pSSCardInfo.reset();
    }
    SSCardInfoPtr &GetSSCardInfo()
    {
        return pSSCardInfo;
    }
    void FillSSCardWithIDCard()
    {
        if (pSSCardInfo && pIDCard)
        {
            pSSCardInfo->strName      = (const char *)pIDCard->szName;
            pSSCardInfo->strIDNumber  = (const char *)pIDCard->szIdentify;
        }
    }

    void SetSSCardInfo(SSCardInfoPtr& pCardInfo)
    {
        pSSCardInfo = pCardInfo;
    }

    string         strIDImageFile;
    string         strMobilePhone;
    string         strSSCardOldPassword;
    string         strSSCardNewPassword;
private:
    IDCardInfoPtr	pIDCard = nullptr;
	SysConfigPtr	pConfig = nullptr;
	CardFormPtr		pCardForm = nullptr;						  // 打印版式
    SSCardInfoPtr   pSSCardInfo = nullptr;

};

using DataCenterPtr = shared_ptr<DataCenter>;
extern DataCenterPtr g_pDataCenter;

#endif // GLOABAL_H
