#pragma execution_character_set("utf-8")
#include "up_readsscard.h"
#include "ui_up_readsscard.h"
#include <QMovie>


up_ReadSSCard::up_ReadSSCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::ReadSSCard)
{
	ui->setupUi(this);
	QMovie* movie = new QMovie("./Image/InsertSSCard.gif");
	ui->label_Swipecard->setMovie(movie);
	movie->start();
}

up_ReadSSCard::~up_ReadSSCard()
{
	delete ui;
}

int up_ReadSSCard::ProcessBussiness()
{
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	RegionInfo& reginfo = g_pDataCenter->GetSysConfigure()->Region;
	QString strMessage;
	char szRCode[1024] = { 0 };

	if (QFailed(g_pDataCenter->OpenSSCardReader(DevConfig.strDesktopReaderModule.c_str(),
												DevConfig.strDesktopSSCardReaderPort.c_str(), 
												DevConfig.nDesktopSSCardReaderType, 
												strMessage)))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	if (DriverInit((HANDLE)g_pDataCenter->GetSSCardReader(), (char*)reginfo.strCityCode.c_str(), (char*)reginfo.strSSCardDefaulutPin.c_str(), (char*)reginfo.strPrimaryKey.c_str(), szRCode))
	{
		strMessage = QString("DriverInit失败:%1").arg(szRCode);
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	if (!m_pWorkThread)
	{
		m_bWorkThreadRunning = true;
		m_pWorkThread = new std::thread(&up_ReadSSCard::ThreadWork, this);
		if (!m_pWorkThread)
		{
			QString strError = QString("内存不足,创建读卡线程失败!");
			gError() << strError.toLocal8Bit().data();
			emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
			return -1;
		}
	}

	return 0;
}

void up_ReadSSCard::ShutDown()
{
	m_bWorkThreadRunning = false;
	if (m_pWorkThread && m_pWorkThread->joinable())
	{
		m_pWorkThread->join();
		delete m_pWorkThread;
		m_pWorkThread = nullptr;
	}

	g_pDataCenter->CloseDevice();
}

//int up_ReadSSCard::OpenSSCardReader(QString strLib, ReaderBrand nReaderType, QString& strMessage)
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
//void up_ReadSSCard::CloseSSCardReader()
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

void up_ReadSSCard::ThreadWork()
{
	char szCardATR[128] = { 0 };
	char szRCode[128] = { 0 };
	int nCardATRLen = 0;
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;

	while (m_bWorkThreadRunning)
	{
		if (QFailed(g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nDesktopSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
		{
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		else
		{
			g_pDataCenter->GetSSCardReader()->Reader_PowerOff(DevConfig.nDesktopSSCardReaderPowerOnType, szRCode);
			g_pDataCenter->CloseDevice();

			emit ShowMaskWidget("操作成功", "卡片已插入,稍后请输入旧密码!", Success, Switch_NextPage);
			break;
		}
	}
}

void up_ReadSSCard::OnTimeout()
{
	ShutDown();
}
