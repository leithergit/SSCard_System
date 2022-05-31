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
	int nStatus = 0;
	int nResult = -1;
	QString strCardProgress;

	SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	do
	{
		if (g_pDataCenter->bDebug)
		{
			string strCardID, strName, strMobile;
			if (QSucceed(LoadTestData(strName, strCardID, strMobile)))
			{
				gInfo() << "Name:" << pIDCard->szName << "\tID:" << pIDCard->szIdentity;
				strcpy((char*)pIDCard->szName, strName.c_str());
				strcpy((char*)pIDCard->szIdentity, strCardID.c_str());
				gInfo() << "Name:" << pIDCard->szName << "\tID:" << pIDCard->szIdentity;
			}
		}
		strcpy((char*)pSSCardInfo->strName, (const char*)pIDCard->szName);
		strcpy((char*)pSSCardInfo->strCardID, (const char*)pIDCard->szIdentity);
		strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
		strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
		strcpy((char*)pSSCardInfo->strTransType, "5");
		strcpy((char*)pSSCardInfo->strCity, Reginfo.strCityCode.c_str());
		strcpy((char*)pSSCardInfo->strSSQX, Reginfo.strCountry.c_str());
		strcpy((char*)pSSCardInfo->strCard, Reginfo.strCardVendor.c_str());
		strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());

		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
		SSCardInfoPtr pTempSSCardInfo = make_shared<SSCardInfo>();
		strcpy((char*)pTempSSCardInfo->strName, (const char*)pIDCard->szName);
		strcpy((char*)pTempSSCardInfo->strCardID, (const char*)pIDCard->szIdentity);

		if (QFailed(QueryCardProgress(strMessage, nStatus, pTempSSCardInfo)))
		{
			break;
		}

		g_pDataCenter->strCardMakeProgress = QString::fromLocal8Bit(pTempSSCardInfo->strCardStatus).toStdString();
		if (g_pDataCenter->nCardServiceType == ServiceType::Service_ReplaceCard &&
			g_pDataCenter->strCardMakeProgress == "制卡中")
		{
			// 可以获取新的社保卡一些数据
			if (QFailed(nResult = g_pDataCenter->ReadSSCardInfo(pSSCardInfo, nStatus, strMessage)))
				break;
			if (nStatus != 0 && nStatus != 1)
				break;
			nResult = 0;
			break;
		}

		// 可取旧社保卡号
		if (QFailed(nResult = g_pDataCenter->ReadSSCardInfo(pSSCardInfo, nStatus, strMessage)))
			break;

		if (nStatus != 0 && nStatus != 1)
			break;

		if (QFailed(nResult = g_pDataCenter->QuerySSCardStatus(pSSCardInfo, strMessage)))
			break;

		if (nStatus != 0)
		{
			break;
		}

		strcpy((char*)pSSCardInfo->strBankCode, pTempSSCardInfo->strBankCode);
		ui->label_Hello->setText(QString("您好，%1").arg(QString::fromLocal8Bit(pSSCardInfo->strName)));
		ui->label_SSCard->setText(QString("社保卡号:%1").arg(pSSCardInfo->strCardNum));
		string strBankName;
		if (QFailed(g_pDataCenter->GetBankName(pSSCardInfo->strBankCode, strBankName)))
		{
			strMessage = QString("未知的银行代码:%1").arg(pSSCardInfo->strBankCode);
			break;
		}

		ui->label_Bank->setText(QString("服务银行:%1").arg(QString(strBankName.c_str())));
		string strCardStatus;
		ui->label_CardStatus->setText(QString("卡片状态:%1").arg(pSSCardInfo->strCardStatus));
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		gError() << gQStr(strMessage);
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return nResult;
	}
	QString strCardStatus = pSSCardInfo->strCardStatus;
	switch (m_nPageIndex)
	{
	case Page_EnsureInformation:
	{
		if (strCardStatus == "正式挂失")
		{
			QString strInfo = QString("卡片状态:正式挂失,稍后请输入手机号码!");
			gInfo() << gQStr(strInfo);
			emit ShowMaskWidget("操作成功", strInfo, Success, Switch_NextPage);
		}
		if (g_pDataCenter->strCardMakeProgress == "制卡中")
		{
			QString strInfo = QString("制卡进度:制卡中,直接跳至制卡页面!");
			gInfo() << gQStr(strInfo);
			// 直接跳转到制止页面						
			//emit SwitchNextPage(nOperation);
			emit ShowMaskWidget("操作成功", "制卡信息已确认,现将转入制卡页面!", Success, Goto_Page, Page_MakeCard);
		}
		break;
	}
	case Page_RegisterLost:
	{
		//ui->pushButton_OK->setEnabled(true);
		if (strCardStatus == "正式挂失")
		{
			ui->label_Ensure->setText("是否确定解除挂失?");
			ui->pushButton_OK->setText("解除挂失");
			m_bRegisterLost = false;
		}
		else if (strCardStatus == "正常")
		{
			ui->label_Ensure->setText("是否确定挂失并补换卡?");
			ui->pushButton_OK->setText("挂失");
			m_bRegisterLost = true;
		}
		else
			ui->pushButton_OK->setEnabled(false);
	}
	default:
		break;
	}

	return nResult;
}

void uc_EnsureInformation::OnTimeout()
{

}

// int uc_EnsureInformation::QueryRegisterLost(QString& strMessage)
// {
// 	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
// 	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
// 	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
// 	QueryRegisterLost()
// 	return 0;
// }

//int	 uc_EnsureInformation::QuerySSCardStatus(QString& strMessage, SSCardInfoPtr& pSSCardInfo)
//{
//	int nResult = 0;
//	char* szStatus[1024] = { 0 };
//	// 这里需要旧卡号
//	if (QFailed(nResult = queryCardStatus(*pSSCardInfo, reinterpret_cast<char*>(szStatus))))
//	{
//		FailureMessage("查询卡状态", pSSCardInfo, szStatus, strMessage);
//		/*if (strcmp((const char*)szStatus, "08") == 0)
//		{
//			strMessage = QString("查询卡状态失败:人社服务器没有响应,可能网络异常或人社服务器故障\n姓名:%1\t卡号:%2\t").arg(pSSCardInfo->strName).arg(pSSCardInfo->strCardNum);
//		}
//		else
//			strMessage = QString("查询卡状态失败:%1\n姓名:%2\t卡号:%3\t").arg(nResult).arg(pSSCardInfo->strName).arg(pSSCardInfo->strCardNum);*/
//		gError() << gQStr(strMessage);
//		return -1;
//	}
//
//	if (strcmp(_strupr((char*)szStatus), "OK") == 0)
//	{
//		strcpy(pSSCardInfo->strCardStatus, "正常");
//	}
//	else
//	{
//
//		QString strStatus = QString::fromLocal8Bit((char*)szStatus);
//		strcpy(pSSCardInfo->strCardStatus, (char*)strStatus.toStdString().c_str());
//	}
//
//	return 0;
//}

//int uc_EnsureInformation::ReadSSCardInfo(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
//{
//	int nResult = 0;
//	char szStatus[1024] = { 0 };
//	if (QFailed(nResult = queryPersonInfo(*pSSCardInfo, szStatus)))
//	{
//		FailureMessage("查询人员信息", pSSCardInfo, szStatus, strMessage);
//		/*if (strcmp(szStatus, "08") == 0)
//			strMessage = QString("失败:人社服务器没有响应,可能网络异常或人社服务器故障\n姓名:%1\1卡号:%2\t").arg(pSSCardInfo->strName).arg(pSSCardInfo->strCardNum);
//		else
//			strMessage = QString("查询人员信息失败:%1\n姓名:%2\t卡号:%3\t").arg(nResult).arg((char*)pSSCardInfo->strName).arg((char*)pSSCardInfo->strCardNum);*/
//
//		gError() << gQStr(strMessage);
//		return -1;
//	}
//	char szDigit[16] = { 0 }, szText[1024] = { 0 };
//	SplitString(szStatus, szDigit, szText);
//	if (strlen(szText))
//		strMessage = QString::fromLocal8Bit(szText);
//	if (strlen(szDigit))
//	{
//		nStatus = strtolong(szDigit, 10);
//		return 0;
//	}
//	else
//		return -1;
//}

int  uc_EnsureInformation::RegisterLost(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	char szStatus[1024] = { 0 };
	int nResult = 0;
	if (QFailed(nResult = reportLostCard(*pSSCardInfo, szStatus)))
	{
		FailureMessage("挂失失败", pSSCardInfo, szStatus, strMessage);
		nStatus = strtol(szStatus, nullptr, 0);
		/*if (strcmp(szStatus, "08") == 0)
			strMessage = QString("挂失失败:人社服务器没有响应,可能网络异常或人社服务器故障\n姓名:%1\1卡号:%2\t").arg(pSSCardInfo->strName).arg(pSSCardInfo->strCardNum);
		else
			strMessage = QString("挂失失败:失败:%1\n姓名:%2\t卡号:%3\t").arg(nResult).arg((char*)pSSCardInfo->strName).arg((char*)pSSCardInfo->strCardNum);*/
		gError() << gQStr(strMessage);
		return -1;
	}

	gInfo() << "reportLostCard:" << szStatus;

	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

int uc_EnsureInformation::UnRegisterLost(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	char szStatus[1024] = { 0 };
	int nResult = 0;
	if (QFailed(nResult = cancelLostCard(*pSSCardInfo, szStatus)))
	{
		FailureMessage("解除挂失", pSSCardInfo, szStatus, strMessage);
		gError() << gQStr(strMessage);
		//QString strInfo = QString("cancelLostCard失败:%1,Name = %2\tCardID = %3\t").arg(nResult).arg((char*)pSSCardInfo->strName).arg((char*)pSSCardInfo->strCardID);
		//gError() << gQStr(strInfo);

		return -1;
	}

	gInfo() << "cancelLostCard:" << szStatus;
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

void uc_EnsureInformation::on_pushButton_OK_clicked()
{
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (m_nPageIndex == Page_EnsureInformation)
	{
		QString strError("社保卡信息已经确认,正在发送挂失信息!");
		gInfo() << strError.toLocal8Bit().data();
		//emit ShowMaskWidget("操作成功", strError, Information, Stay_CurrentPage);
		int nStatus = 0;
		if (QFailed(RegisterLost(strError, nStatus, pSSCardInfo)))
		{
			gError() << gQStr(strError);
			// 即使失败，也作二次确认，防止后台系统抽风，成功也返回错误
			bool bSucceed = false;
			QString strMessage;
			do
			{
				if (nStatus == 8)	// 网络错误，直接返回
					break;

				if (QFailed(g_pDataCenter->QuerySSCardStatus(pSSCardInfo, strMessage)))
				{
					strError += "二次确认卡状态时再次失败!";
					gError() << gQStr(strError);
					break;
				}
				QString strCardStatus = pSSCardInfo->strCardStatus;
				if (strCardStatus == "正式挂失")
				{
					strError = "社保卡挂失成功,稍后请输入常用手机号码!";
					gInfo() << gQStr(strError);
					bSucceed = true;
					emit ShowMaskWidget("操作成功", strError, Success, Switch_NextPage);
				}

			} while (0);
			if (!bSucceed)
				emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);
			return;
		}
		else
		{
			if (nStatus == 200)
			{
				strError = "社保卡挂失成功,稍后请输入常用手机号码!";
				gInfo() << gQStr(strError);
				emit ShowMaskWidget("操作成功", strError, Success, Switch_NextPage);
			}
			else
			{
				QString strMessage;
				if (QFailed(g_pDataCenter->QuerySSCardStatus(pSSCardInfo, strMessage)))
				{
					strMessage = "挂失时社保中心返回错误代码:【";
					strMessage += std::to_string(nStatus).c_str();
					strMessage += "】二次确认卡状态时再次失败!";
					gError() << gQStr(strMessage);
					emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);
					return;
				}
				QString strCardStatus = pSSCardInfo->strCardStatus;
				if (strCardStatus == "正式挂失")
				{
					strError = "社保卡挂失成功,稍后请输入常用手机号码!";
					gInfo() << gQStr(strError);
					emit ShowMaskWidget("操作成功", strError, Success, Switch_NextPage);
				}
				else
				{
					strMessage = "挂失时社保中心返回错误【";
					strMessage + strError;
					strMessage + "】";
					gError() << gQStr(strMessage);
					emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);
					return;
				}
			}
		}
	}
	else
	{
		if (m_bRegisterLost)
		{
			QString strError("社保卡信息已经确认,正在发送挂失信息!");
			gInfo() << strError.toLocal8Bit().data();
			//emit ShowMaskWidget("操作成功", strError, Information, Stay_CurrentPage);
			int nStatus = 0;
			if (QFailed(RegisterLost(strError, nStatus, pSSCardInfo)))
			{
				gError() << gQStr(strError);
				emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);
			}
			else
			{
				strError = "社保卡挂失成功!";
				emit ShowMaskWidget("操作成功", strError, Success, Return_MainPage);
			}
		}
		else
		{
			QString strError("社保卡信息已经确认,正在发送解挂信息!");
			gInfo() << strError.toLocal8Bit().data();
			//emit ShowMaskWidget("操作成功", strError, Information, Stay_CurrentPage);
			int nStatus = 0;
			if (QFailed(UnRegisterLost(strError, nStatus, pSSCardInfo)))
			{
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
