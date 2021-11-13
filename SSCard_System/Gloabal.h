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
	string		strPrinter;							   // ��ӡ����
	string		strDPI;								   // ��ӡ��DPI��300*300,300*600
	string		PinKeybroadPort;					   // ������̴��ںͲ�����
	string		nPinKeybroadBaudrate;				   // 9600
	string		strSSCardReadType;					   // ������	DC || HD
	string		nSSCardReaderPowerOnType;			   // �ϵ緽ʽ  1:�Ӵ� 2 : �ǽ�
	string		strSSCardReaderPort;				   // �������˿� COM1 = 0��COM2 = 1.....USB = 100
	string		strIDCardReaderPort;				   // ���֤����������:USB, COM1, COM2...
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
	string		strRegionCode;						   // ���б��� 410700
	string		strArea;							   // ������� 41070000
	string		strLicense;							   // LICENSE = STZOLdFrhbc
	string		strEMURL;							   // ���ܻ�ip http://10.120.6.239:7777/
	string		strCMURL;							   // ����ip   http://10.120.1.18:7001/hnCardService/services/CardService
};

struct TextPosition
{
	string		strText;							   // ��ӡ������
	string		strFontName;						   // ��ӡ����,����"����"
	int			nFontSize;							   // �����С,��λΪpt,1��=72pt
	int			nColor;								   // ��ӡ������ɫ,RGB��ɫֵ
	int			nFontStyle;							   // ������1-���棻2-���壻4-б�壻8-����
	float		fxPos;								   // ��ʼX���꣬��λ����
	float		fyPos;								   // ��ʼY���꣬��λ����
	int			fAngle;								   // ��ת�Ƕ�
};

struct ImagePosition
{
	string		strPath;							   // ͼƬȫ·��
	float		fxPos;								   // ��ʼX���꣬��λ����
	float		fyPos;								   // ��ʼY���꣬��λ����	
	float		fWidth;								   // ��ӡ���
	float		fHeight;							   // ��ӡ�߶�
	int			fAngle;								   // ��ת�Ƕ�
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
	TextPosition	posName;						   // ����
    ImagePosition	posImage;						   // ��Ƭ
	TextPosition	posIDNumber;					   // ���֤��
	TextPosition	posSSCardID;					   // �籣����
	TextPosition	posIssueDate;					   // ǩ������
	string			strFont;						   // ��������
	UINT			nFontColor;						   // ������ɫ
	UINT			nFontSize;						   // ����ߴ�
	float			fAngle;							   // ������ת�Ƕ�
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
	DeviceConfig	DevConfig;						  // �豸����
	RegionInfo		RegInfo;						  // ������Ϣ����
	CardLayout		CardForm;						  // ��ӡ��ʽ
	int				nBatchMode;						  // �����ƿ� ������0    �رգ�1
	string			strDBPath;						  // ���ݴ洢·��
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
