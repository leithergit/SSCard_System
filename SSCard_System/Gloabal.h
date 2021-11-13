#ifndef GLOABAL_H
#define GLOABAL_H

#include <thread>
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <string>
#include <QObject>
#include <QString>
#include <memory>
#include <QSettings>
#include "../utility/TimeUtility.h"
#include "../utility/Utility.h"
#include "../utility/AutoLock.h"
#include "SDK/IDCard/idcard_api.h"
#include "SDK/PinKeybroad/XZ_F10_API.h"
#include "logging.h"
using namespace std;

#define gInfo()      LOG(INFO)
#define gError()     LOG(ERROR)
#define gWarning()   LOG(WARNING)
struct DeviceConfig
{
	DeviceConfig(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		gInfo() << "Try to read device configure";
		pSettings->beginGroup("Device");
		strPrinter				= pSettings->value("Printer").toString().toStdString();
		strDPI					= pSettings->value("PrinterDPI","300*300").toString().toStdString();
		PinKeybroadPort			= pSettings->value("PinKeybroadPort").toString().toStdString();
		nPinKeybroadBaudrate	= pSettings->value("PinKeybroadBaudrate","9600").toUInt();
		strSSCardReadType		= pSettings->value("SSCardReadType","DC").toString().toStdString();
		nSSCardReaderPowerOnType= pSettings->value("SSCardReaderPowerOnType").toUInt();
		strSSCardReaderPort		= pSettings->value("SSCardReaderPORT", "100").toString().toStdString();
		strIDCardReaderPort		= pSettings->value("IDCardReaderPort", "USB").toString().toStdString();
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
	int			fAngle;								   // 旋转角度
};

struct ImagePosition
{
	string		strPath;							   // 图片全路径
	float		fxPos;								   // 起始X坐标，单位毫米
	float		fyPos;								   // 起始Y坐标，单位毫米	
	float		fWidth;								   // 打印宽度
	float		fHeight;							   // 打印高度
	int			fAngle;								   // 旋转角度
};
struct CardLayout
{
	CardLayout(QSettings* pSettings)
	{
		if (!pSettings)
			return;
		pSettings->beginGroup("CardForm");
		QString strTemp = pSettings->value("ImagePOS").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%f|%f|%d", &posImage.fxPos, &posImage.fyPos, &posImage.fWidth, &posImage.fHeight, &posImage.fAngle);

		strTemp = pSettings->value("NamePOS").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%d", &posName.fxPos, &posName.fyPos, &posImage.fAngle);

		strTemp = pSettings->value("IDPOS").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%d", &posIDNumber.fxPos, &posIDNumber.fyPos, &posIDNumber.fAngle);

		strTemp = pSettings->value("CARDNOPOS").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%d", &posSSCardID.fxPos, &posSSCardID.fyPos, &posSSCardID.fAngle);

		strTemp = pSettings->value("DATEPOS").toString();
		sscanf_s(strTemp.toStdString().c_str(), "%f|%f|%d", &posIssueDate.fxPos, &posIssueDate.fyPos, &posIssueDate.fAngle);

		strFont = pSettings->value("Font").toString().toStdString();

		nFontSize = pSettings->value("FontSize").toUInt();
		
		nFontColor = pSettings->value("FontColor").toUInt();

		fAngle = pSettings->value("Angle").toUInt();

		pSettings->endGroup();
	}
	TextPosition	posName;						   // 姓名
    ImagePosition	posImage;						   // 照片
	TextPosition	posIDNumber;					   // 身份证号
	TextPosition	posSSCardID;					   // 社保卡号
	TextPosition	posIssueDate;					   // 签发日期
	string			strFont;						   // 字体名称
	UINT			nFontColor;						   // 字体颜色
	UINT			nFontSize;						   // 字体尺寸
	float			fAngle;							   // 整体旋转角度
};


struct Config
{
	Config(QSettings* pSettings)
		: DevConfig(pSettings)
		, RegInfo(pSettings)
		, CardForm(pSettings)
	{
		pSettings->beginGroup("CardForm");
		nBatchMode = pSettings->value("BATCHMODE").toInt();
		strDBPath = pSettings->value("DBPATH").toString().toStdString();
		pSettings->endGroup();
	}
	DeviceConfig	DevConfig;						  // 设备配置
	RegionInfo		RegInfo;						  // 区域信息配置
	CardLayout		CardForm;						  // 打印版式
	int				nBatchMode;						  // 批量制卡 开启：0    关闭：1
	string			strDBPath;						  // 数据存储路径
};

using ConfigPtr = shared_ptr<Config>;

class DataCenter
{
public:
	explicit DataCenter();
	~DataCenter();
	IDCardInfo& GetIDCardInfo()
	{
		return IDCard;
	}
	int LoadConfig(QString& strError);
	
private:
	IDCardInfo	IDCard;
	ConfigPtr	pConfig = nullptr;
};

using DataCenterPtr = shared_ptr<DataCenter>;
extern DataCenterPtr g_pDataCenter;

#endif // GLOABAL_H
