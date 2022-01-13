#pragma execution_character_set("utf-8")
#include "up_Inputpwd.h"
#include "ui_up_Inputpwd.h"
#include "Gloabal.h"
#include <QString>
#include <chrono>
#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;
//#include "mainwindow.h"

up_InputPWD::up_InputPWD(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::InputPWD)
{
	ui->setupUi(this);
	connect(this, &QStackPage::InputPin, this, &up_InputPWD::OnInputPin);
}

up_InputPWD::~up_InputPWD()
{
	ShutDown();
	delete ui;
}

void  up_InputPWD::ClearPassword()
{
	ZeroMemory(m_szPin, sizeof(m_szPin));
	m_nPinSize = 0;
	ui->lineEdit_OldPassword->setText("");
}

int up_InputPWD::ProcessBussiness()
{
	ClearPassword();
	ui->lineEdit_OldPassword->setFocus();
	m_nSSCardPWDSize = g_pDataCenter->GetSysConfigure()->nSSCardPasswordSize;
	m_strDevPort = g_pDataCenter->GetSysConfigure()->DevConfig.strPinBroadPort.c_str();
	m_nBaudreate = QString(g_pDataCenter->GetSysConfigure()->DevConfig.strPinBroadBaudrate.c_str()).toUShort();
	m_pPinKeybroad = make_shared<QPinKeybroad>(m_strDevPort, m_nBaudreate);
	QString strError;
	if (!m_pPinKeybroad)
	{
		strError = QString("内存不足,初始化密码键盘对象失败!");
		gError() << strError.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
		return -1;
	}

	if (QFailed(m_pPinKeybroad->OpenDevice(strError)))
	{
		gError() << strError.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
		return -1;
	}
	QString strMessage;
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	RegionInfo& reginfo = g_pDataCenter->GetSysConfigure()->Region;
	char szRCode[1024] = { 0 };
	char szATR[1024] = { 0 };
	int nATRLen = 0;


	if (QFailed(g_pDataCenter->OpenSSCardReader(DevConfig.strDesktopReaderModule.c_str(), DevConfig.nDesktopSSCardReaderType, strMessage)))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	if (QFailed(g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nDesktopSSCardReaderPowerOnType, szATR, nATRLen, szRCode)))
	{
		emit ShowMaskWidget("操作失败", "卡片上电失败!", Fetal, Return_MainPage);
		return -1;
	}

	if (DriverInit((HANDLE)g_pDataCenter->GetSSCardReader(), (char*)reginfo.strCityCode.c_str(), (char*)reginfo.strSSCardDefaulutPin.c_str(), (char*)reginfo.strPrimaryKey.c_str(), szRCode))
	{
		strMessage = QString("DriverInit失败:%1").arg(szRCode);
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	m_bWorkThreadRunning = true;
	ZeroMemory(m_szPin, sizeof(m_szPin));
	m_pWorkThread = new std::thread(&up_InputPWD::ThreadWork, this);
	if (!m_pWorkThread)
	{
		strError = QString("内存不足,创建密码键盘线程失败!");
		gError() << strError.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
		return -1;
	}
	return 0;
}

void up_InputPWD::OnTimeout()
{
	ShutDown();
}

void  up_InputPWD::ShutDown()
{
	gInfo() << __FUNCTION__;
	m_bWorkThreadRunning = false;
	if (m_pWorkThread && m_pWorkThread->joinable())
	{
		m_pWorkThread->join();
		delete m_pWorkThread;
		m_pWorkThread = nullptr;
	}

	QString strMessage;
	if (m_pPinKeybroad)
	{
		m_pPinKeybroad->CloseDevice(strMessage);
		m_pPinKeybroad = nullptr;
	}

	g_pDataCenter->CloseDevice();
}
//
//int up_InputPWD::OpenSSCardReader(QString strLib, ReaderBrand nReaderType, QString& strMessage)
//{
//	int nResult = -1;
//	try
//	{
//		m_pReaderLib = nullptr;
//		do
//		{
//			if (!m_pReaderLib)
//			{
//				char szRCode[32] = { 0 };
//				QString strAppPath = QCoreApplication::applicationDirPath();
//				QString strReaderMudule = strAppPath + "/" + strLib.toStdString().c_str();
//
//				m_pReaderLib = make_shared<KTModule<KT_Reader>>(strReaderMudule.toStdString());
//				if (!m_pReaderLib)
//				{
//					strMessage = strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strReaderMudule);
//					break;
//				}
//				m_pSSCardReader = m_pReaderLib->Instance();
//				if (!m_pSSCardReader)
//				{
//					strMessage = QString("创建‘%1’实例失败!").arg(strReaderMudule);
//					break;
//				}
//				if (QFailed(nResult = m_pSSCardReader->Reader_Create(nReaderType, szRCode)))
//				{
//					strMessage = QString("Reader_Create(‘%1’)失败,错误代码:%2").arg(nReaderType).arg(szRCode);
//					break;
//				}
//				if (QFailed(nResult = m_pSSCardReader->Reader_Init(szRCode)))
//				{
//					strMessage = QString("Reader_Init失败,错误代码:%2").arg(szRCode);
//					break;
//				}
//			}
//
//		} while (0);
//		if (QFailed(nResult))
//			return -1;
//		else
//			return 0;
//	}
//	catch (std::exception& e)
//	{
//		strMessage = e.what();
//		gError() << gQStr(strMessage);
//		return -1;
//	}
//	return 0;
//}
//
//void up_InputPWD::CloseSSCardReader()
//{
//	char szRCode[32] = { 0 };
//
//	if (m_pSSCardReader)
//	{
//		m_pSSCardReader->Reader_Exit(szRCode);
//		m_pSSCardReader = nullptr;
//	}
//	m_pReaderLib = nullptr;
//}


void up_InputPWD::OnInputPin(unsigned char ch)
{
	switch (ch)
	{
	case 0x1b:		// cancel
	{
		ClearPassword();
		break;
	}
	case 0x08:		// update
	{
		if (m_nPinSize > 0)
		{
			m_szPin[m_nPinSize - 1] = '\0';
			m_nPinSize--;
		}
		break;
	}
	case 0x0d:		// confirm
	{
		qDebug() << "Catch enter!";

		qDebug("confirm...");
		if (m_nPinSize < m_nSSCardPWDSize)
		{
			QString strMessage = QString("输入的密码位数不足%1位,请输入正确的密码!").arg(m_nSSCardPWDSize);
			emit ShowMaskWidget("操作错误", strMessage, Error, Stay_CurrentPage);
		}
		else
		{
			ui->pushButton_OK->emit click();
		}
		break;
	}
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	{
		qDebug("function key...");
		break;
	}
	case 0xaa:
	{
		qDebug("Input end...");
		QString strMessage;
		m_pPinKeybroad->CloseDevice(strMessage);
		m_pPinKeybroad->OpenDevice(strMessage);
		break;
	}
	default:
	{
		if (m_nPinSize < 6)
			m_szPin[m_nPinSize++] = ch;
	}
	}
	QString strOldPassword = (char*)m_szPin;
	ui->lineEdit_OldPassword->setText(strOldPassword);
}
void up_InputPWD::ThreadWork()
{
	uchar szTemp[16] = { 0 };
	int nRet = 0;

	while (m_bWorkThreadRunning)
	{
		nRet = SUNSON_ScanKeyPress(szTemp);
		if (nRet > 0)
			emit InputPin(szTemp[0]);
		this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

int  up_InputPWD::CheckPassword(QString& strMessage)
{
	char szRCode[1024] = { 0 };
	string strCardATR;
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;

	if (QFailed(iVerifyPin(DevConfig.nDesktopSSCardReaderPowerOnType, (char*)m_szPin, szRCode)))
	{
		strMessage = "密码校验失败";
		ui->lineEdit_OldPassword->setText("");
		ZeroMemory(m_szPin, sizeof(m_szPin));
		m_nPinSize = 0;
		return -1;
	}
	if (strcmp(szRCode, "0000") == 0)
	{
		strMessage = "密码校验成功!";
		return 0;
	}
	else
	{
		int iRemainChange = strtol(szRCode, nullptr, 10);
		if (iRemainChange == 0)
			strMessage = QString("输入密码错误,卡片已锁定!").arg(iRemainChange);
		else
			strMessage = QString("输入密码错误,剩余机会:%1").arg(iRemainChange);
		return -1;
	}
}

void up_InputPWD::on_pushButton_OK_clicked()
{
	qDebug("confirm...");
	QString strError;
	if (m_nPinSize < 6)
	{
		strError = QString("输入的密码位数不足6位,请继续输入密码!");
		gError() << strError.toLocal8Bit().data();
		emit ShowMaskWidget("操作错误", strError, Error, Stay_CurrentPage);
	}
	else
	{
		QString strError;
		if (QFailed(CheckPassword(strError)))
		{
			strError = QString("密码校验失败,请重新输入密码!");
			gError() << strError.toLocal8Bit().data();
			ui->lineEdit_OldPassword->setText("");
			emit ShowMaskWidget("操作失败", strError, Error, Stay_CurrentPage);
		}
		else
		{
			g_pDataCenter->strSSCardOldPassword = (const char*)m_szPin;

			emit ShowMaskWidget("操作成功", "密码校验成功,随后请输入新密码!", Success, Switch_NextPage);
		}
	}
}
