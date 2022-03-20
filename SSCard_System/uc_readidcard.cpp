#pragma execution_character_set("utf-8")
#include "uc_readidcard.h"
#include "ui_uc_readidcard.h"
#include "Gloabal.h"
#include <chrono>
#include <algorithm>
#include "Payment.h"
#include "MaskWidget.h"
#include <QMovie>
extern MaskWidget* g_pMaskWindow;
//#include "mainwindow.h"
using namespace std;
using namespace chrono;

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")
//#pragma comment(lib,"SDK/IDCard/IDCard_API")

uc_ReadIDCard::uc_ReadIDCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, ReadID_Type nType, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::ReadIDCard),
	nReadIDType(nType)
{
	ui->setupUi(this);
	QMovie* movie = new QMovie("./Image/SwipeIDCard.gif");
	ui->label_Swipecard->setMovie(movie);
	movie->start();
	connect(this, &QStackPage::ErrorMessage, this, &uc_ReadIDCard::OnErrorMessage);
}

uc_ReadIDCard::~uc_ReadIDCard()
{
	ShutDown();
	delete ui;
}


int uc_ReadIDCard::ProcessBussiness()
{
	if (!g_pDataCenter->OpenCamera())
	{
		gInfo() << "Failed in OpenCamera";
		emit ShowMaskWidget("严重错误", "打开摄像机失败!", Fetal, Return_MainPage);
		return -1;
	}
	/*if (g_pMaskWindow)
		g_pMaskWindow->hide();*/
	QSize WindowsSize = size();
	qDebug() << "Height = " << WindowsSize.height() << "Width = " << WindowsSize.width();
	m_bSucceed = false;

	SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
	g_pDataCenter->SetSSCardInfo(pSSCardInfo);

	IDCardInfoPtr pIDCardInfo = make_shared<IDCardInfo>();
	g_pDataCenter->SetIDCardInfo(pIDCardInfo);

	m_pIDCard = g_pDataCenter->GetIDCardInfo();

	SysConfigPtr& pConfigure = g_pDataCenter->GetSysConfigure();
	m_strDevPort = pConfigure->DevConfig.strIDCardReaderPort;
	transform(m_strDevPort.begin(), m_strDevPort.end(), m_strDevPort.begin(), ::toupper);
	if (!m_pWorkThread)
	{
		m_bWorkThreadRunning = true;
		m_pWorkThread = new std::thread(&uc_ReadIDCard::ThreadWork, this);
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

void uc_ReadIDCard::OnTimeout()
{
	ShutDown();
}

void  uc_ReadIDCard::ShutDown()
{
	gInfo() << __FUNCTION__;

	m_bWorkThreadRunning = false;
	if (m_pWorkThread && m_pWorkThread->joinable())
	{
		m_pWorkThread->join();
		m_pWorkThread = nullptr;
	}
	m_pIDCard = nullptr;
	CloseReader();
}

void uc_ReadIDCard::OnErrorMessage(QString strErrorMsg)
{
	ui->label_Notify->setText(strErrorMsg);
}

void uc_ReadIDCard::ThreadWork()
{
	auto tLast = high_resolution_clock::now();
	QString strError;
	while (m_bWorkThreadRunning)
	{
		auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
		if (tDuration.count() >= 1000)
		{
			tLast = high_resolution_clock::now();
			int nResult = ReaderIDCard();
			if (nResult == IDCard_Status::IDCard_Succeed)
			{
				if (nReadIDType == ReadID_UpdateCard)
					strError = "读取身份证成功,稍后将进行人脸识别以确认是否本人操作!";
				else if (nReadIDType == ReadID_RegisterLost)
					strError = "读取身份证成功,稍后请进行挂失/解挂操作!";
				gInfo() << strError.toLocal8Bit().data();
				emit ShowMaskWidget("操作成功", strError, Success, Switch_NextPage);
				break;
			}
			else
			{
				char szText[256] = { 0 };
				GetErrorMessage((IDCard_Status)nResult, szText, sizeof(szText));
				strError = QString("读取身份证失败:%1").arg(szText);
				emit ErrorMessage(strError);
				//emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);
				gError() << strError.toLocal8Bit().data();
			}
		}
		else
		{
			this_thread::sleep_for(chrono::milliseconds(200));
		}
	}
}

// int  uc_ReadIDCard::GetIDImageStorePath(string& strFilePath)
// {
// 	QString strStorePath = QCoreApplication::applicationDirPath();
// 	strStorePath += "/IDImage/";
// 	strStorePath += QDateTime::currentDateTime().toString("yyyyMMdd/");
// 	QFileInfo fi(strStorePath);
// 	if (!fi.isDir())
// 	{// 当天目录不存在？则创建目录
// 		QDir storeDir;
// 		if (!storeDir.mkpath(strStorePath))
// 		{
// 			char szError[1024] = { 0 };
// 			_strerror_s(szError, 1024, nullptr);
// 			QString Info = QString("创建身份证照片保存目录'%1'失败:%2").arg(strStorePath, szError);
// 			gInfo() << Info.toLocal8Bit().data();
// 			return -1;
// 		}
// 	}
// 	QString strTempPath = strStorePath + QString("ID_%1.bmp").arg((const char*)g_pDataCenter->GetIDCardInfo()->szIdentify);
// 	strFilePath = strTempPath.toStdString();
// 	return 0;
// }

#define    Error_Not_IDCARD         (-1)

int uc_ReadIDCard::ReaderIDCard()
{
	int nResult = IDCard_Status::IDCard_Succeed;
	bool bDevOpened = false;
	do
	{
		if (m_bSucceed)
			break;
		//if (m_bSucceed && m_nDelayCount <= 3)
		//{
		//	m_nDelayCount++;
		//	break;
		//}
		//else
		//{
		//	m_nDelayCount = 0;
		//	m_bSucceed = false;
		//}

		if (m_strDevPort == "AUTO" || !m_strDevPort.size())
		{
			gInfo() << "Try to open IDCard Reader auto!" << m_strDevPort;
			nResult = OpenReader(nullptr);
		}
		else
		{
			gInfo() << "Try to open IDCard Reader " << m_strDevPort;
			nResult = OpenReader(m_strDevPort.c_str());
		}

		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			bDevOpened = true;
			break;
		}
		nResult = FindIDCard();
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}

		nResult = ReadIDCard(*m_pIDCard.get());
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		int nNationalityCode = strtol((char*)m_pIDCard->szNationaltyCode, nullptr, 10);
		if (nNationalityCode < 10)
			sprintf_s((char*)m_pIDCard->szNationaltyCode, sizeof(m_pIDCard->szNationaltyCode), "%02d", nNationalityCode);
		g_pDataCenter->SetIDCardInfo(m_pIDCard);

		if (QSucceed(GetImageStorePath(g_pDataCenter->strIDImageFile)))
		{
			QImage ImagePhoto = QImage::fromData(m_pIDCard->szPhoto, m_pIDCard->nPhotoSize);
			ImagePhoto.save(QString::fromLocal8Bit(g_pDataCenter->strIDImageFile.c_str()));
		}
		m_bSucceed = true;
	} while (0);
	if (bDevOpened)
	{
		gInfo() << "Try to close IDCard Reader!";
		CloseReader();
	}

	return nResult;
}
void uc_ReadIDCard::timerEvent(QTimerEvent* event)
{

}
