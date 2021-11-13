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
	string		strPrinter;							   // ��ӡ����
	string		strDPI;								   // 
	string		PinKeybroadPort;					   // ������̴��ںͲ�����
	string		strPinKeybroadBaudrate;				   // 
	string		strSSCardReadType;					   // ������	DC || HD
	string		strSSCardReaderPowerOnType;			   // �ϵ緽ʽ  1:�Ӵ� 2 : �ǽ�
	string		strSSCardReaderPort;				   // �������˿� COM1 = 0��COM2 = 1.....USB = 100
	string		strIDCardReaderPort;				   // ���֤����������:USB, COM1, COM2...
};

struct RegionInfo
{												   
	string		strRegionCode;						   // ���б��� 410700
	string		strZone;							   // ������� 41070000
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
	int			fAngle;								   // ��ת�Ƕ�
	float		fWidth;								   // ��ӡ���
	float		fHeight;							   // ��ӡ�߶�
};
struct InfoLayout
{
	TextPosition	posName;						   // ����
    ImagePosition	posPhoto;						   // ��Ƭ
	TextPosition	posIDNumber;					   // ���֤��
	TextPosition	posSSCardID;					   // �籣����
	TextPosition	posIssueDate;					   // ǩ������
	float			fAngle;							   // ������ת�Ƕ�
};


struct Config
{
	DeviceConfig	DevConfig;						  // �豸����
	RegionInfo		RegInfo;						  // ������Ϣ����
	InfoLayout		CardLayout;						  // ��ӡ��ʽ
	int				nBatchMode;						  // �����ƿ� ������0    �رգ�1
	string			strDBPath;						  // ���ݴ洢·��
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
