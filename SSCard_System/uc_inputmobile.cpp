#pragma execution_character_set("utf-8")
#include "uc_inputmobile.h"
#include "ui_uc_inputmobile.h"
#include "Gloabal.h"
//#include "mainwindow.h"

uc_InputMobile::uc_InputMobile(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::InputMobile)
{
	ui->setupUi(this);
}

uc_InputMobile::~uc_InputMobile()
{
	delete ui;
}

int uc_InputMobile::ProcessBussiness()
{
    m_nMobilePhoneSize = g_pDataCenter->GetSysConfigure()->nMobilePhoneSize;
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

void uc_InputMobile::on_pushButton_OK_clicked()
{
    if (m_strMobile.size() == m_nMobilePhoneSize)
    {
        g_pDataCenter->strMobilePhone = m_strMobile.toStdString();
        QString strInfo = "手机号码已确认，稍后请支付补卡费用!";
        gInfo()<<strInfo.toLocal8Bit().data();
        emit ShowMaskWidget(strInfo,Success,Switch_NextPage);
    }
    else
    {
        QString strInfo = QString("手机号码不足%1位,请输入正确的手机号码!").arg(m_nMobilePhoneSize);
        gInfo()<<strInfo.toLocal8Bit().data();
        emit ShowMaskWidget(strInfo,Error,Stay_CurrentPage);
    }
}
