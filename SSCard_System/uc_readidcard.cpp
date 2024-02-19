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
	ui->checkBox_SwitchBank->setStyleSheet("QCheckBox::indicator {width: 48px;height: 48px;}\
		QCheckBox::indicator:unchecked{image:url(./Image/CheckBox_UnCheck.png);}\
		QCheckBox::indicator:checked{image:url(./Image/CheckBox_Checked.png);}\
		QCheckBox{font-weight:normal;line-height:49px;letter-spacing:1px;color:#707070;font:42px \"思源黑体 CN Medium\";border-radius: 24px;}");
	//ui->checkBox_SwitchBank->show();
	/*ui->checkBox_Agency->setStyleSheet("QCheckBox::indicator {width: 48px;height: 48px;}\
		QCheckBox::indicator:unchecked{image:url(./Image/CheckBox_UnCheck.png);}\
		QCheckBox::indicator:checked{image:url(./Image/CheckBox_Checked.png);}\
		QCheckBox{font-weight:normal;line-height:49px;letter-spacing:1px;color:#707070;font:42px \"思源黑体 CN Medium\";border-radius: 24px;}");*/
	connect(this, &QStackPage::ErrorMessage, this, &uc_ReadIDCard::OnErrorMessage);
	qDebug() << geometry();
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
	//桌面版不用
#ifndef  _DESKTOP
	if (!g_pDataCenter->OpenCamera())
	{
		gInfo() << "Failed in OpenCamera";
		emit ShowMaskWidget("严重错误", "打开摄像机失败!", Fetal, Return_MainPage);
		return -1;
	}
#endif
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
	//ShowReadCardID();
	disconnect(ui->checkBox_WithoutIDCard, &QCheckBox::stateChanged, this, &uc_ReadIDCard::On_WithoutIDCard);
	if (g_pDataCenter->nCardServiceType == ServiceType::Service_RegisterLost)
	{
		ui->checkBox_WithoutIDCard->hide();
	}
	else
	{
		ui->checkBox_WithoutIDCard->show();
	}
	ui->checkBox_SwitchBank->setChecked(false);
	if (g_pDataCenter->nCardServiceType == ServiceType::Service_NewCard)
	{
		ui->checkBox_SwitchBank->show();
	}
	else
	{
		ui->checkBox_SwitchBank->hide();
		
	}

	StartDetect();
	ui->checkBox_WithoutIDCard->setChecked(false);
	if (!(g_pDataCenter->nCardServiceType == ServiceType::Service_NewCard))
	{
		ui->checkBox_SwitchBank->setChecked(false);
	}
	
	m_bAgency = false;
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
	QString strMessage;
	while (m_bWorkThreadRunning)
	{
		auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
		if (tDuration.count() >= 1000)
		{
			tLast = high_resolution_clock::now();
			int nResult = g_pDataCenter->ReaderIDCard(m_pIDCard.get());
			if (nResult == IDCard_Status::IDCard_Succeed)
			{
				g_pDataCenter->SetIDCardInfo(m_pIDCard);

				if (QSucceed(GetImageStorePath(g_pDataCenter->strIDImageFile)))
				{
					QImage ImagePhoto = QImage::fromData(m_pIDCard->szPhoto, m_pIDCard->nPhotoSize);
					ImagePhoto.save(QString::fromLocal8Bit(g_pDataCenter->strIDImageFile.c_str()));
				}

				int nNewPage = Page_FaceCapture;
				int nOperation = Goto_Page;

				QString strProgressFile = QString("%1/data/Progress_%2.json").arg(QCoreApplication::applicationDirPath()).arg((char*)m_pIDCard->szIdentity);
				if (fs::exists(strProgressFile.toStdString()))
				{
					strMessage = "读取身份证成功,业务流程已开始!";
					switch (nReadIDType)
					{
					case ReadID_NewCard:
						nNewPage = Page_CommitNewInfo;
						break;
					case ReadID_UpdateCard:
						nNewPage = Page_EnsureInformation;
						break;
					case ReadID_RegisterLost:
						nNewPage = Page_RegisterLost;
						strMessage = "读取身份证成功,稍后请进行挂失/解挂操作!";
						break;
					case ReadID_QueryInfo:
						nNewPage = Page_QueryInformation;
						strMessage = "读取身份证成功,稍后请进行信息查询!";
						break;
					}
				}
				else
				{
					strMessage = "读取身份证成功,稍后将进行人脸识别以确认是否本人操作!";
					switch (nReadIDType)
					{
					case ReadID_NewCard:
					case ReadID_UpdateCard:
						break;
					case ReadID_RegisterLost:
						nNewPage = Page_RegisterLost;
						strMessage = "读取身份证成功,稍后请进行挂失/解挂操作!";
						break;
					case ReadID_QueryInfo:
						nNewPage = Page_QueryInformation;
						strMessage = "读取身份证成功,稍后请进行信息查询!";
						break;
					}
				}

				gInfo() << gQStr(strMessage);
				g_pDataCenter->SetIDCardInfo(m_pIDCard);
				emit ShowMaskWidget("操作成功", strMessage, Success, nOperation, nNewPage);
				break;
			}
			else
			{
				char szText[256] = { 0 };
				GetErrorMessage((IDCard_Status)nResult, szText, sizeof(szText));
				strMessage = QString("读取身份证失败:%1").arg(szText);
				gInfo() << gQStr(strMessage);
				emit ErrorMessage(strMessage);
			}
		}
		else
		{
			this_thread::sleep_for(chrono::milliseconds(200));
		}
	}
}

void uc_ReadIDCard::timerEvent(QTimerEvent* event)
{

}

void uc_ReadIDCard::On_WithoutIDCard(int arg1)
{
	MainWindow* pMainWind = (MainWindow*)qApp->activeWindow();
	if (pMainWind && pMainWind->pLastStackPage)
	{
		g_pDataCenter->bWithoutIDCard = true;
		pMainWind->pLastStackPage->emit SwitchPage(Switch_NextPage);
	}
}

void uc_ReadIDCard::on_checkBox_SwitchBank_stateChanged(int arg1)
{
	g_pDataCenter->bSwitchBank = ui->checkBox_SwitchBank->isChecked();
	//if (g_pDataCenter->bSwitchBank)
	//{
	//	g_pDataCenter->nCardServiceType = ServiceType::Service_ReplaceCardSwitchBank;
	//}
	//else {
	//	g_pDataCenter->nCardServiceType = ServiceType::Service_NewCard;
	//}
}

