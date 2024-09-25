#pragma execution_character_set("utf-8")
#include "uc_makecard.h"
#include "ui_uc_makecard.h"
#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;
#include "Gloabal.h"
#include "Payment.h"
#pragma warning(disable:4189)

uc_MakeCard::uc_MakeCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::MakeCard)
{
	ui->setupUi(this);
	m_nTimeout = 300;

	m_LableStep.push_back(ui->label_MakeCard);
	//m_LableStep.push_back(ui->label_Depanse);
	m_LableStep.push_back(ui->label_Write);
	m_LableStep.push_back(ui->label_Print);
	m_LableStep.push_back(ui->label_ReturnData);
	m_LableStep.push_back(ui->label_Reject);

	connect(this, &uc_MakeCard::UpdateProgress, this, &uc_MakeCard::OnUpdateProgress, Qt::BlockingQueuedConnection);
	connect(this, &uc_MakeCard::EnableButtonOK, this, &uc_MakeCard::on_EnableButtonOK, Qt::BlockingQueuedConnection);
}

uc_MakeCard::~uc_MakeCard()
{
	ShutDown();
	delete ui;
}

void uc_MakeCard::OnUpdateProgress(int nStep)
{
	qDebug() << __FUNCTION__ << "nStep = " << nStep;
	if (nStep >= 0 && nStep < m_LableStep.size())
		m_LableStep[nStep]->setStyleSheet(QString::fromUtf8("image: url(:/Image/Status_Finish.png);"));
}

void uc_MakeCard::ShowSSCardInfo()
{
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	ui->lineEdit_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName));
	ui->lineEdit_CardID->setText(pSSCardInfo->strCardID);
	ui->lineEditl_SSCard->setText(pSSCardInfo->strCardNum);
	//ui->lineEdit_Bank->setText(pSSCardInfo->strBankNum);
	int nYear = 0, nMonth = 0, nDay;
	sscanf_s(pSSCardInfo->strReleaseDate, "%04d%02d%02d", &nYear, &nMonth, &nDay);
	char szReleaseDate[32] = { 0 };
	sprintf_s(szReleaseDate, 32, "%d年%d月", nYear, nMonth);
	ui->lineEdit_Datetime->setText(szReleaseDate);

	QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
	ui->label_Pohoto->setStyleSheet(strStyle);
}

//void uc_MakeCard::registerPaymentTemp()
//{
//	SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
//	SSCardInfoPtr& pSSCardInfo1 = g_pDataCenter->GetSSCardInfo();
//	
//	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
//	strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
//	strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
//	strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
//	strcpy((char*)pSSCardInfo->strTransType, "5");
//	strcpy((char*)pSSCardInfo->strCity, Reginfo.strCityCode.c_str());
//	strcpy((char*)pSSCardInfo->strSSQX, Reginfo.strCountry.c_str());
//	strcpy((char*)pSSCardInfo->strCardVender, Reginfo.strCardVendor.c_str());
//	strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
//	strcpy((char*)pSSCardInfo->strPayCode, g_pDataCenter->strPayCode.c_str());
//	strcpy((char*)pSSCardInfo->strTransactionTime, g_pDataCenter->strTransTime.c_str());
//
//	strcpy((char*)pSSCardInfo->strCardID, pSSCardInfo1->strCardID);
//	strcpy((char*)pSSCardInfo->strName, pSSCardInfo1->strName);
//	strcpy((char*)pSSCardInfo->strCity, pSSCardInfo1->strCity);
//	strcpy((char*)pSSCardInfo->strPayCode, "41000024100022372382");
//	strcpy((char*)pSSCardInfo->strTransactionTime, "2024-04-18 10:08:26");
//
//	QString strMessage;
//	int nStatus = 0;
//	ResgisterPayment(strMessage, nStatus, pSSCardInfo);          // 缴费登记
//}
int uc_MakeCard::ProcessBussiness()
{
	for (auto var : m_LableStep)
		var->setStyleSheet(QString::fromUtf8("image: url(:/Image/todo.png);"));
	m_nSocketRetryInterval = g_pDataCenter->GetSysConfigure()->PaymentConfig.nQueryPayResultInterval;            // 支付结构查询时间间隔单 毫秒
	m_nSocketRetryCount = g_pDataCenter->GetSysConfigure()->PaymentConfig.nSocketRetryCount;
	int nResult = -1;
	QString strMessage;

	ui->pushButton_OK->setText("确定");
	if (!g_pDataCenter->GetPrinter())
	{
		if (QFailed(g_pDataCenter->OpenPrinter(strMessage)))
		{
			gError() << gQStr(strMessage);
			emit ShowMaskWidget("操作失败", strMessage, Fatal, Return_MainPage);
			return nResult;
		}
	}
	if (!g_pDataCenter->GetSSCardReader())
	{
		if (QFailed(g_pDataCenter->OpenSSCardReader(strMessage)))
		{
			gError() << gQStr(strMessage);
			emit ShowMaskWidget("操作失败", strMessage, Fatal, Return_MainPage);
			return nResult;
		}
	}
	ui->pushButton_OK->setEnabled(true);
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
	strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
	strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
	strcpy((char*)pSSCardInfo->strTransType, "5");
	strcpy((char*)pSSCardInfo->strCity, Reginfo.strCityCode.c_str());
	strcpy((char*)pSSCardInfo->strSSQX, Reginfo.strCountry.c_str());
	strcpy((char*)pSSCardInfo->strCardVender, Reginfo.strCardVendor.c_str());
	strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
	if (!g_pDataCenter->strPayCode.size())
	{
		char szPayCode[64] = { 0 };
		if (g_pDataCenter->GetProgressField("payCode", szPayCode))
			g_pDataCenter->strPayCode = szPayCode;
		else
		{
			emit ShowMaskWidget("操作失败", "缴费码为空!", Fatal, Return_MainPage);
			return -1;
		}
	}
	if (!g_pDataCenter->strTransTime.size())
	{
		char szTranTime[64] = { 0 };
		if (g_pDataCenter->GetProgressField("transTime", szTranTime))
			g_pDataCenter->strTransTime = szTranTime;
		else
		{
			emit ShowMaskWidget("操作失败", "缴费时间为空!", Fatal, Return_MainPage);
			return -1;
		}
	}
		
	strcpy((char*)pSSCardInfo->strPayCode, g_pDataCenter->strPayCode.c_str());
	strcpy((char*)pSSCardInfo->strTransactionTime, g_pDataCenter->strTransTime.c_str());
	ZeroMemory(StepStatus, sizeof(StepStatus));
	int nStatus = 0;
	//registerPaymentTemp();
	ResgisterPayment(strMessage, nStatus,pSSCardInfo);          // 缴费登记
	if (g_pDataCenter->strCardMakeProgress == "制卡中")
	{
		if (QFailed(PrecessCardInMaking(strMessage)))
		{
			emit ShowMaskWidget("操作失败", strMessage, Fatal, Return_MainPage);
			return -1;
		}
		StepStatus[Step_Mark] = true;
	}
	else
	{
		if (QFailed(PrepareMakeCard(strMessage)))
		{
			emit ShowMaskWidget("操作失败", strMessage, Fatal, Return_MainPage);
			return -1;
		}
		StepStatus[Step_PreMake] = true;
	}
	//emit UpdateProgress(MP_PreMakeCard);
	OnUpdateProgress(MP_PreMakeCard);
	ShowSSCardInfo();

	return 0;
}

void uc_MakeCard::OnTimeout()
{
	ShutDown();
}

void  uc_MakeCard::ShutDown()
{
	gInfo() << __FUNCTION__;
	m_bWorkThreadRunning = false;
	if (m_pWorkThread && m_pWorkThread->joinable())
	{
		m_pWorkThread->join();
		m_pWorkThread = nullptr;
	}
	//g_pDataCenter->CloseDevice();
}

int uc_MakeCard::PrecessCardInMaking(QString& strMessage)
{
	gInfo() << __FUNCTION__;
	int nStatus = 0;
	int nResult = 0;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	QString strCardProgress = QString::fromLocal8Bit(pSSCardInfo->strCardStatus);

	int nCardProgress = 0;
	do
	{
		if (!g_pDataCenter->GetProgressStatus("MarkCard"))		// 未标注
		{
			if (QFailed(nResult = MarkCard(strMessage, nStatus, pSSCardInfo)))
			{
				if (strMessage == "已经开始制卡")
				{
					if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
					{
						if (strMessage.contains("已经有批次号"))
						{
							strMessage += "\n请转到后台以写卡失败选项手动制卡!";
							break;
						}
						if (strMessage.contains("批次确认加锁失败"))
						{
							strMessage += ",需社保局后台更新数据,请在2小时后再尝试制卡!";
						}
						break;
						/*if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus)))
							break;*/
					}

					if (nStatus != 0 && nStatus != 1)
					{
						nResult = -1;
						break;
					}
					break;
				}
				else
					break;
			}
			if (nStatus != 0 && nStatus != 1)
			{
				nResult = -1;
				break;
			}
			g_pDataCenter->SetProgressStatus("MarkCard", 1);
		}
		else
			nResult = 0;
		//if (!g_pDataCenter->GetProgressStatus("GetCardData"))		// 未取得卡数据
		{
			// GetCardData 内部作了处理，可以自动从进度文件中取得数据
			if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
			{
				break;
			}

			if (nStatus != 0 && nStatus != 1)
			{
				nResult = -1;
				break;
			}
			g_pDataCenter->SetProgressStatus("GetCardData", 1);
		}
// 		else
// 		{
// 			g_pDataCenter->LoadSSCardData()
// 			nResult = 0;
// 		}
			
		
	} while (0);
	return nResult;
}

int uc_MakeCard::PrepareMakeCard(QString& strMessage)
{
	int nStatus = 0;
	int nResult = -1;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	do
	{
		string strProgress = "";
		if (g_pDataCenter->nCardServiceType == ServiceType::Service_NewCard)
		{
			if (!pSSCardInfo->strPhoto)
			{
				QByteArray baPhoto;
				QFileInfo fi(g_pDataCenter->strSSCardPhotoFile.c_str());
				if (fi.isFile())
				{
					QFile qfile(g_pDataCenter->strSSCardPhotoFile.c_str());
					if (qfile.open(QIODevice::ReadOnly))
					{
						baPhoto = qfile.readAll().toBase64();
						g_pDataCenter->strPhotoBase64 = baPhoto.data();
						pSSCardInfo->strPhoto = (char*)g_pDataCenter->strPhotoBase64.c_str();
					}
				}
			}
			if (!g_pDataCenter->GetProgressStatus("AppNewCard"))		// 未取得卡数据
			{
				strProgress = "ApplyNewCard";
				nResult = ApplyNewCard(strMessage, nStatus, pSSCardInfo);			 // 申请 新制卡
			}
			else
				nResult = 0;
		}
		else
		{
			if (!g_pDataCenter->GetProgressStatus("ApplyCardReplacement"))		// 未取得卡数据
			{
				strProgress = "ApplyCardReplacement";
				nResult = ApplyCardReplacement(strMessage, nStatus, pSSCardInfo);     //  申请补换卡
			}
			else
				nResult = 0;
			
		}
		if (QFailed(nResult))
			break;

		if (nStatus != 0 && nStatus != 1)
		{
			nResult = -1;
			break;
		}
		g_pDataCenter->SetProgressStatus(strProgress, 1);
		if (!g_pDataCenter->GetProgressStatus("MarkCard"))
		{
			nResult = MarkCard(strMessage, nStatus, pSSCardInfo);
			if (QFailed(nResult))
			{
				break;
			}
			if (nStatus != 0 && nStatus != 1)
			{
				strMessage = QString("即制卡标注失败:%1!").arg(nStatus);
				nResult = -1;
				break;
			}
			g_pDataCenter->SetProgressStatus("MarkCard", 1);
		}
		else
			nResult = 0;

		//if (!g_pDataCenter->GetProgressStatus("GetCardData"))
		{
			if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
			{
				if (strMessage.contains("批次"))
				{
					strMessage += ",需社保局后台更新数据,请在2小时后再尝试制卡!";
					break;
				}

				if (nStatus != 0 && nStatus != 1)
				{
					strMessage = QString("因获取制卡数据失败:%1!").arg(strMessage);
					nResult = -1;
					break;
				}
				nResult = -1;
			}
			if (nStatus != 0 && nStatus != 1)
			{
				nResult = -1;
				break;
			}
			g_pDataCenter->SetProgressStatus("GetCardData", 1);
		}
		//else
		//	nResult = 0;

	} while (0);
	return nResult;
}

#define GBKString(x)	QString::fromLocal8Bit(x).toStdString();
void uc_MakeCard::ThreadWork()
{
	char szRCode[32] = { 0 };
	int nResult = -1;
	QString strMessage;
	auto& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szBuffer[1024] = { 0 };
	int nBufferSize = sizeof(szBuffer);
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	int nStatus = 0;
	QString strInfo;

	do
	{
		nResult = -1;
		if (!g_pDataCenter->GetProgressStatus("WriteCard"))
			//if (!StepStatus[Step_WriteCard])
		{
			if (QFailed(g_pDataCenter->SafeWriteCard(strMessage)))
			{
				strMessage = QString("%1,稍后,您可以点击重试按钮或更换卡片后继续制卡!");
				gError() << gQStr(strMessage);
				break;
			}
			g_pDataCenter->SetProgressField("IdentifyNum", pSSCardInfo->strIdentifyNum);
			g_pDataCenter->SetProgressField("CardATR", pSSCardInfo->strCardATR);
			g_pDataCenter->SetProgressField("BankNum", pSSCardInfo->strBankNum);
			g_pDataCenter->SetProgressStatus("WriteCard", 1);
			StepStatus[Step_WriteCard] = true;
		}
		else
		{
			g_pDataCenter->GetProgressField("IdentifyNum", pSSCardInfo->strIdentifyNum);
			g_pDataCenter->GetProgressField("CardATR", pSSCardInfo->strCardATR);
			g_pDataCenter->GetProgressField("BankNum", pSSCardInfo->strBankNum);
			gInfo() << GBKString("写卡成功");
		}

		emit UpdateProgress(MP_WriteCard);
		if (!g_pDataCenter->GetProgressStatus("PrintCard"))
		//if (!StepStatus[Step_PrintCard])
		{
			if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
			{
				strMessage = "%1,请检查制卡机后,然后点击重试以继续完成卡片制作!";
				return;
			}
			g_pDataCenter->SetProgressStatus("PrintCard", 1);
			//StepStatus[Step_PrintCard] = true;
		}

		emit UpdateProgress(MP_PrintCard);
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);

		//if (!StepStatus[Step_ReturnData])
		if (!g_pDataCenter->GetProgressStatus("ReturnData"))
		{
			// 数据回盘
			if (QFailed(nResult = ReturnCardData(strMessage, nStatus, pSSCardInfo, false)))
			{
				gError() << gQStr(strMessage);
				break;
			}

#pragma Warning("回盘失败如何处理？")
			if (nStatus != 0 && nStatus != 1)
				break;
			g_pDataCenter->SetProgressStatus("ReturnData", 1);
			StepStatus[Step_ReturnData] = true;
		}
		else
			nResult = 0;

		if (!g_pDataCenter->GetProgressStatus("EnableCard"))
			//if (!StepStatus[Step_EnableCard])
		{
			// 启用
			if (QFailed(nResult = EnalbeCard(strMessage, nStatus, pSSCardInfo)))
			{
				gError() << strMessage.toLocal8Bit().data();
				break;
			}
			StepStatus[Step_EnableCard] = true;
			g_pDataCenter->SetProgressStatus("EnableCard", 1);
			gInfo() << "关闭制卡进度";
			g_pDataCenter->CloseProgress();
			gInfo() << "移除制卡数据文件";
			RemoveCardData(pSSCardInfo);
		}
		else
			nResult = 0;

		if (nStatus != 0 && nStatus != 1)
			break;
#pragma Warning("启用卡片失败如何处理？")
		emit UpdateProgress(MP_EnableCard);
		this_thread::sleep_for(chrono::milliseconds(2000));

		nResult = 0;
	} while (0);

	if (nResult == 0)
	{
		g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);
		emit UpdateProgress(MP_RejectCard);
	}
		
	this_thread::sleep_for(chrono::milliseconds(2000));
	if (QFailed(nResult))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fatal, Stay_CurrentPage);
		emit EnableButtonOK(true);
	}
	else
	{
		emit ShowMaskWidget("提示", "制卡成功,请及时取走卡片", Success, Return_MainPage);
	}
}

void uc_MakeCard::on_pushButton_OK_clicked()
{
	if (m_pWorkThread)
	{
		m_bWorkThreadRunning = false;
		if (m_pWorkThread->joinable())
			m_pWorkThread->join();
		delete m_pWorkThread;
	}
	QString strMessage;
	if (!g_pDataCenter->GetPrinter())
	{
		strMessage = "打印机异常,请检查打印机!";
		gError() << gQStr(strMessage);
		emit ShowMaskWidget("严重错误", strMessage, Fatal, Stay_CurrentPage);
		return;
	}
			
	if (QFailed(g_pDataCenter->UpdatePrinterStatus(strMessage)))
	{
		gError() << gQStr(strMessage);
		emit ShowMaskWidget("严重错误", strMessage, Fatal, Stay_CurrentPage);
		return;
	}
	if (g_pDataCenter->PrinterStatus.fwToner != Ribbon_Full &&
		g_pDataCenter->PrinterStatus.fwToner != Ribbon_LOW)
	{
		strMessage = "色带异常,请更换色带!";
		gError() << gQStr(strMessage);
		emit ShowMaskWidget("严重错误", strMessage, Fatal, Stay_CurrentPage);
		return;
	}

	//if (!g_pDataCenter->GetSSCardReader())
	//{
	//	strMessage = "读卡器异常,请检查读卡器!";
	//	gError() << gQStr(strMessage);
	//	emit ShowMaskWidget("严重错误", strMessage, Fetal, Stay_CurrentPage);
	//	return;
	//}
	
	m_bWorkThreadRunning = true;
	m_pWorkThread = new std::thread(&uc_MakeCard::ThreadWork, this);
	if (!m_pWorkThread)
	{
		QString strError = QString("内存不足,创建制卡线程失败!");
		gError() << strError.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strError, Fatal, Return_MainPage);
	}
	ui->pushButton_OK->setEnabled(false);
	
}

void uc_MakeCard::on_EnableButtonOK(bool bEnable)
{
	for (auto var : m_LableStep)
		var->setStyleSheet(QString::fromUtf8("image: url(:/Image/todo.png);"));
	ui->label_MakeCard->setStyleSheet(QString::fromUtf8("image: url(:/Image/Status_Finish.png);"));
	ui->pushButton_OK->setEnabled(bEnable);
	ui->pushButton_OK->setText("重试");
}
