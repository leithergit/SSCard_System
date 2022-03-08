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

int uc_MakeCard::ProcessBussiness()
{
	for (auto var : m_LableStep)
		var->setStyleSheet(QString::fromUtf8("image: url(:/Image/todo.png);"));
	m_nSocketRetryInterval = g_pDataCenter->GetSysConfigure()->PaymentConfig.nQueryPayResultInterval;            // 支付结构查询时间间隔单 毫秒
	m_nSocketRetryCount = g_pDataCenter->GetSysConfigure()->PaymentConfig.nSocketRetryCount;
	int nResult = -1;
	QString strMessage;

	if (!g_pDataCenter->GetPrinter())
	{
		if (QFailed(g_pDataCenter->OpenPrinter(strMessage)))
		{
			gError() << gQStr(strMessage);
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return nResult;
		}
	}
	if (!g_pDataCenter->GetSSCardReader())
	{
		if (QFailed(g_pDataCenter->OpenSSCardReader(strMessage)))
		{
			gError() << gQStr(strMessage);
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
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
	strcpy((char*)pSSCardInfo->strCard, Reginfo.strCardVendor.c_str());
	strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());

	if (g_pDataCenter->strCardMakeProgress == "制卡中")
	{
		if (QFailed(PrecessCardInMaking(strMessage)))
		{
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return -1;
		}
	}
	else
	{
		if (QFailed(PrepareMakeCard(strMessage)))
		{
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return -1;
		}
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
	g_pDataCenter->CloseDevice();
}


int uc_MakeCard::PrecessCardInMaking(QString& strMessage)
{
	gInfo() << __FUNCTION__;
	int nStatus = 0;
	int nResult = 0;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	QString strCardProgress = QString::fromLocal8Bit(pSSCardInfo->strCardStatus);

	do
	{
		if (QFailed(nResult = MarkCard(strMessage, nStatus, pSSCardInfo)))
		{
			if (strMessage == "已经开始制卡")
			{
				if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
				{
					if (strMessage.contains("已经有批次号"))
					{
						QString strText = strMessage;
						strText += "\n请转到后台以写卡失败选项手动制卡!";
						strMessage = strText;
						break;
					}
					if (strMessage.contains("批次"))
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

		if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
		{
			break;
			/*if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus)))
				break;*/
		}

		if (nStatus != 0 && nStatus != 1)
		{
			nResult = -1;
			break;
		}
	} while (0);
	return nResult;
}


int uc_MakeCard::PrepareMakeCard(QString& strMessage)
{
	int nStatus = 0;
	int nResult = -1;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	int nPayStatus = Pay_Not;
	do
	{
		nResult = ApplyCardReplacement(strMessage, nStatus, pSSCardInfo);     //  申请补换卡
		if (QFailed(nResult))
			break;
		if (nStatus != 0 && nStatus != 1)
		{
			nResult = -1;
			break;
		}

		nResult = ResgisterPayment(strMessage, nStatus, pSSCardInfo);          // 缴费登记
		if (QFailed(nResult))
			break;

		if (nStatus != 0 && nStatus != 1)
		{
			strMessage = QString("缴费登记失败:%1!").arg(nStatus);
			nResult = -1;
			break;
		}

		nResult = MarkCard(strMessage, nStatus, pSSCardInfo);
		if (QFailed(nResult))
		{
			strMessage = QString("即制卡标注失败,Result:%1!").arg(nResult);
			break;
		}
		if (nStatus != 0 && nStatus != 1)
		{
			strMessage = QString("即制卡标注失败:%1!").arg(nStatus);
			nResult = -1;
			break;
		}

		if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
		{
			if (strMessage.contains("批次"))
			{
				strMessage += ",需社保局后台更新数据,请在2小时后再尝试制卡!";
				break;
			}

			if (QFailed(nResult = CancelMarkCard(strMessage, nStatus, pSSCardInfo)))
			{
				strMessage = "因获取制卡数据失败,尝试取消即制卡标注时再次失败!";
				nResult = -1;
				break;
			}
			if (nStatus != 0 && nStatus != 1)
			{
				strMessage = QString("因获取制卡数据失败,尝试取消即制卡标失败:%1!").arg(strMessage);
				nResult = -1;
				break;
			}
			break;
		}
		if (nStatus != 0 && nStatus != 1)
		{
			nResult = -1;
			break;
		}
	} while (0);
	return nResult;

}
void uc_MakeCard::ThreadWork()
{
	char szRCode[32] = { 0 };
	int nResult = -1;
	QString strMessage;

	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szBuffer[1024] = { 0 };
	int nBufferSize = sizeof(szBuffer);
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	int nStatus = 0;
	QString strInfo;

	do
	{
		/*if (g_pDataCenter->strCardMakeProgress == "制卡中")
		{
			if (QFailed(PrecessCardInMaking(strMessage)))
				break;
		}
		else
		{
			if (QFailed(PrepareMakeCard(strMessage)))
			{
				break;
			}
		}
		emit UpdateProgress(MP_PreMakeCard);*/
		//this_thread::sleep_for(chrono::milliseconds(2000));
		//if (QFailed(g_pDataCenter->Depense(strMessage)))
		//	break;
		//emit UpdateProgress(Depense);
		//this_thread::sleep_for(chrono::milliseconds(2000));
		//strInfo = "进卡成功";
		//gInfo() << gQStr(strInfo);
		int nWriteCardCount = 0;
		nResult = -1;
		while (nWriteCardCount < 3)
		{
			strInfo = QString("尝试第%1次写卡!").arg(nWriteCardCount + 1);
			gInfo() << gQStr(strInfo);
			nResult = g_pDataCenter->WriteCard(pSSCardInfo, strMessage);
			if (nResult == 0)
				break;
			if (nResult == -4)
			{
				nWriteCardCount++;
				strMessage = "写卡上电失败!";
				gInfo() << gQStr(strMessage);
				g_pDataCenter->MoveCard(strMessage);
				continue;
			}
			else if (QFailed(nResult))
			{
				strMessage = "写卡失败!";
				break;
			}
		}

		if (QFailed(nResult))
		{
			strInfo = "写卡失败";
			gInfo() << gQStr(strInfo);
			break;
		}
		emit UpdateProgress(MP_WriteCard);
		///this_thread::sleep_for(chrono::milliseconds(2000));
		if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
		{
			nResult = -1;
			strMessage = "卡面打印失败,稍后请管理人员到【手动制卡】界面,选择【打印卡面】以继续完成卡片制作!";
			return;
		}

		emit UpdateProgress(MP_PrintCard);
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		nResult = 0;
	} while (0);

	if QFailed(nResult)
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}
	else if (QFailed(nResult))
	{
		gInfo() << gQStr(strMessage);
		do
		{
			gInfo() << "Try to CancelMarkCard";
			if (QFailed(nResult = CancelMarkCard(strMessage, nStatus, pSSCardInfo)))
			{
				nResult = -1;
				strMessage = QString("取消标注失败:%1").arg(strMessage);
				gInfo() << gQStr(strMessage);
				break;
			}
			//			目前在河南无权取消
			// 			if (QFailed(CancelCardReplacement(strMessage, nStatus)))
			// 			{
			// 				gInfo() << gQStr(strMessage);
			// 				break;
			// 			}
			//#pragma Warning("需要处理取消补换卡的状态")
		} while (0);

		gError() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}
	gInfo() << gQStr(QString("写卡，打印成功"));
	nResult = -1;
	do
	{
		// 数据回盘
		if (QFailed(nResult = ReturnCardData(strMessage, nStatus, pSSCardInfo, false)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}

#pragma Warning("回盘失败如何处理？")
		if (nStatus != 0 && nStatus != 1)
			break;

		// 启用
		if (QFailed(nResult = EnalbeCard(strMessage, nStatus, pSSCardInfo)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
		if (nStatus != 0 && nStatus != 1)
			break;
#pragma Warning("启用卡片失败如何处理？")
		emit UpdateProgress(MP_EnableCard);
		this_thread::sleep_for(chrono::milliseconds(2000));

		nResult = 0;
	} while (0);

	g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);

	emit UpdateProgress(MP_RejectCard);
	this_thread::sleep_for(chrono::milliseconds(2000));
	if (QFailed(nResult))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}
	else
	{
		emit ShowMaskWidget("提示", "制卡成功,请及时取走卡片", Success, Return_MainPage);
	}
}

void uc_MakeCard::on_pushButton_OK_clicked()
{
	if (!m_pWorkThread)
	{
		m_bWorkThreadRunning = true;
		m_pWorkThread = new std::thread(&uc_MakeCard::ThreadWork, this);
		if (!m_pWorkThread)
		{
			QString strError = QString("内存不足,创建制卡线程失败!");
			gError() << strError.toLocal8Bit().data();
			emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
		}
		ui->pushButton_OK->setEnabled(false);
	}
}
