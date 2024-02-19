#pragma execution_character_set("utf-8")
#include "nc_commitPersonInfo.h"
#include "ui_nc_commitpersoninfo.h"
#include <QCoreApplication>
#include <QObject>
#include "Gloabal.h"
#include "Payment.h"
#include "../utility/json/CJsonObject.hpp"

nc_commitPersonInfo::nc_commitPersonInfo(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::nc_commitPersonInfo)
{
	ui->setupUi(this);
	struct _CareerType
	{
		QString strCode;
		QString strName;
	};
	_CareerType CareetArray[] = {
							{"1000000","单位负责人"},
							{"2000000","专业、技术人员"},
							{"3000000","办事人员"},
							{"4000000","商业、服务人员"},
							{"5000000","农林牧渔水利生产人员"},
							{"6000000","生产运输工人"},
							{"8000000","其他从业人员"} };
	ui->comboBox_Career->clear();
	for (auto& var : CareetArray)
		ui->comboBox_Career->addItem(var.strName, var.strCode);
}

nc_commitPersonInfo::~nc_commitPersonInfo()
{
	delete ui;
}

void nc_commitPersonInfo::OnTimeout()
{

}

int nc_commitPersonInfo::ProcessBussiness()
{
	gInfo() << __FUNCTION__;
	/*if (g_pMaskWindow)
		g_pMaskWindow->hide();*/
	QString strMessage;
	int nResult = -1;
	QString strCardProgress;

	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!pSSCardInfo)
		pSSCardInfo = make_shared<SSCardBaseInfo>();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;

	do
	{
		if (g_pDataCenter->bDebug)
		{
			string strMobile;
			if (QFailed(LoadTestIDData(pIDCard, pSSCardInfo)))
			{
				//pSSCardInfo->strMobile = strMobile.c_str();
				strMessage = "测试用户数据不存在!";
				break;
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

		/*
		"CardID":"33333333333333",
		 "Name":"",
		 "City":"",			// 行政区域
		 "BankCode":""
		*/
		CJsonObject jsonIn;
		jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
		jsonIn.Add("Name", (const char*)pIDCard->szName);
		jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
		jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
		string strJsonIn = jsonIn.ToString();
		string strJsonOut;
		if (QFailed(pService->QueryCardStatus(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText, strErrcode;
			int nErrCode = -1;
			//jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			jsonOut.Get("errcode", strErrcode);
			nErrCode = strtol(strErrcode.c_str(), nullptr, 10);
			QString qstrText = QString::fromLocal8Bit(strText.c_str());
			if ((nErrCode == 3) || (nErrCode == 4 && qstrText.contains("放号")))	// 已经申请过,则继续制卡
			{
				strMessage = QString::fromLocal8Bit(strText.c_str());
				emit ShowMaskWidget("操作成功", strMessage, Success, Switch_NextPage);
			}
			else
			{
				strMessage = QString("查询制卡状态失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}
		}

		pSSCardInfo->strName = (const char*)pIDCard->szName;
		pSSCardInfo->strGender = (const char*)pIDCard->szGender;
		pSSCardInfo->strNation = (const char*)pIDCard->szNationalty;
		pSSCardInfo->strNationCode = (const char*)pIDCard->szNationaltyCode;
		pSSCardInfo->strBirthday = (const char*)pIDCard->szBirthday;
		pSSCardInfo->strIdentity = (const char*)pIDCard->szIdentity;
		pSSCardInfo->strAddress = (const char*)pIDCard->szAddress;
		string strGender = GBK_UTF8((const char*)pIDCard->szGender);
		if (strGender == "男")
			pSSCardInfo->strGenderCode = "1";
		else if (strGender == "女")
			pSSCardInfo->strGenderCode = "2";
		else
			pSSCardInfo->strGenderCode = "9";

		pSSCardInfo->strOrganID = Reginfo.strAgency;
		pSSCardInfo->strBankCode = Reginfo.strBankCode;
		pSSCardInfo->strCity = Reginfo.strCityCode;
		pSSCardInfo->strSSQX = Reginfo.strCountry;
		pSSCardInfo->strCardVender = Reginfo.strCardVendor;
		pSSCardInfo->strBankCode = Reginfo.strBankCode;
		pSSCardInfo->strOccupType = ui->comboBox_Career->currentData().toString().toStdString();

		jsonIn.Clear();
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("City", Reginfo.strCityCode);
		jsonIn.Add("DocType", pSSCardInfo->strCardType);

		strJsonIn = jsonIn.ToString();
		string strJsonout;
		string strCommand = "QueryPersonPhoto";

		if (QFailed(pService->SetExtraInterface(strCommand, strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText;
			int nErrCode = -1;
			jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			strMessage = QString("获取个人照片失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}
		CJsonObject jsonOut(strJsonOut);
		string strPhoto;
		if (jsonOut.Get("Photo", strPhoto))
		{
			SaveSSCardPhoto(strMessage, strPhoto.c_str());
			if (QFailed(SaveSSCardPhotoBase64(strMessage, strPhoto.c_str())))
			{
				strMessage = QString("保存照片数据失败!");
				break;
			}
		}
		else
		{
			strMessage = QString("社保后台未返回个人照片!");
			break;
		}

		ui->label_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName.c_str()));
		ui->label_Gender->setText(QString::fromLocal8Bit(pSSCardInfo->strGender.c_str()));
		ui->label_Nation->setText(QString::fromLocal8Bit(pSSCardInfo->strNation.c_str()));
		ui->label_Birthday->setText(QString::fromLocal8Bit(pSSCardInfo->strBirthday.c_str()));
		ui->label_Identity->setText(QString::fromLocal8Bit(pSSCardInfo->strIdentity.c_str()));
		ui->label_Address->setText(QString::fromLocal8Bit(pSSCardInfo->strAddress.c_str()));
		string strBankName;
		if (QFailed(g_pDataCenter->GetBankName(pSSCardInfo->strBankCode, strBankName)))
		{
			strBankName = pSSCardInfo->strBankCode;
		}
		ui->label_BankName->setText(strBankName.c_str());

		QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
		ui->label_Photo->setStyleSheet(strStyle);
		nResult = 0;
		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	} while (0);

	if (QFailed(nResult))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
	}
	return nResult;
}

void nc_commitPersonInfo::on_pushButton_OK_clicked()
{
	gInfo() << __FUNCTION__;
	QString strInfo;
	if (g_pDataCenter->strSSCardPhotoFile.empty())
	{
		gInfo() << gQStr(strInfo);
		emit ShowMaskWidget("操作失败", "未找到照片数据,需要照片才能进行新办卡业务", Fetal, Return_MainPage);
		return;
	}
	g_pDataCenter->GetSSCardInfo()->strOccupType = ui->comboBox_Career->currentData().toString().toStdString();
	strInfo = QString("信息已确认,稍后请输入手机号码!");
	gInfo() << gQStr(strInfo);
	emit ShowMaskWidget("操作成功", strInfo, Success, Switch_NextPage);
}
