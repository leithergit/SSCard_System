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
	if (g_pDataCenter->bDebug)
	{
		string strJsonFile = "./Debug/Carddata_" + pSSCardInfo->strIdentity + ".json";
		if (QFailed(LoadJsonCardData(pSSCardInfo, strJsonFile)))
			return;
	}

	ui->lineEdit_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName.c_str()));
	ui->lineEdit_CardID->setText(pSSCardInfo->strIdentity.c_str());
	ui->lineEditl_SSCard->setText(pSSCardInfo->strCardNum.c_str());
	QDateTime dt = QDateTime::currentDateTime();
	QString strIssueDate = dt.toString("yyyy年MM月");
	//int nYear = 0, nMonth = 0, nDay;
	//sscanf_s(pSSCardInfo->strReleaseDate.c_str(), "%04d%02d%02d", &nYear, &nMonth, &nDay);
	//char szReleaseDate[32] = { 0 };
	//sprintf_s(szReleaseDate, 32, "%d年%d月", nYear, nMonth);
	ui->lineEdit_Datetime->setText(strIssueDate);

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
	//m_pSSCardInfo = g_pDataCenter->GetSSCardInfo();

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
	//g_pDataCenter->CloseDevice();
}

//int  uc_MakeCard::PremakeCard(QString& strMessage)
//{
//	int nResult = -1;
//	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
//	IDCardInfoPtr& pIDCardInfo = g_pDataCenter->GetIDCardInfo();
//	SSCardService* pService = g_pDataCenter->GetSSCardService();
//	string strJsonIn, strJsonOut;
//
//	do
//	{
//		CJsonObject jsonIn;
//		jsonIn.Add("Name", pSSCardInfo->strName);
//		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
//		jsonIn.Add("City", pSSCardInfo->strCity);
//		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
//		strJsonIn = jsonIn.ToString();
//		gInfo() << "JsonIn = " << strJsonIn;
//		if (QFailed(pService->PreMakeCard(strJsonIn, strJsonOut)))
//		{
//			CJsonObject jsonOut(strJsonOut);
//			string strErrText;
//			jsonOut.Get("Message", strErrText);
//			strMessage = QString("预制卡开户失败:%1").arg(QString::fromLocal8Bit(strErrText.c_str()));
//			break;
//		}
//		gInfo() << "JsonOut = " << strJsonOut;
//		CJsonObject jsonOut(strJsonOut);
//		if (!jsonOut.KeyExist("CardNum") ||
//			!jsonOut.KeyExist("Photo"))
//		{
//			strMessage = "未能获取社保卡号码和照片数据!";
//			break;
//		}
//		string strPhoto;
//		string strSSCardNum;
//		jsonOut.Get("CardNum", pSSCardInfo->strCardNum);
//		jsonIn.Add("Nation", pSSCardInfo->strNationCode);
//		//jsonOut.Get("NationalityCode", pSSCardInfo->strNationCode);
//		jsonOut.Get("Photo", pSSCardInfo->strPhoto);
//		SaveSSCardPhoto(strMessage, pSSCardInfo->strPhoto.c_str());
//		nResult = 0;
//	} while (0);
//	return nResult;
//}
//
//int uc_MakeCard::LoadPhoto(SSCardService* pService, string& strPhoto, QString& strMessage)
//{
//	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
//	IDCardInfoPtr& pIDCardInfo = g_pDataCenter->GetIDCardInfo();
//	int nResult = -1;
//	if (g_pDataCenter->strSSCardPhotoFile.empty())
//	{
//		CJsonObject jsonQuery;
//		jsonQuery.Clear();
//
//		jsonQuery.Add("CardID", pSSCardInfo->strIdentity);
//		jsonQuery.Add("Name", pSSCardInfo->strName);
//		jsonQuery.Add("City", pSSCardInfo->strCity);
//
//		string strJsonQuery = jsonQuery.ToString();
//		string strJsonOut;
//		string strCommand = "QueryPersonPhoto";
//
//		if (QFailed(pService->SetExtraInterface(strCommand, strJsonQuery, strJsonOut)))
//		{
//			CJsonObject jsonOut(strJsonOut);
//			string strText;
//			int nErrCode = -1;
//			jsonOut.Get("Result", nErrCode);
//			jsonOut.Get("Message", strText);
//			strMessage = QString("获取个人照片失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
//			return -1;
//		}
//		CJsonObject jsonOut(strJsonOut);
//		string strPhoto;
//		if (jsonOut.Get("Photo", strPhoto))
//		{
//			SaveSSCardPhoto(strMessage, strPhoto.c_str());
//			if (QFailed(SaveSSCardPhotoBase64(strMessage, strPhoto.c_str())))
//			{
//				strMessage = QString("保存照片数据失败!");
//				return -1;
//			}
//		}
//		else
//		{
//			strMessage = QString("社保后台未返回个人照片!");
//			return 1;
//		}
//		/*strMessage = "提示", "未找到照片数据,请先下载照片";
//		break;*/
//	}
//
//	try
//	{
//		ifstream ifs(g_pDataCenter->strSSCardPhotoBase64File, ios::in | ios::binary);
//		stringstream ss;
//		ss << ifs.rdbuf();
//		// 读取jpg图片并转base64
//		//QByteArray ba(ss.str().c_str(), ss.str().size());
//		//QByteArray baBase64 = ba.toBase64();
//		//jsonIn.Add("Photo", baBase64.data());
//		// 直接使用base64数据
//		strPhoto = ss.str();
//		//jsonIn.Add("Photo", );
//		return 0;
//	}
//	catch (std::exception& e)
//	{
//		gInfo() << e.what();
//		return -1;
//	}
//}
//
//int  uc_MakeCard::CommitPersionInfo(QString& strMessage)
//{
//	int nResult = -1;
//
//	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
//	IDCardInfoPtr& pIDCardInfo = g_pDataCenter->GetIDCardInfo();
//	RegionInfo RegInfo = g_pDataCenter->GetSysConfigure()->Region;
//	pSSCardInfo->strBankCode = RegInfo.strBankCode;
//	SSCardService* pService = g_pDataCenter->GetSSCardService();
//	string strJsonIn, strJsonOut;
//	do
//	{
//		CJsonObject jsonIn;
//		jsonIn.Add("IssueDate", (char*)pIDCardInfo->szExpirationDate1);
//		QString strExpireDate = QString::fromLocal8Bit((const char*)pIDCardInfo->szExpirationDate2);
//		if (strExpireDate == "长期")
//			jsonIn.Add("ExpireDate", "99991231");
//		else
//			jsonIn.Add("ExpireDate", (char*)pIDCardInfo->szExpirationDate2);
//
//		jsonIn.Add("Name", pSSCardInfo->strName);
//		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
//		jsonIn.Add("Gender", pSSCardInfo->strGender);
//		jsonIn.Add("City", pSSCardInfo->strCity);
//
//		jsonIn.Add("Mobile", pSSCardInfo->strMobile);
//		jsonIn.Add("Birthday", pSSCardInfo->strBirthday);
//		jsonIn.Add("Nation", pSSCardInfo->strNationCode);
//		//jsonIn.Add("Nation", (char*)pIDCardInfo->szNationaltyCode);
//		jsonIn.Add("Address", pSSCardInfo->strAddress);
//		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
//		if QFailed(LoadPhoto(pService, pSSCardInfo->strPhoto, strMessage))
//		{
//			break;
//		}
//		gInfo() << "JsonIn(without photo) = " << strJsonIn;
//		jsonIn.Add("Photo", pSSCardInfo->strPhoto);
//
//		switch (g_pDataCenter->nCardServiceType)
//		{
//		case ServiceType::Service_NewCard:
//		{
//
//		}
//		break;
//		case ServiceType::Service_ReplaceCard:
//		{
//			jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
//		}
//		break;
//		case ServiceType::Service_RegisterLost:
//		default:
//		{
//			strMessage = "不支持的制卡服务!";
//			gInfo() << gQStr(strMessage);
//			break;
//		}
//		}
//
//		strJsonIn = jsonIn.ToString();
//
//		if (QFailed(pService->CommitPersonInfo(strJsonIn, strJsonOut)))
//		{
//			gInfo() << "JsonOut = " << strJsonOut;
//			CJsonObject jsonOut(strJsonOut);
//			string strErrText;
//			string strErrcode;
//			jsonOut.Get("Message", strErrText);
//			jsonOut.Get("errcode", strErrcode);
//			QString qstrErrText = QString::fromLocal8Bit(strErrText.c_str());
//			if (qstrErrText.contains("申卡人存在有效的制卡申请"))
//			{
//				nResult = 0;
//				break;
//			}
//			strMessage = QString("提交用户信息失败:%1").arg(qstrErrText);
//			nResult = -1;
//			break;
//		}
//		gInfo() << "JsonOut = " << strJsonOut;
//		nResult = 0;
//	} while (0);
//	return nResult;
//}
//
//int	 uc_MakeCard::WriteCard(QString& strMessage)
//{
//	int nResult = -1;
//	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
//	SSCardService* pService = g_pDataCenter->GetSSCardService();
//	string strJsonIn, strJsonOut;
//	QString strInfo;
//	int nWriteCardCount = 0;
//
//	while (nWriteCardCount < 3)
//	{
//		strInfo = QString("尝试第%1次写卡!").arg(nWriteCardCount + 1);
//		gInfo() << gQStr(strInfo);
//		nResult = g_pDataCenter->WriteCard(pSSCardInfo, strMessage);
//
//		if (nResult == -4)
//		{
//			nWriteCardCount++;
//			strMessage = "写卡上电失败!";
//			gInfo() << gQStr(strMessage);
//			g_pDataCenter->MoveCard(strMessage);
//			continue;
//		}
//		else if (QFailed(nResult))
//		{
//			strMessage = "写卡失败!";
//			break;
//		}
//		nResult = 0;
//	}
//	return nResult;
//}
//
//int  uc_MakeCard::EnsureData(QString& strMessage)
//{
//	int nResult = -1;
//	char szRCode[32] = { 0 };
//	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
//	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
//
//	QString strInfo;
//	SSCardService* pService = g_pDataCenter->GetSSCardService();
//	string strJsonIn, strJsonOut;
//	char szCardATR[1024] = { 0 };
//	char szCardBaseRead[1024] = { 0 };
//	int nCardATRLen = 0;
//
//	do
//	{
//		CJsonObject jsonIn;
//		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
//		jsonIn.Add("Name", pSSCardInfo->strName);
//		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
//		jsonIn.Add("City", pSSCardInfo->strCity);
//		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
//		jsonIn.Add("CardATR", pSSCardInfo->strCardATR);
//		jsonIn.Add("CardIdentity", pSSCardInfo->strCardIdentity);
//		jsonIn.Add("ChipNum", pSSCardInfo->strBankNum);
//		jsonIn.Add("MagNum", pSSCardInfo->strBankNum);
//		jsonIn.Add("CardVersion", g_pDataCenter->strCardVersion);
//		jsonIn.Add("ChipType", "32");
//
//		strJsonIn = jsonIn.ToString();
//		gInfo() << "JsonIn = " << strJsonIn;
//		if (QFailed(pService->ReturnData(strJsonIn, strJsonOut)))
//		{
//			gInfo() << "JsonOut = " << strJsonOut;
//			CJsonObject jsonOut(strJsonOut);
//			string strErrText;
//			jsonOut.Get("Message", strErrText);
//			strMessage = strErrText.c_str();
//			break;
//		}
//		gInfo() << "JsonOut = " << strJsonOut;
//		nResult = 0;
//	} while (0);
//	return nResult;
//}
//
//int  uc_MakeCard::ActiveCard(QString& strMessage)
//{
//	char szRCode[32] = { 0 };
//	int nResult = -1;
//
//	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
//	QString strInfo;
//	SSCardService* pService = g_pDataCenter->GetSSCardService();
//	string strJsonIn, strJsonOut;
//
//	do
//	{
//		CJsonObject jsonIn;
//
//		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
//		jsonIn.Add("Name", pSSCardInfo->strName);
//		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
//		jsonIn.Add("City", pSSCardInfo->strCity);
//		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
//		jsonIn.Add("MagNum", pSSCardInfo->strBankNum);
//		strJsonIn = jsonIn.ToString();
//		gInfo() << "JsonIn = " << strJsonIn;
//		if (QFailed(pService->ActiveCard(strJsonIn, strJsonOut)))
//		{
//			gInfo() << "JsonOut = " << strJsonOut;
//			CJsonObject jsonOut(strJsonOut);
//			string strErrText;
//			jsonOut.Get("Message", strErrText);
//			strMessage = strErrText.c_str();
//			break;
//		}
//		gInfo() << "JsonOut = " << strJsonOut;
//		nResult = 0;
//	} while (0);
//	return 0;
//}

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
		if (QFailed(g_pDataCenter->SafeWriteCard(strMessage)))
			break;
		emit UpdateProgress(MP_WriteCard);

		if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
			break;
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		emit UpdateProgress(MP_PrintCard);

		if (QFailed(g_pDataCenter->EnsureData(strMessage)))
			break;

		if (QFailed(g_pDataCenter->ActiveCard(strMessage)))
			break;
		emit UpdateProgress(MP_EnableCard);
		g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);

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
	emit UpdateProgress(MP_RejectCard);
	this_thread::sleep_for(chrono::milliseconds(2000));
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
