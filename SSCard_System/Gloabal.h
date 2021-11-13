#ifndef GLOABAL_H
#define GLOABAL_H

#include <thread>
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "logging.h"
#include "utility/TimeUtility.h"
#include "utility/Utility.h"
#include "utility/AutoLock.h"
#include "SDK/IDCard/idcard_api.h"
#include "SDK/PinKeybroad/XZ_F10_API.h"
#include <string>
#include <QObject>
#include <QString>
#include <memory>

using namespace std;

#define _info()      LOG(INFO)
#define _error()     LOG(ERROR)
#define _warning()   LOG(WARNING)
struct DeviceConfig
{
	string		strPrinter;							   // 打印机名
	string		strDPI;								   // 
	string		PinKeybroadPort;					   // 密码键盘串口和波特率
	string		strPinKeybroadBaudrate;				   // 
	string		strSSCardReadType;					   // 读卡器	DC || HD
	string		strSSCardReaderPowerOnType;			   // 上电方式  1:接触 2 : 非接
	string		strSSCardReaderPort;				   // 读卡器端口 COM1 = 0，COM2 = 1.....USB = 100
	string		strIDCardReaderPort;				   // 身份证读卡器配置:USB, COM1, COM2...
};

struct RegionInfo
{												   
	string		strRegionCode;						   // 地市编码 410700
	string		strZone;							   // 区域编码 41070000
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
	int			fAngle;								   // 旋转角度
	float		fWidth;								   // 打印宽度
	float		fHeight;							   // 打印高度
};
struct InfoLayout
{
	TextPosition	posName;						   // 姓名
    ImagePosition	posPhoto;						   // 照片
	TextPosition	posIDNumber;					   // 身份证号
	TextPosition	posSSCardID;					   // 社保卡号
	TextPosition	posIssueDate;					   // 签发日期
	float			fAngle;							   // 整体旋转角度
};


struct Config
{
	DeviceConfig	DevConfig;						  // 设备配置
	RegionInfo		RegInfo;						  // 区域信息配置
	InfoLayout		CardLayout;						  // 打印版式
	int				nBatchMode;						  // 批量制卡 开启：0    关闭：1
	string			strDBPath;						  // 数据存储路径
};

using ConfigPtr = shared_ptr<Config>;

class DataCenter
{
	Q_OBJECT
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


#endif // GLOABAL_H
