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
}

uc_MakeCard::~uc_MakeCard()
{
	ShutDown();
	delete ui;
}

int uc_MakeCard::ProcessBussiness()
{
	/*if (g_pMaskWindow)
		g_pMaskWindow->hide();*/
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
	m_pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!m_pWorkThread)
	{
		m_bWorkThreadRunning = true;
		m_pWorkThread = new std::thread(&uc_MakeCard::ThreadWork, this);
		if (!m_pWorkThread)
		{
			QString strError = QString("内存不足,创建制卡线程失败!");
			gError() << strError.toLocal8Bit().data();
			emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
			return -1;
		}
	}
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

void uc_MakeCard::ThreadWork()
{
	char szRCode[32] = { 0 };
	int nResult = -1;
	QString strMessage;

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

	char szBuffer[1024] = { 0 };
	int nBufferSize = sizeof(szBuffer);
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	int nStatus = 0;
	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;

	do
	{

		if (QFailed(pService->CommitPersonInfo(strJsonIn, strJsonOut)))
			break;

		if (QFailed(pService->PreMakeCard(strJsonIn, strJsonOut)))

			break;

		if (QFailed(g_pDataCenter->Depense(strMessage)))
			break;
		strInfo = "进卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(g_pDataCenter->WriteCard(pSSCardInfo, strMessage)))
			break;
		strInfo = "写卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
			break;
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(pService->ReturnData(strJsonIn, strJsonOut)))
			break;
		if (QFailed(pService->ActiveCard(strJsonIn, strJsonOut)))
			break;
		nResult = 0;
	} while (0);


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
