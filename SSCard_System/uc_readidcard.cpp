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
	/*ui->checkBox_Agency->setStyleSheet("QCheckBox::indicator {width: 48px;height: 48px;}\
		QCheckBox::indicator:unchecked{image:url(./Image/CheckBox_UnCheck.png);}\
		QCheckBox::indicator:checked{image:url(./Image/CheckBox_Checked.png);}\
		QCheckBox{font-weight:normal;line-height:49px;letter-spacing:1px;color:#707070;font:42px \"思源黑体 CN Medium\";border-radius: 24px;}");*/
	connect(this, &QStackPage::ErrorMessage, this, &uc_ReadIDCard::OnErrorMessage);
	connect(this, &uc_ReadIDCard::UseFinishedProgress, this, &uc_ReadIDCard::on_UseFinishedProgress,Qt::BlockingQueuedConnection);
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
			emit ShowMaskWidget("严重错误", strError, Fatal, Return_MainPage);
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
		emit ShowMaskWidget("严重错误", "打开摄像机失败!", Fatal, Return_MainPage);
		return -1;
	}

	/*if (g_pMaskWindow)
		g_pMaskWindow->hide();*/
	QSize WindowsSize = size();
	qDebug() << "Height = " << WindowsSize.height() << "Width = " << WindowsSize.width();
	m_bSucceed = false;

	SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
	g_pDataCenter->SetSSCardInfo(pSSCardInfo);

	m_pIDCard = make_shared<IDCardInfo>();
	//g_pDataCenter->SetIDCardInfo(m_pIDCard);;

	SysConfigPtr& pConfigure = g_pDataCenter->GetSysConfigure();
	m_strDevPort = pConfigure->DevConfig.strIDCardReaderPort;
	transform(m_strDevPort.begin(), m_strDevPort.end(), m_strDevPort.begin(), ::toupper);
	//ShowReadCardID();

	disconnect(ui->checkBox_WithoutIDCard, &QCheckBox::stateChanged, this, &uc_ReadIDCard::On_WithoutIDCard);
	if (g_pDataCenter->nCardServiceType == ServiceType::Service_NewCard)
		ui->checkBox_WithoutIDCard->show();
	else
		ui->checkBox_WithoutIDCard->hide();
	StartDetect();
	ui->checkBox_WithoutIDCard->setChecked(false);
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
	auto tTimeTick = tLast;
	QString strMessage;
	while (m_bWorkThreadRunning)
	{
		this_thread::sleep_for(chrono::milliseconds(200));
		auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
		if (tDuration.count() >= 1000)
		{
			int nResult = -1;
			tLast = high_resolution_clock::now();
			if (g_pDataCenter->bDebug)
			{
				auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tTimeTick);
				if (tDuration.count() < 10 * 1000)
					continue;
				QString strApp = qApp->applicationDirPath() + "/debug/Person_Test.json";
				nResult = LoadPersonInfo(strApp,m_pIDCard);
			}
			else
				nResult = g_pDataCenter->ReaderIDCard(m_pIDCard.get());
			
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
				SSCardInfoPtr pSSCardInfo = nullptr;
				QString strMessage = "读取身份证成功,稍后将进行人脸识别以确认是否本人操作!";
				// 近回文件类型，文件名和json指针(若文件存在)
				auto tpProgressInfo = FindCardProgress((const char*)m_pIDCard->szIdentity, pSSCardInfo);
#ifdef HN2022
				strcpy((char*)pSSCardInfo->strIDCardIssuedDate, (char *)m_pIDCard->szExpirationDate1);
#endif
				switch (std::get<0>(tpProgressInfo))
				{
				default:
				case ProgrerssType::Progress_UnStart:
				{
					// 开启新流程					
					switch (g_pDataCenter->nCardServiceType)
					{
					case ServiceType::Service_NewCard:
					case ServiceType::Service_ReplaceCard:
					{// 创建新的流程
						auto optProgressFile = g_pDataCenter->GetProgressFile(m_pIDCard);
						auto optProgressJson = g_pDataCenter->OpenProgress(optProgressFile.value(),true);
						if (optProgressJson)
						{
							std::get<1>(tpProgressInfo) = optProgressFile.value();
							std::get<2>(tpProgressInfo) = optProgressJson.value();
							g_pDataCenter->SetProgress(tpProgressInfo);
						}
							
						break;
					}
					default:
						nOperation = Switch_NextPage;
						nNewPage = 0;
						strMessage = "读取身份证成功,稍后请进行挂失/解挂操作!";
						break;
					}
					break;
				}
					
				case ProgrerssType::Progress_Making:
				{
					g_pDataCenter->SetSSCardInfo(pSSCardInfo);
					g_pDataCenter->SetProgress(tpProgressInfo);
					g_pDataCenter->strCardMakeProgress = "制卡中";
					//strMessage = QString("\t\t姓名:%1\n\t\t身份证:%2\n\t\t社保卡号:%3\n\t\t日期:%4").arg((char*)m_pIDCard->szName).arg((char*)m_pIDCard->szIdentity).arg(pSSCardInfo->strCardNum).arg(pSSCardInfo->strReleaseDate);
					strMessage = QString("发现当前用户未完成制卡数据,现将转入制卡页面");
					nOperation = Goto_Page;
					nNewPage = Page_MakeCard;
					nResult = 0;
					break;
				}
				case ProgrerssType::Progress_Finished:
				{// 使用现有流程
					if (emit UseFinishedProgress())
					{
						g_pDataCenter->SetSSCardInfo(pSSCardInfo);
						g_pDataCenter->SetProgress(tpProgressInfo);
						//strMessage = QString("\t\t姓名:%1\n\t\t身份证:%2\n\t\t社保卡号:%3\n\t\t日期:%4").arg((char*)m_pIDCard->szName).arg((char*)m_pIDCard->szIdentity).arg(pSSCardInfo->strCardNum).arg(pSSCardInfo->strReleaseDate);
						//strMessage = QString("发现当前用户未完成制卡数据\n\%1,现将转入制卡页面").arg(strUserInfo);
						nOperation = Goto_Page;
						nNewPage = Page_MakeCard;
						nResult = 0;
					}
					else // 使用新进度文件
					{
						auto optProgressFile = g_pDataCenter->GetProgressFile(m_pIDCard);
						auto optProgressJson = g_pDataCenter->OpenProgress(optProgressFile.value(),true);
						if (optProgressJson)
						{
							std::get<1>(tpProgressInfo) = optProgressFile.value();
							std::get<2>(tpProgressInfo) = optProgressJson.value();
							g_pDataCenter->SetProgress(tpProgressInfo);
						}
					}
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
		pMainWind->pLastStackPage->emit SwitchPage(Goto_Page, Page_InputIDCardInfo);
	}
}

bool uc_ReadIDCard::on_UseFinishedProgress()
{
	return QMessageBox_CN(QMessageBox::Question, "询问", "系统中存在一个已经完成的制卡进度文件，是否需使用该进度?如果是仅打印卡面选择是!", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
}