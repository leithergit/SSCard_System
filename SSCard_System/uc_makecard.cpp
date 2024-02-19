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
	m_LableStep.push_back(ui->label_Read);
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
	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();

	ui->lineEdit_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName.c_str()));
	ui->lineEdit_CardID->setText(pSSCardInfo->strIdentity.c_str());
	ui->lineEditl_SSCard->setText(pSSCardInfo->strCardNum.c_str());
	QDateTime dt = QDateTime::currentDateTime();
	QString strIssueDate = dt.toString("yyyy年MM月");

	ui->lineEdit_Datetime->setText(strIssueDate);

	string strBankName;
	if (QFailed(g_pDataCenter->GetBankName(pSSCardInfo->strBankCode, strBankName)))
		strBankName = pSSCardInfo->strBankCode;

	ui->lineEdit_Bank->setText(strBankName.c_str());

	QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
	ui->label_Photo->setStyleSheet(strStyle);
}

int uc_MakeCard::ProcessBussiness()
{
	/*if (g_pMaskWindow)
		g_pMaskWindow->hide();*/
	for (auto var : m_LableStep)
		var->setStyleSheet(QString::fromUtf8("image: url(:/Image/todo.png);"));

	m_nSocketRetryInterval = g_pDataCenter->GetSysConfigure()->PaymentConfig.nQueryPayResultInterval;            // 支付结构查询时间间隔单 毫秒
	m_nSocketRetryCount = g_pDataCenter->GetSysConfigure()->PaymentConfig.nSocketRetryCount;
	int nResult = -1;
	QString strMessage;
	ui->pushButton_OK->setText("确定");
#ifdef _DESKTOP
	//int nResult = -1;
	if (QFailed(nResult = g_pDataCenter->OpenDevice(strMessage)))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	if (QFailed(nResult = g_pDataCenter->TestPrinter(strMessage)))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	if (QFailed(nResult = g_pDataCenter->TestCard(strMessage)))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

#endif

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

	if (QFailed(g_pDataCenter->CommitPersionInfo(strMessage)))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}

	if (QFailed(g_pDataCenter->PremakeCard(strMessage)))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}
	OnUpdateProgress(MP_PreMakeCard);
	ShowSSCardInfo();
	ui->pushButton_OK->setEnabled(true);

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
		delete m_pWorkThread;
		m_pWorkThread = nullptr;
	}
}


void uc_MakeCard::ThreadWork()
{
	int nResult = -1;
	QString strMessage;
	char szRCode[128] = { 0 };
	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	QString strInfo;
	do
	{
		if (QFailed(g_pDataCenter->Depense(strMessage)))
			break;

		if (QFailed(g_pDataCenter->SafeReadCard(strMessage)))
			break;
		emit UpdateProgress(MP_ReadCard);
		if (!g_pDataCenter->bSkipWriteCard)
		{
			if (QFailed(g_pDataCenter->SafeWriteCard(strMessage)))
				break;
		}

		emit UpdateProgress(MP_WriteCard);

		if (!g_pDataCenter->bSkipPrintCard)
		{
			if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
				break;
		}

		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		emit UpdateProgress(MP_PrintCard);

		if (QFailed(g_pDataCenter->EnsureData(strMessage)))
			break;

		if (QFailed(g_pDataCenter->ActiveCard(strMessage)))
			break;
		emit UpdateProgress(MP_EnableCard);

		g_pDataCenter->RemoveTempPersonInfo();
		g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);

		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Stay_CurrentPage);
		emit EnableButtonOK(true);
		return;
	}
	else
	{
		emit ShowMaskWidget("提示", "制卡成功,请及时取走卡片", Success, Return_MainPage);
	}
	emit UpdateProgress(MP_RejectCard);
	this_thread::sleep_for(chrono::milliseconds(2000));
}

void uc_MakeCard::on_pushButton_OK_clicked()
{
	if (m_pWorkThread)
	{
		if (m_pWorkThread->joinable())
			m_pWorkThread->join();
		delete m_pWorkThread;
		m_pWorkThread = nullptr;
	}

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

void uc_MakeCard::on_EnableButtonOK(bool bEnable)
{
	for (auto var : m_LableStep)
		var->setStyleSheet(QString::fromUtf8("image: url(:/Image/todo.png);"));
	ui->label_MakeCard->setStyleSheet(QString::fromUtf8("image: url(:/Image/Status_Finish.png);"));
	ui->pushButton_OK->setEnabled(bEnable);
	ui->pushButton_OK->setText("重试");
}
