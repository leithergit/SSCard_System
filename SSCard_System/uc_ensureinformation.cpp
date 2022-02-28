#pragma execution_character_set("utf-8")
#include "uc_ensureinformation.h"
#include "ui_uc_ensureinformation.h"
#include "Gloabal.h"
#include "Payment.h"
//#include "mainwindow.h"
#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;

uc_EnsureInformation::uc_EnsureInformation(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::EnsureInformation)
{
	ui->setupUi(this);
}

uc_EnsureInformation::~uc_EnsureInformation()
{
	ShutDown();
	delete ui;
}

int uc_EnsureInformation::ProcessBussiness()
{
	/*if (g_pMaskWindow)
		g_pMaskWindow->hide();*/
	QString strMessage;
	int nResult = -1;
	QString strCardProgress;

	SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	g_pDataCenter->nCardStratus = CardStatus::Card_Unknow;
	do
	{
		if (g_pDataCenter->bDebug)
		{
			string strMobile;
			if (QSucceed(LoadTestIDData(pIDCard, pSSCardInfo, strMobile)))
			{
				pSSCardInfo->strMobile = strMobile;
			}
		}

		SSCardService* pService = nullptr;

		if (QFailed(g_pDataCenter->OpenSSCardService(&pService, strMessage)))
		{
			break;
		}

		if (!pService)
		{
			strMessage = "社保卡卡管服务不可用!";
			break;
		}
		CJsonObject jsonIn;
		jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
		jsonIn.Add("Name", (const char*)pIDCard->szName);
		jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
		jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
		string strJsonIn = jsonIn.ToString();
		string strJsonOut;
		if (QFailed(pService->QueryCardInfo(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText;
			jsonOut.Get("Result", nResult);
			jsonOut.Get("Message", strText);
			strMessage = QString("查询制卡信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}
		CJsonObject jsonOut(strJsonOut);

		string strSSCardNum, strBankCode, strMobile;
		int nCardStatus;
		if (!jsonOut.Get("CardNum", strSSCardNum) ||
			!jsonOut.Get("CardStatus", nCardStatus) ||
			!jsonOut.Get("BankCode", strBankCode) ||
			!jsonOut.Get("Mobile", strMobile))
		{
			strMessage = "姓名,身份证,社保卡号,社保卡状态,银行代码存在无效字段!";
			gInfo() << strJsonOut;
			break;
		}
		g_pDataCenter->nCardStratus = (CardStatus)nCardStatus;
		//if (g_pDataCenter->nCardServiceType == ServiceType::Service_ReplaceCard)
		//{
		//	if (QFailed(pService->QueryCardStatus(strJsonIn, strJsonOut)))					// 查不到职业类型
		//	{
		//		CJsonObject jsonOut(strJsonOut);
		//		string strText;
		//		jsonOut.Get("Result", nResult);
		//		jsonOut.Get("Message", strText);
		//		strMessage = QString("查询制卡状态失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
		//		break;
		//	}
		//}
		//pService->SetExtraInterface("QueryPersonInfo", strJsonIn, strJsonOut);		// 查不到职业类型
		//pService->SetExtraInterface("QueryPersonPhoto", strJsonIn, strJsonOut);
		//jsonOut.Clear();
		//jsonOut.Parse(strJsonOut);
		//CJsonObject jsonOut(strJsonOut);
		//string strSSCardNum, strBankCode, strMobile;
		//int nCardStatus;
		if (!jsonOut.Get("CardNum", strSSCardNum) ||
			!jsonOut.Get("CardStatus", nCardStatus) ||
			!jsonOut.Get("BankCode", strBankCode) ||
			!jsonOut.Get("Mobile", strMobile))
		{
			strMessage = "姓名,身份证,社保卡号,社保卡状态,银行代码存在无效字段!";
			gInfo() << strJsonOut;
			break;
		}
		g_pDataCenter->nCardStratus = (CardStatus)nCardStatus;

		pSSCardInfo->strCardNum = strSSCardNum;
		pSSCardInfo->strBankCode = strBankCode;
		pSSCardInfo->strOrganID = Reginfo.strAgency;
		pSSCardInfo->strTransType = "5";
		pSSCardInfo->strCity = Reginfo.strCityCode;
		pSSCardInfo->strSSQX = Reginfo.strCountry;
		pSSCardInfo->strCardVender = Reginfo.strCardVendor;

		nResult = 0;
	} while (0);

	pSSCardInfo->strName = (const char*)pIDCard->szName;
	pSSCardInfo->strGender = (const char*)pIDCard->szGender;
	pSSCardInfo->strNation = (const char*)pIDCard->szNationalty;
	pSSCardInfo->strNationCode = (const char*)pIDCard->szNationaltyCode;
	pSSCardInfo->strBirthday = (const char*)pIDCard->szBirthday;
	pSSCardInfo->strIdentity = (const char*)pIDCard->szIdentity;
	pSSCardInfo->strAddress = (const char*)pIDCard->szAddress;

	g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	if (QFailed(nResult))
	{
		gError() << gQStr(strMessage);
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return nResult;
	}

	QString strCardStatus2 = "未知";
	switch (g_pDataCenter->nCardStratus)
	{
	case CardStatus::Card_Release:
		strCardStatus2 = "放号";
		break;
	case CardStatus::Card_Normal:
		strCardStatus2 = "正常";
		m_bRegisterLost = true;
		break;
	case CardStatus::Card_RegisterLost:
		m_bRegisterLost = false;
		strCardStatus2 = "挂失";
		break;
	case CardStatus::Card_RegisgerTempLost:
		strCardStatus2 = "临时挂失";
		break;
	case CardStatus::Card_CanceledOnRegisterLost:
		strCardStatus2 = "挂失后注销";
		break;
	case CardStatus::Card_Canceled:
		strCardStatus2 = "注销";
		break;
	case CardStatus::Card_DeActived:
		strCardStatus2 = "未启用";
		break;
	case CardStatus::Card_Making:
		strCardStatus2 = "制卡中";
		break;
	default:
	case CardStatus::Card_Unknow:
		strCardStatus2 = "未知";
		break;
	}
	ui->label_Hello->setText(QString("您好,%1").arg(QString::fromLocal8Bit((const char*)pIDCard->szName)));
	ui->label_SSCard->setText(QString("社保卡号:%1").arg(pSSCardInfo->strCardNum.c_str()));
	string strBankName;
	if (QFailed(g_pDataCenter->GetBankName(pSSCardInfo->strBankCode, strBankName)))
	{
		strBankName = pSSCardInfo->strBankCode;
	}
	ui->label_Bank->setText(QString("服务银行:%1").arg(strBankName.c_str()));
	ui->label_CardStatus->setText(QString("卡片状态:%1").arg(strCardStatus2));

	QString strCardStatus = pSSCardInfo->strCardStatus.c_str();
	switch (m_nPageIndex)
	{
	case Page_EnsureInformation:
	{
		switch (g_pDataCenter->nCardStratus)
		{
		case CardStatus::Card_Release:
		case CardStatus::Card_Normal:
			ui->pushButton_OK->setEnabled(true);
			break;
		case CardStatus::Card_CanceledOnRegisterLost:
		case CardStatus::Card_Canceled:
		{
			QString strInfo = QString("卡号已注销,请按新办卡流程处理!");
			gError() << gQStr(strMessage);
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return 0;
		}
		break;
		case CardStatus::Card_RegisgerTempLost:
		case CardStatus::Card_RegisterLost:
		{
			ui->pushButton_OK->setEnabled(true);
			strCardStatus2 = "挂失";
			QString strInfo = QString("卡片状态:正式挂失,稍后请输入手机号码!");
			gInfo() << gQStr(strInfo);
			emit ShowMaskWidget("操作成功", strInfo, Success, Switch_NextPage);
		}
		break;
		case CardStatus::Card_DeActived:
		{
			ui->pushButton_OK->setEnabled(false);
			strCardStatus2 = "未启用";
			QString strInfo = QString("卡片状态:未记用,请先启用再输补卡业务!");
			gInfo() << gQStr(strInfo);
			emit ShowMaskWidget("操作成功", strInfo, Fetal, Return_MainPage);
			return 0;
		}
		break;
		case CardStatus::Card_Making:
		{
			ui->pushButton_OK->setEnabled(false);
			QString strInfo = QString("制卡进度:制卡中,直接跳至制卡页面!");
			gInfo() << gQStr(strInfo);
			// 直接跳转到制止页面
			// nNewPage = nCurIndex + nPageOperation -Switch_NextPage + 1;
			// nPageOperation = nNewPage - nCurIndex + Switch_NextPage - 1;
			int nOperation = Page_MakeCard - Page_EnsureInformation + Switch_NextPage - 1;
			emit SwitchNextPage(nOperation);
			strCardStatus2 = "制卡中";
		}

		break;
		default:
		case CardStatus::Card_Unknow:
			strCardStatus2 = "未知";
			ui->pushButton_OK->setEnabled(false);
			break;
		}

		break;
	}
	case Page_RegisterLost:
	{
		switch (g_pDataCenter->nCardStratus)
		{
		case CardStatus::Card_Release:
		case CardStatus::Card_Normal:
		{
			ui->label_Ensure->setText("是否确定挂失并补换卡?");
			ui->pushButton_OK->setText("挂失");
			ui->pushButton_OK->setEnabled(true);
		}
		break;
		case CardStatus::Card_CanceledOnRegisterLost:
		case CardStatus::Card_Canceled:
		{
			ui->pushButton_OK->setEnabled(false);
			QString strInfo = QString("卡号已注销,请按新办卡流程处理!");
			gError() << gQStr(strMessage);
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return 0;
		}
		break;
		case CardStatus::Card_RegisgerTempLost:
		case CardStatus::Card_RegisterLost:
		{
			ui->pushButton_OK->setEnabled(true);
			strCardStatus2 = "挂失";
			ui->label_Ensure->setText("是否确定解除挂失?");
			ui->pushButton_OK->setText("解除挂失");
			//QString strInfo = QString("卡片状态:正式挂失,稍后请输入手机号码!");
			//gInfo() << gQStr(strInfo);
			//emit ShowMaskWidget("操作成功", strInfo, Success, Switch_NextPage);
		}
		break;

		case CardStatus::Card_DeActived:
		{
			ui->pushButton_OK->setEnabled(false);
			strCardStatus2 = "未启用";
			QString strInfo = QString("卡片状态:未记用,请先启用再输补卡业务!");
			gInfo() << gQStr(strInfo);
			emit ShowMaskWidget("操作成功", strInfo, Fetal, Return_MainPage);
			return 0;
		}
		break;
		case CardStatus::Card_Making:
		{
			ui->pushButton_OK->setEnabled(true);
			QString strInfo = QString("制卡进度:制卡中,直接跳至制卡页面!");
			gInfo() << gQStr(strInfo);
			// 直接跳转到制止页面
			// nNewPage = nCurIndex + nPageOperation -Switch_NextPage + 1;
			// nPageOperation = nNewPage - nCurIndex + Switch_NextPage - 1;
			int nOperation = Page_MakeCard - Page_EnsureInformation + Switch_NextPage - 1;
			emit SwitchNextPage(nOperation);
			strCardStatus2 = "制卡中";
		}
		break;
		default:
		case CardStatus::Card_Unknow:
		{
			strCardStatus2 = "未知";
			QString strInfo = QString("卡状态未知,无法进行下一步业务,请联系工作人处理!");
			gError() << gQStr(strMessage);
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		}
		break;
		}
		break;
	}
	default:
		ui->pushButton_OK->setEnabled(false);
		break;
	}

	return nResult;
}

void uc_EnsureInformation::OnTimeout()
{

}


void uc_EnsureInformation::on_pushButton_OK_clicked()
{
	SSCardBaseInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	QString strMessage;
	if (m_nPageIndex == Page_EnsureInformation)
	{
		if (g_pDataCenter->nCardStratus == CardStatus::Card_Release ||
			g_pDataCenter->nCardStratus == CardStatus::Card_Normal)
		{
			QString strError("社保卡信息已经确认,正在发送挂失信息!");
			gInfo() << strError.toLocal8Bit().data();
			//emit ShowMaskWidget("操作成功", strError, Information, Stay_CurrentPage);
			SSCardService* pService = g_pDataCenter->GetSSCardService();
			CJsonObject jsonIn;

			jsonIn.Add("CardID", (const char*)pIDCard->szIdentity);
			jsonIn.Add("Name", (const char*)pIDCard->szName);
			jsonIn.Add("City", Reginfo.strCityCode);
			jsonIn.Add("BankCode", Reginfo.strBankCode);
			jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
			jsonIn.Add("Operator", Reginfo.strOperator);
			string strJsonIn = jsonIn.ToString();
			string strJsonout;
			int nResult = 0;

			if (QFailed(pService->RegisterLost(strJsonIn, strJsonout)))
			{
				CJsonObject jsonOut(strJsonout);
				string strText;
				jsonOut.Get("Result", nResult);
				jsonOut.Get("Message", strText);
				strError = QString("挂失失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				gError() << gQStr(strError);
				emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);
				return;
			}
			if (QFailed(pService->QueryCardStatus(strJsonIn, strJsonout)))
			{
				CJsonObject jsonOut(strJsonout);
				string strText;
				jsonOut.Get("Result", nResult);
				jsonOut.Get("Message", strText);
				strError = "查询卡状态失败!";
				gError() << gQStr(strError);
				emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);
			}
			gInfo() << strError.toLocal8Bit().data();
			emit ShowMaskWidget("操作成功", strError, Success, Switch_NextPage);
			return;
		}
	}
	else
	{

		if (g_pDataCenter->nCardStratus == CardStatus::Card_Release ||
			g_pDataCenter->nCardStratus == CardStatus::Card_Normal)
		{
			QString strError("社保卡信息已经确认,正在发送挂失信息!");
			gInfo() << strError.toLocal8Bit().data();
			SSCardService* pService = g_pDataCenter->GetSSCardService();
			CJsonObject jsonIn;
			jsonIn.Add("CardID", (const char*)pIDCard->szIdentity);
			jsonIn.Add("Name", (const char*)pIDCard->szName);
			jsonIn.Add("City", Reginfo.strCityCode);
			jsonIn.Add("BankCode", Reginfo.strBankCode);
			jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
			jsonIn.Add("Operator", Reginfo.strOperator);
			string strJsonIn = jsonIn.ToString();
			string strJsonout;
			int nResult = 0;

			if (QFailed(pService->RegisterLostTemporary(strJsonIn, strJsonout)))
			{
				CJsonObject jsonOut(strJsonout);
				string strText;
				jsonOut.Get("Result", nResult);
				jsonOut.Get("Message", strText);
				strError = QString("挂失失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				gError() << gQStr(strError);
				emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);
			}
			else
			{
				gInfo() << strError.toLocal8Bit().data();
				emit ShowMaskWidget("操作成功", strError, Success, Return_MainPage);
			}
		}
		else if (g_pDataCenter->nCardStratus == CardStatus::Card_RegisgerTempLost ||
			g_pDataCenter->nCardStratus == CardStatus::Card_RegisterLost)
		{
			QString strError("社保卡信息已经确认,正在发送解除挂失信息!");
			gError() << gQStr(strError);
			SSCardService* pService = g_pDataCenter->GetSSCardService();
			CJsonObject jsonIn;
			jsonIn.Add("CardID", (const char*)pIDCard->szIdentity);
			jsonIn.Add("Name", (const char*)pIDCard->szName);
			jsonIn.Add("City", Reginfo.strCityCode);
			jsonIn.Add("BankCode", Reginfo.strBankCode);
			jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
			jsonIn.Add("Operator", Reginfo.strOperator);
			string strJsonIn = jsonIn.ToString();
			string strJsonout;

			if (QFailed(pService->RegisterLostTemporary(strJsonIn, strJsonout, 1)))
			{
				int nResult = 0;
				CJsonObject jsonOut(strJsonout);
				string strText;
				jsonOut.Get("Result", nResult);
				jsonOut.Get("Message", strText);
				strError = QString("解除挂失失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				gError() << gQStr(strError);
				emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);

			}
			else
			{
				strError = "社保卡解除挂失成功!";
				emit ShowMaskWidget("操作成功", strError, Success, Return_MainPage);
			}
		}
	}
}
