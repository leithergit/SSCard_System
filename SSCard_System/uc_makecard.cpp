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

int  uc_MakeCard::PremakeCard(QString& strMessage)
{
	int nResult = -1;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;

	do
	{
		CJsonObject jsonIn;
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("CardID", pSSCardInfo->strCardID);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
		strJsonIn = jsonIn.ToString();
		gInfo() << "JsonIn = " << strJsonIn;
		if (QFailed(pService->PreMakeCard(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			jsonOut.Get("Message", strErrText);
			strMessage = strErrText.c_str();
			break;
		}
		gInfo() << "JsonOut = " << strJsonOut;
		CJsonObject jsonOut(strJsonOut);
		if (!jsonOut.KeyExist("CardNum") ||
			!jsonOut.KeyExist("Photo"))
		{
			strMessage = "未能获取社保卡号码和照片数据!";
			break;
		}
		string strPhoto;
		string strSSCardNum;
		jsonOut.Get("CardNum", strSSCardNum);
		jsonOut.Get("Photo", strPhoto);
		SaveSSCardPhoto(strMessage, strPhoto.c_str());
		nResult = 0;
	} while (0);
	return nResult;
}

int  uc_MakeCard::CommitPersionInfo(QString& strMessage)
{
	int nResult = -1;

	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	IDCardInfoPtr& pIDCardInfo = g_pDataCenter->GetIDCardInfo();
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;
	do
	{
		CJsonObject jsonIn;
		jsonIn.Add("Name", (char*)pSSCardInfo->strName);
		jsonIn.Add("CardID", (char*)pSSCardInfo->strCardID);
		jsonIn.Add("City", (char*)pSSCardInfo->strCity);
		jsonIn.Add("Mobile", (char*)pSSCardInfo->strMobile);
		jsonIn.Add("IssueDate", (char*)pIDCardInfo->szExpirationDate1);
		jsonIn.Add("ExpireDate", (char*)pIDCardInfo->szExpirationDate1);
		jsonIn.Add("Birthday", (char*)pIDCardInfo->szBirthday);
		jsonIn.Add("Gender", (char*)pIDCardInfo->szGender);
		jsonIn.Add("Nation", (char*)pIDCardInfo->szNation);
		jsonIn.Add("Address", (char*)pIDCardInfo->szAddress);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		strJsonIn = jsonIn.ToString();
		gInfo() << "JsonIn = " << strJsonIn;
		if (QFailed(pService->CommitPersonInfo(strJsonIn, strJsonOut)))
		{
			gInfo() << "JsonOut = " << strJsonOut;
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			jsonOut.Get("Message", strErrText);
			strMessage = strErrText.c_str();
			break;
		}
		gInfo() << "JsonOut = " << strJsonOut;
		nResult = 0;
	} while (0);
	return nResult;
}

int	 uc_MakeCard::WriteCard(QString& strMessage)
{
	int nResult = -1;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;
	QString strInfo;
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
		nResult = 0;
	}
	return nResult;
}

int  uc_MakeCard::EnsureData(QString& strMessage)
{
	int nResult = -1;
	char szRCode[32] = { 0 };
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;

	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;
	char szCardATR[1024] = { 0 };
	char szCardBaseRead[1024] = { 0 };
	int nCardATRLen = 0;

	do
	{
		CJsonObject jsonIn;
		jsonIn.Add("CardID", pSSCardInfo->strCardID);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		jsonIn.Add("CardATR", pSSCardInfo->strCardATR);
		jsonIn.Add("CardIdentity", pSSCardInfo->strIdentifyNum);
		jsonIn.Add("ChipNum", pSSCardInfo->strBankNum);
		jsonIn.Add("MagNum", pSSCardInfo->strBankNum);
		jsonIn.Add("CardVersion", g_pDataCenter->strCardVersion);
		jsonIn.Add("ChipType", "32");

		strJsonIn = jsonIn.ToString();
		gInfo() << "JsonIn = " << strJsonIn;
		if (QFailed(pService->ReturnData(strJsonIn, strJsonOut)))
		{
			gInfo() << "JsonOut = " << strJsonOut;
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			jsonOut.Get("Message", strErrText);
			strMessage = strErrText.c_str();
			break;
		}
		gInfo() << "JsonOut = " << strJsonOut;
		nResult = 0;
	} while (0);
	return nResult;
}

int  uc_MakeCard::ActiveCard(QString& strMessage)
{
	char szRCode[32] = { 0 };
	int nResult = -1;

	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;

	do
	{
		CJsonObject jsonIn;

		jsonIn.Add("CardID", pSSCardInfo->strCardID);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		jsonIn.Add("MagNum", pSSCardInfo->strBankNum);
		strJsonIn = jsonIn.ToString();
		gInfo() << "JsonIn = " << strJsonIn;
		if (QFailed(pService->ActiveCard(strJsonIn, strJsonOut)))
		{
			gInfo() << "JsonOut = " << strJsonOut;
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			jsonOut.Get("Message", strErrText);
			strMessage = strErrText.c_str();
			break;
		}
		gInfo() << "JsonOut = " << strJsonOut;
		nResult = 0;
	} while (0);
	return 0;
}

void uc_MakeCard::ThreadWork()
{
	char szRCode[32] = { 0 };
	int nResult = -1;
	QString strMessage;

	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	QString strInfo;
	do
	{
		if (QFailed(this->CommitPersionInfo(strMessage)))
			break;

		if (QFailed(this->PremakeCard(strMessage)))
			break;

		if (QFailed(g_pDataCenter->Depense(strMessage)))
			break;

		if (QFailed(this->WriteCard(strMessage)))
			break;

		if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
			break;
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(this->EnsureData(strMessage)))
			break;

		if (QFailed(this->ActiveCard(strMessage)))
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
