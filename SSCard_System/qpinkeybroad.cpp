#include "qpinkeybroad.h"
#include "Gloabal.h"

QPinKeybroad::QPinKeybroad(QString strDevPort, ushort nBaudrate, QObject* parent) :
	QObject(parent),
	m_strDevPort(strDevPort.toUpper()),
	m_nBaudreate(nBaudrate)
{
}
QPinKeybroad::~QPinKeybroad()
{
	QString strError;
	gInfo() << "Try to close device";
	CloseDevice(strError);

}
int  QPinKeybroad::OpenDevice(QString& strError)
{
	gInfo() << "Try to Open device " << strError.toStdString().c_str();
	int nOffset = m_strDevPort.indexOf("COM");
	if (nOffset < 0)
	{
		strError = QString("密码键盘端口参数错误:%1").arg(m_strDevPort);
		return -1;
	}

	QString strNum = m_strDevPort.right(m_strDevPort.size() - 3);
	int nPort = strNum.toInt();

	gInfo() << "Try to Open Port " << nPort;
	if (!g_pDataCenter->OpenCom(nPort, m_nBaudreate))
	{
		strError = QString("打开密码键盘失败:%1,Baudrate:%2").arg(m_strDevPort).arg(m_nBaudreate);
		return -1;
	}
	gInfo() << "Try to SUNSON_UseEppPlainTextMode";
	unsigned char szRetInfo[255] = { 0 };
	if (!g_pDataCenter->UseEppPlainTextMode(0x06, 1, szRetInfo))
	{
		g_pDataCenter->CloseCom();
		strError = QString("密码键盘设置输入模式失败!");
		return -1;
	}

	m_bDevOpened = true;
	return 0;
}
int QPinKeybroad::CloseDevice(QString& strError)
{

	Q_UNUSED(strError);
	if (m_bDevOpened)
	{
		gInfo() << "Try to SUNSON_CloseCom";
		m_bDevOpened = false;
		g_pDataCenter->CloseCom();
	}

	return 0;
}
int  QPinKeybroad::ReadPin(uchar* pKeyValue)
{
	return g_pDataCenter->ScanKeyPress(pKeyValue);
}
