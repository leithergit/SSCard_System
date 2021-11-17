#pragma execution_character_set("utf-8")
#include "uc_ensureinformation.h"
#include "ui_uc_ensureinformation.h"
#include "Gloabal.h"
//#include "mainwindow.h"

uc_EnsureInformation::uc_EnsureInformation(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
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
    if (QFailed(ReadSSCardInfo(strMessage)))
    {
        gError()<<strMessage.toLocal8Bit().data();
        emit ShowMaskWidget(strMessage,Fetal,Stay_CurrentPage);
        return -1;
    }

    if (QSucceed(QueryRegisterLost(strMessage)))
    {
        gError()<<strMessage.toLocal8Bit().data();
        emit ShowMaskWidget(strMessage,Fetal,Stay_CurrentPage);
        return -1;
    }



	return 0;
}

void uc_EnsureInformation::OnTimeout()
{

}

int uc_EnsureInformation::QueryRegisterLost(QString &strMessage)
{
    return 0;
}

int uc_EnsureInformation::ReadSSCardInfo(QString &strError)
{

    return 0;
}

int  uc_EnsureInformation::RegisterLost(QString &strError)
{
    if (m_bTestStatus)
        return 0;
    else
    {
        strError = "社保卡挂失失败，请确认信息后重试!";
        m_bTestStatus = true;
        return -1;
    }
}

void uc_EnsureInformation::on_pushButton_OK_clicked()
{
    QString strError("社保卡信息已经确认,正在发送挂失信息!");
    gInfo()<<strError.toLocal8Bit().data();
    emit ShowMaskWidget(strError,Information,Stay_CurrentPage);
    if (QFailed(RegisterLost(strError)))
    {
        gError()<<strError.toLocal8Bit().data();
        emit ShowMaskWidget(strError,Failed,Stay_CurrentPage);
        return ;
    }
    else
    {
        strError = "社保卡挂失成功,稍后请输入常用手机号码!";
        gInfo()<<strError.toLocal8Bit().data();
        emit ShowMaskWidget(strError,Success,Switch_NextPage);
    }
}
