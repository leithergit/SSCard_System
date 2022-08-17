#include "qpinkeybroad.h"
#include "Gloabal.h"

QPinKeybroad::QPinKeybroad( QObject* parent) :
	QObject(parent)
{
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	m_strDevPort = g_pDataCenter->GetSysConfigure()->DevConfig.strPinBroadPort.c_str();
	m_nBaudreate = QString(g_pDataCenter->GetSysConfigure()->DevConfig.strPinBroadBaudrate.c_str()).toUShort();
	strModule = DevConfig.strPinKeybroadModule;
}
QPinKeybroad::~QPinKeybroad()
{
	QString strError;
	gInfo() << "Try to close device";
	CloseDevice(strError);
	FreeLibrary(hMoudle);
}

bool  QPinKeybroad::OpenDevice(string strDev, int nBaud, QString& strError)
{
	m_nBaudreate = nBaud;
	m_strDevPort = strDev.c_str();
	return OpenDevice(strError);
}
bool  QPinKeybroad::OpenDevice(QString& strError)
{
	gInfo() << "Try to Open device " << strError.toStdString().c_str();
	if (strModule.empty())
	{
		strError = QString("未配置密码键盘模块:%1");
		return false;
	}
	if (strModule.find("XZ_F10") != string::npos)
	{
		nFailedFlag = -1;
		nDevType = XZ_F10;
	}
	
	hMoudle = LoadLibraryA(strModule.c_str());
	if (!hMoudle)
	{
		strError = QString("加载密码键盘模块%1失败").arg(strModule.c_str());
		return false;
	}
	_SUNSON_OpenCom = (fnSUNSON_OpenCom)GetProcAddress(hMoudle, "SUNSON_OpenCom");
	SUNSON_CloseCom = (fnSUNSON_CloseCom)GetProcAddress(hMoudle, "SUNSON_CloseCom");
	_SUNSON_ScanKeyPress = (fnSUNSON_ScanKeyPress)GetProcAddress(hMoudle, "SUNSON_ScanKeyPress");
	if (strModule.find("XZ_F10") != string::npos)
		SUNSON_UseEppPlainTextMode10 = (fnSUNSON_UseEppPlainTextMode10)GetProcAddress(hMoudle, "SUNSON_UseEppPlainTextMode");
	else
		SUNSON_UseEppPlainTextMode31 = (fnSUNSON_UseEppPlainTextMode31)GetProcAddress(hMoudle, "SUNSON_UseEppPlainTextMode");


	int nOffset = m_strDevPort.indexOf("COM");
	if (nOffset < 0)
	{
		strError = QString("密码键盘端口参数错误:%1").arg(m_strDevPort);
		return false;
	}

	QString strNum = m_strDevPort.right(m_strDevPort.size() - 3);
	int nPort = strNum.toInt();

	gInfo() << "Try to Open Port " << nPort;
	if (_SUNSON_OpenCom(nPort, m_nBaudreate) == nFailedFlag)
	{
		strError = QString("打开密码键盘失败:%1,Baudrate:%2").arg(m_strDevPort).arg(m_nBaudreate);
		return false;
	}
	gInfo() << "Try to SUNSON_UseEppPlainTextMode";
	unsigned char szRetInfo[255] = { 0 };
	if (strModule.find("XZ_F10") != string::npos)
	{
		if (SUNSON_UseEppPlainTextMode10(0x03, szRetInfo) == nFailedFlag)
		{
			SUNSON_CloseCom();
			strError = QString("密码键盘设置输入模式失败!");
			return false;
		}
	}
	else if (!SUNSON_UseEppPlainTextMode31(0x06, 1, szRetInfo) == nFailedFlag)
	{
		SUNSON_CloseCom();
		strError = QString("密码键盘设置输入模式失败!");
		return false;
	}

	m_bDevOpened = true;
	return true;
}

bool QPinKeybroad::CloseDevice(QString& strError)
{
	Q_UNUSED(strError);
	if (m_bDevOpened)
	{
		gInfo() << "Try to SUNSON_CloseCom";
		m_bDevOpened = false;
		SUNSON_CloseCom();
	}

	return 0;
}

//bool  QPinKeybroad::ReadPin(uchar* pKeyValue)
//{
//	return SUNSON_ScanKeyPress(pKeyValue) != nFailedFlag;
//}
