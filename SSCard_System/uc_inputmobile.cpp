#pragma execution_character_set("utf-8")
#include "uc_inputmobile.h"
#include "ui_uc_inputmobile.h"
#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;
#include "Gloabal.h"
//#include "mainwindow.h"

uc_InputMobile::uc_InputMobile(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::InputMobile)
{
	ui->setupUi(this);
}

uc_InputMobile::~uc_InputMobile()
{
	ShutDown();
	delete ui;
}

int uc_InputMobile::ProcessBussiness()
{
	ui->lineEdit_Mobile->setText("");
	//ui->lineEdit_Mobile->selectAll();
	m_nMobilePhoneSize = g_pDataCenter->GetSysConfigure()->nMobilePhoneSize;
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	strcpy((char*)pSSCardInfo->strMobile, (const char*)g_pDataCenter->strMobilePhone.c_str());
	m_strMobile = "";
	return 0;
}

void uc_InputMobile::OnTimeout()
{

}

void uc_InputMobile::on_pushButton_0_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "0";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_1_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "1";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_2_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "2";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_3_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "3";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_4_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "4";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_5_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "5";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_6_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "6";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_7_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "7";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_8_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "8";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_9_clicked()
{
	if (m_strMobile.size() < MaxMobileSize)
	{
		m_strMobile += "9";
		ui->lineEdit_Mobile->setText(m_strMobile);
	}
}

void uc_InputMobile::on_pushButton_Clear_clicked()
{
	m_strMobile = "";
	ui->lineEdit_Mobile->setText(m_strMobile);
}


void uc_InputMobile::on_pushButton_Backspace_clicked()
{
	m_strMobile = m_strMobile.left(m_strMobile.size() - 1);
	ui->lineEdit_Mobile->setText(m_strMobile);
}

// 查询支付结果
int uc_InputMobile::QueryPayResult(QString& strMessage, int& nResult)
{
	nResult = 0;

	return 0;
}

void uc_InputMobile::on_pushButton_OK_clicked()
{
	if (m_strMobile.size() == m_nMobilePhoneSize)
	{
		g_pDataCenter->strMobilePhone = m_strMobile.toStdString();
		SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
		strcpy((char*)pSSCardInfo->strMobile, (const char*)g_pDataCenter->strMobilePhone.c_str());
		QString strMessage;
		PayResult nPayResult = PayResult::Pay_Unsupport;
		string strPayMessage;
		SSCardService* pService = g_pDataCenter->GetSSCardService();
		CJsonObject jsonIn;
		jsonIn.Add("CardID", (const char*)g_pDataCenter->GetIDCardInfo()->szIdentity);
		string strJsonIn = jsonIn.ToString();
		string strJsonOut;

		if (QFailed(pService->QueryPayResult(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			jsonOut.Get("Result", (int&)nPayResult);

			jsonOut.Get("Message", strPayMessage);
			gError() << strPayMessage;
			strMessage = QString::fromLocal8Bit(strPayMessage.c_str());
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return;
		}
		CJsonObject jsonOut(strJsonOut);
		jsonOut.Get("Result", (int&)nPayResult);
		jsonOut.Get("Message", strPayMessage);

		if (nPayResult == PayResult::Pay_Unsupport)
		{
			QString strInfo = "手机号码已确认,稍后开始制卡!";
			gInfo() << gQStr(strInfo);
			emit ShowMaskWidget("操作成功", strInfo, Success, Switch_NextPage);
		}
		else if (nPayResult == PayResult::PaySucceed)
		{
			QString strInfo = "手机号码已确认,并且补卡费用已支付,稍后开始制卡!";
			gInfo() << strInfo.toLocal8Bit().data();
			emit ShowMaskWidget("操作成功", strInfo, Success, Skip_NextPage);
		}
		else
		{
			QString strInfo = "手机号码已确认,稍后将进入费用支付流程!";
			gInfo() << strInfo.toLocal8Bit().data();
			emit ShowMaskWidget("操作成功", strInfo, Success, Switch_NextPage);
		}
	}
	else
	{
		QString strInfo = QString("手机号码不足%1位,请输入正确的手机号码!").arg(m_nMobilePhoneSize);
		gInfo() << strInfo.toLocal8Bit().data();
		emit ShowMaskWidget("输入错误", strInfo, Error, Stay_CurrentPage);
	}
}
