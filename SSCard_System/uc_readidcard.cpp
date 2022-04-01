#pragma execution_character_set("utf-8")
#include "uc_readidcard.h"
#include "ui_uc_readidcard.h"
#include "Gloabal.h"
#include <chrono>
#include <algorithm>
#include "Payment.h"
#include "MaskWidget.h"
#include "uc_inputidcardinfo.h"
#include <QMovie>
extern MaskWidget* g_pMaskWindow;

#include "mainwindow.h"
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

	ui->checkBox_WithoutIDCard->setStyleSheet("QCheckBox::indicator {width: 48px;height: 48px;}\
		QCheckBox::indicator:unchecked{image:url(./Image/CheckBox_UnCheck.png);}\
		QCheckBox::indicator:checked{image:url(./Image/CheckBox_Checked.png);}\
		QCheckBox{font-weight:normal;line-height:49px;letter-spacing:1px;color:#707070;font:42px \"思源黑体 CN Medium\";border-radius: 24px;}");
	connect(this, &QStackPage::ErrorMessage, this, &uc_ReadIDCard::OnErrorMessage);

}

uc_ReadIDCard::~uc_ReadIDCard()
{
	ShutDown();
	delete ui;
}

void uc_ReadIDCard::StartDetect()
{
	if (!m_pWorkThread)
	{
		m_bWorkThreadRunning = true;
		m_pWorkThread = new std::thread(&uc_ReadIDCard::ThreadWork, this);
		if (!m_pWorkThread)
		{
			QString strError = QString("内存不足,创建读卡线程失败!");
			gError() << strError.toLocal8Bit().data();
			emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
			return;
		}
	}
}

void uc_ReadIDCard::StopDetect()
{
	m_bWorkThreadRunning = false;
	if (m_pWorkThread && m_pWorkThread->joinable())
	{
		m_pWorkThread->join();
		m_pWorkThread = nullptr;
	}
	CloseReader();
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

	m_pIDCard = make_shared<IDCardInfo>();
	//g_pDataCenter->SetIDCardInfo(m_pIDCard);;

	SysConfigPtr& pConfigure = g_pDataCenter->GetSysConfigure();
	m_strDevPort = pConfigure->DevConfig.strIDCardReaderPort;
	transform(m_strDevPort.begin(), m_strDevPort.end(), m_strDevPort.begin(), ::toupper);
	ShowReadCardID();
	disconnect(ui->checkBox_WithoutIDCard, &QCheckBox::stateChanged, this, &uc_ReadIDCard::On_WithoutIDCard);

	StartDetect();
	ui->checkBox_WithoutIDCard->setChecked(false);
	connect(ui->checkBox_WithoutIDCard, &QCheckBox::stateChanged, this, &uc_ReadIDCard::On_WithoutIDCard);
	return 0;
}

void uc_ReadIDCard::OnTimeout()
{
	ShutDown();
}

void  uc_ReadIDCard::ShutDown()
{
	gInfo() << __FUNCTION__;
	StopDetect();
	//on_checkBox_WithoutIDCard_stateChanged(0);

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
			break;
		}
		bDevOpened = true;
		nResult = FindIDCard();
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}

		nResult = ReadIDCard(m_pIDCard.get());
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

void uc_ReadIDCard::ShowReadCardID()
{
	ui->label_Swipecard->show();
	ui->label_Notify->show();
	ui->checkBox_WithoutIDCard->show();
	if (pInputIDCardWidget)
	{
		pInputIDCardWidget->ResetPage();
		pInputIDCardWidget->setParent(nullptr);
		pInputIDCardWidget->hide();
		ui->horizontalLayout_2->removeWidget(pInputIDCardWidget);
		disconnect(this, &uc_ReadIDCard::ShowNationWidget, this, &uc_ReadIDCard::on_ShowNationWidget);
	}
}

void uc_ReadIDCard::ShowInputCardID()
{
	ui->label_Swipecard->hide();
	ui->label_Notify->hide();
	ui->checkBox_WithoutIDCard->hide();
	if (!pInputIDCardWidget)
	{
		pInputIDCardWidget = new uc_InputIDCardInfo(this);
		connect(this, &uc_ReadIDCard::ShowNationWidget, this, &uc_ReadIDCard::on_ShowNationWidget);
	}
	pInputIDCardWidget->setParent(this);
	ui->horizontalLayout_2->addWidget(pInputIDCardWidget);
	pInputIDCardWidget->show();

}
void uc_ReadIDCard::On_WithoutIDCard(int arg1)
{
	QWaitCursor Wait;
	if (arg1)
	{
		ShowInputCardID();
		StopDetect();
	}
	else
	{
		//ui->label_Swipecard->setParent(this);
		//ui->horizontalLayout_2->addWidget(ui->label_Swipecard);
		ShowReadCardID();
		StartDetect();
	}
	MainWindow* pMainWind = (MainWindow*)qApp->activeWindow();
	if (pMainWind && pMainWind->pLastStackPage)
	{
		pMainWind->pLastStackPage->ResetTimer(arg1 != 0, this);
	}
}

void uc_ReadIDCard::RemoveUI()
{

}

void uc_ReadIDCard::on_ShowNationWidget(bool bShow)
{
	if (!pQNationWidget)
	{
		pQNationWidget = new QNationWidget(this);
	}
	QRect rt;
	if (bShow)
	{
		pQNationWidget->setParent(this);
		ui->horizontalLayout_2->addWidget(pQNationWidget);
		pQNationWidget->show();
		if (pInputIDCardWidget)
		{
			pInputIDCardWidget->setParent(nullptr);
			pInputIDCardWidget->hide();
			ui->horizontalLayout_2->removeWidget(pInputIDCardWidget);
		}
		rt = ui->horizontalSpacer_3->geometry();
	}
	else
	{
		if (pInputIDCardWidget)
		{
			pInputIDCardWidget->setParent(this);
			ui->horizontalLayout_2->addWidget(pInputIDCardWidget);
			pInputIDCardWidget->show();
		}
		pQNationWidget->setParent(nullptr);
		pQNationWidget->hide();
		ui->horizontalLayout_2->removeWidget(pQNationWidget);
		rt = ui->horizontalSpacer_3->geometry();
	}
	adjustSize();
}
