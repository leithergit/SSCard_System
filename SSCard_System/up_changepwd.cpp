#pragma execution_character_set("utf-8")
#include "up_changepwd.h"
#include "ui_up_changepwd.h"
#include "Gloabal.h"
#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;
//#include "mainwindow.h"

up_ChangePWD::up_ChangePWD(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::up_ChangePWD)
{
	ui->setupUi(this);
	ui->lineEdit_Password1->installEventFilter(this);
	ui->lineEdit_Password2->installEventFilter(this);
	m_pLineEdit[0] = ui->lineEdit_Password1;
	m_pLineEdit[1] = ui->lineEdit_Password2;
	connect(this, &QStackPage::InputPin, this, &up_ChangePWD::OnInputPin);
}

up_ChangePWD::~up_ChangePWD()
{
	ShutDown();
	delete ui;
}

bool up_ChangePWD::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::FocusIn)
	{
		if (watched == ui->lineEdit_Password1)
			m_nInputFocus = 0;

		if (watched == ui->lineEdit_Password2)
			m_nInputFocus = 1;
		return true;
	}
	else
		return false;
}

void  up_ChangePWD::ClearPassword()
{
	ZeroMemory(m_szPin, sizeof(m_szPin));
	ZeroMemory(m_nPinSize, sizeof(m_nPinSize));
	ui->lineEdit_Password1->setText("");
	ui->lineEdit_Password2->setText("");
}

int  up_ChangePWD::CheckPassword(QString& strError)
{
	if (m_nPinSize[0] < m_nSSCardPWDSize ||
		m_nPinSize[0] < m_nSSCardPWDSize)
	{
		strError = QString("密码长度不足%1位,请重新输入密码").arg(m_nSSCardPWDSize);
		return -1;
	}
	if (QFailed(memcmp(m_szPin[0], m_szPin[1], g_pDataCenter->GetSysConfigure()->nSSCardPasswordSize)))
	{
		strError = "两次输入密码不一致,请重新输入密码";
		return -1;
	}
	else
	{
		return 0;
	}
}

//int up_ChangePWD::OpenSSCardReader(QString strLib, ReaderBrand nReaderType, QString& strMessage)
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

//void up_ChangePWD::CloseSSCardReader()
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

int  up_ChangePWD::ChangePassword(QString& strMessage)
{
	char szRCode[1024] = { 0 };
	string strCardATR;
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	if (QFailed(iChangePin(DevConfig.nDesktopSSCardReaderPowerOnType, (char*)g_pDataCenter->strSSCardOldPassword.c_str(), (char*)m_szPin, szRCode)))
	{
		strMessage = "密码校验失败";
		return -1;
	}
	if (strcmp(szRCode, "0000") == 0)
	{
		strMessage = "密码修改成功!";
		return 0;
	}
	else
	{
		int iRemainChange = strtol(szRCode, nullptr, 10);
		if (iRemainChange == 0)
			strMessage = QString("修改密码失败,卡片已锁定!").arg(iRemainChange);
		else
			strMessage = QString("修改密码失败,剩余机会:%1").arg(iRemainChange);
		return -1;
	}
}

int up_ChangePWD::ProcessBussiness()
{
	ClearPassword();
	m_nInputFocus = 0;
	m_nSSCardPWDSize = g_pDataCenter->GetSysConfigure()->nSSCardPasswordSize;
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	//m_strDevPort = g_pDataCenter->GetSysConfigure()->DevConfig.strPinBroadPort.c_str();
	//m_nBaudreate = QString(g_pDataCenter->GetSysConfigure()->DevConfig.strPinBroadBaudrate.c_str()).toUShort();
	m_pPinKeybroad = make_shared<QPinKeybroad>();
	QString strMessage;
	if (!m_pPinKeybroad)
	{
		strMessage = QString("内存不足,初始化密码键盘对象失败!");
		gError() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	if (!m_pPinKeybroad->OpenDevice(strMessage))
	{
		gError() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	if (QFailed(g_pDataCenter->OpenSSCardReader(DevConfig.strDesktopReaderModule.c_str(), DevConfig.nDesktopSSCardReaderType, strMessage)))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	char szRCode[1024] = { 0 };
	char szCardATR[128] = { 0 };
	int  nCardAtrLen = 0;
	string strCardATR;
	if (QFailed(g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nDesktopSSCardReaderPowerOnType, szCardATR, nCardAtrLen, szRCode)))
	{
		emit ShowMaskWidget("操作失败", QString("卡片上电失败"), Fetal, Return_MainPage);
		return -1;
	}

	RegionInfo& reginfo = g_pDataCenter->GetSysConfigure()->Region;
	if (DriverInit((HANDLE)g_pDataCenter->GetSSCardReader(), (char*)reginfo.strCityCode.c_str(), (char*)reginfo.strSSCardDefaulutPin.c_str(), (char*)reginfo.strPrimaryKey.c_str(), szRCode))
	{
		strMessage = QString("DriverInit失败:%1").arg(szRCode);
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	m_pLineEdit[0]->setFocus();
	m_bWorkThreadRunning = true;
	m_pWorkThread = new std::thread(&up_ChangePWD::ThreadWork, this);
	if (!m_pWorkThread)
	{
		strMessage = QString("内存不足,创建密码键盘线程失败!");
		gError() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strMessage, Fetal, Return_MainPage);
		return -1;
	}
	return 0;
}

void up_ChangePWD::OnTimeout()
{
	ShutDown();
}

void  up_ChangePWD::ShutDown()
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

void up_ChangePWD::OnInputPin(uchar ch)
{
	switch (ch)
	{
	case 0x1b:		// cancel
	{
		memset(m_szPin[m_nInputFocus], 0, sizeof(m_szPin[m_nInputFocus]));
		m_nPinSize[m_nInputFocus] = 0;
		m_pLineEdit[m_nInputFocus]->setText("");
		gInfo() << QString("取消密码1输入!").toLocal8Bit().data();
		break;
	}
	case 0x08:		// update
	{
		if (m_nPinSize[m_nInputFocus] > 0)
		{
			m_szPin[m_nInputFocus][m_nPinSize[m_nInputFocus] - 1] = '\0';
			m_nPinSize[m_nInputFocus] --;
		}

		qDebug("Update...");
		break;
	}
	case 0x0d:		// confirm
	{
		ui->pushButton_OK->emit click();
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
		if (m_nPinSize[m_nInputFocus] < m_nSSCardPWDSize)
		{
			m_szPin[m_nInputFocus][m_nPinSize[m_nInputFocus]++] = ch;
		}
		break;
	}
	}
	QString strOldPassword = (char*)m_szPin[m_nInputFocus];
	m_pLineEdit[m_nInputFocus]->setText(strOldPassword);
}
void up_ChangePWD::ThreadWork()
{
	uchar szTemp[16] = { 0 };
	int nRet = 0;

	while (m_bWorkThreadRunning)
	{
		nRet = m_pPinKeybroad->SUNSON_ScanKeyPress(szTemp);
		if (nRet > 0)
			emit InputPin(szTemp[0]);
		this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void up_ChangePWD::on_pushButton_OK_clicked()
{
	if (m_nPinSize[m_nInputFocus] < m_nSSCardPWDSize)
	{
		QString strMessage = QString("输入的密码位数不足%1位,请输入正确的密码!").arg(m_nSSCardPWDSize);
		emit ShowMaskWidget("操作错误", strMessage, Error, Stay_CurrentPage);
	}
	else
	{
		if (m_nInputFocus == 1)
		{
			QString strMessage;
			if (QFailed(CheckPassword(strMessage)))
			{
				ui->lineEdit_Password1->setText("");
				ui->lineEdit_Password2->setText("");
				ZeroMemory(m_szPin[0], sizeof(m_szPin[0]));
				ZeroMemory(m_szPin[1], sizeof(m_szPin[1]));
				m_nPinSize[0] = 0;
				m_nPinSize[1] = 0;
				emit ShowMaskWidget("操作失败", strMessage, Error, Stay_CurrentPage);
				return;
			}
			if (QFailed(ChangePassword(strMessage)))
			{
				emit ShowMaskWidget("操作失败", strMessage, Error, Stay_CurrentPage);
				return;
			}
			emit ShowMaskWidget("操作成功", "密码修改成功,请妥善保管好您的新密码!", Success, Return_MainPage);
		}
		else
		{
			m_nInputFocus = 1;
			m_pLineEdit[m_nInputFocus]->setFocus();
		}
	}
}
