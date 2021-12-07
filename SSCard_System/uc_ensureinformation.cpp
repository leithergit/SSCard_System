#pragma execution_character_set("utf-8")
#include "uc_ensureinformation.h"
#include "ui_uc_ensureinformation.h"
#include "Gloabal.h"
#include "Payment.h"
//#include "mainwindow.h"

uc_EnsureInformation::uc_EnsureInformation(QLabel* pTitle, QString strStepImage, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nTimeout, parent),
	ui(new Ui::EnsureInformation)
{
	ui->setupUi(this);
}

uc_EnsureInformation::~uc_EnsureInformation()
{
	delete ui;
}

int uc_EnsureInformation::ProcessBussiness()
{
	QString strMessage;
	int nStatus = 0;
	int nResult = -1;
	QString strCardProgress;
	do
	{
		IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
		SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
#ifdef _DEBUG
#pragma Warning("测试阶段使用测试人员身份信息")
		QString strName, strID, strMobile;
		if (QSucceed(LoadTestData(strName, strID, strMobile)))
		{

			gInfo() << "Succeed in load Test Card Data:" << gQStr(strName) << gQStr(strID) << gQStr(strMobile);
			strcpy((char*)pIDCard->szName, strName.toLocal8Bit().data());
			strcpy((char*)pSSCardInfo->strName, strName.toLocal8Bit().data());
			strcpy((char*)pIDCard->szIdentify, strID.toStdString().c_str());
			strcpy((char*)pSSCardInfo->strCardID, strID.toStdString().c_str());
			strcpy((char*)pSSCardInfo->strMobile, strMobile.toStdString().c_str());
		}
#endif
		SSCardInfoPtr pTempSSCardInfo = make_shared<SSCardInfo>();//QString strName = "韩娜娜";

		//strcpy((char*)pIDCard->szIdentify, "412726198006120043");
		//strcpy((char*)pSSCardInfo->strMobile, "15895349880");
		//strcpy((char*)pSSCardInfo->strCardID, "412726198006120043");

		if (QFailed(QueryCardProgress(strMessage, nStatus, pIDCard, pTempSSCardInfo)))
		{
			break;
		}

		g_pDataCenter->strCardMakeProgress = QString::fromLocal8Bit(pTempSSCardInfo->strCardStatus).toStdString();
		if (g_pDataCenter->strCardMakeProgress == "制卡中")
		{
			// 可以获取新的社保卡一些数据
			if (QFailed(nResult = (ReadSSCardInfo(strMessage, nStatus))))
				break;
			if (nStatus != 0 && nStatus != 1)
				break;
			nResult = 0;
			break;
		}

		if (QFailed(ReadSSCardInfo(strMessage, nStatus)))
			break;

		if (nStatus != 0 && nStatus != 1)
			break;

		if (QFailed(QuerySSCardStatus(strMessage)))
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
	}
	if (g_pDataCenter->strCardMakeProgress == "制卡中")
	{
		QString strInfo = QString("制卡进度:制卡中,直接跳至制卡页面!");
		gInfo() << gQStr(strInfo);
		// 直接跳转到制止页面
		// nNewPage = nCurIndex + nPageOperation -Switch_NextPage + 1;
		// nPageOperation = nNewPage - nCurIndex + Switch_NextPage - 1;
		int nOperation = Page_MakeCard - Page_EnsureInformation + Switch_NextPage - 1;
		emit SwitchNextPage(nOperation);
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

int	 uc_EnsureInformation::QuerySSCardStatus(QString& strMessage)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	int nResult = 0;
	char* szStatus[128] = { 0 };
	// int queryCardStatus(const char* user, const char* pwd, const char* cardID, const char* cardNum, const char* name, const char* city, char* pOutInfo);
	if (QFailed(nResult = queryCardStatus(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		reinterpret_cast<const char*>(pIDCard->szIdentify),
		pSSCardInfo->strCardNum,
		reinterpret_cast<const char*>(pIDCard->szName),
		Region.strRegionCode.c_str(),
		reinterpret_cast<char*>(szStatus))))
	{
		QString strInfo = QString("queryCardStatus失败:%1,CardID = %2\tName = %3\t").arg(nResult).arg(reinterpret_cast<char*>(pIDCard->szIdentify)).arg(reinterpret_cast<char*>(pIDCard->szName));
		gError() << gQStr(strInfo);
		strMessage = QString("查询卡状态失败!");
		return -1;
	}

	if (strcmp(_strupr((char*)szStatus), "OK") == 0)
	{
		strcpy(pSSCardInfo->strCardStatus, "正常");
	}
	else
	{
		QString strStatus = QString::fromLocal8Bit((char*)szStatus);
		strcpy(pSSCardInfo->strCardStatus, (char*)strStatus.toStdString().c_str());
	}

	return 0;
}

int uc_EnsureInformation::ReadSSCardInfo(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;

	int nResult = 0;
	char szStatus[1024] = { 0 };
	if (QFailed(nResult = queryPersonInfo(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		reinterpret_cast<const char*>(pIDCard->szIdentify),
		reinterpret_cast<const char*>(pIDCard->szName),
		Region.strRegionCode.c_str(),
		*pSSCardInfo,
		szStatus)))
	{
		QString strInfo = QString("queryPersonInfo失败:%1,CardID = %2\tName = %3\t").arg(nResult).arg((char*)pIDCard->szIdentify).arg((char*)pIDCard->szName);
		gError() << gQStr(strInfo);
		strMessage = QString("查询社保卡信息失败!");
		return -1;
	}
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
	}
	else
	{
		strMessage = szStatus;
		return -1;
	}
	//g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	return 0;
}

int  uc_EnsureInformation::RegisterLost(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = 0;
	if (QFailed(nResult = reportLostCard(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		(const char*)pIDCard->szIdentify,
		(const char*)pSSCardInfo->strCardNum,
		(const char*)pIDCard->szName,
		Region.strRegionCode.c_str(),
		szStatus)))
	{
		QString strInfo = QString("reportLostCard失败:%1,CardID = %2\tName = %3\t").arg(nResult).arg((char*)pIDCard->szIdentify).arg((char*)pIDCard->szName);
		gError() << gQStr(strInfo);
		strMessage = QString("挂失失败!");
		return -1;
	}

	gInfo() << "reportLostCard:" << szStatus;
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		if (nStatus == 2)
		{
			strMessage = (char*)&szStatus[2];
		}
	}
	else
	{
		strMessage = szStatus;
		return -1;
	}

	return 0;
}

void uc_EnsureInformation::on_pushButton_OK_clicked()
{
	QString strError("社保卡信息已经确认,正在发送挂失信息!");
	gInfo() << strError.toLocal8Bit().data();
	//emit ShowMaskWidget("操作成功", strError, Information, Stay_CurrentPage);
	int nStatus = 0;
	if (QFailed(RegisterLost(strError, nStatus)))
	{
		gError() << strError.toLocal8Bit().data();
		emit ShowMaskWidget("操作失败", strError, Failed, Stay_CurrentPage);
		return;
	}
	else
	{
		strError = "社保卡挂失成功,稍后请输入常用手机号码!";
		gInfo() << strError.toLocal8Bit().data();
		emit ShowMaskWidget("操作成功", strError, Success, Switch_NextPage);
	}
}
