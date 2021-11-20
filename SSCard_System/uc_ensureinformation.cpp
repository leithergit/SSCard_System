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

    if (QFailed(QueryRegisterLost(strMessage)))
    {
        gError()<<strMessage.toLocal8Bit().data();
        emit ShowMaskWidget(strMessage,Fetal,Stay_CurrentPage);
        return -1;
    }

    SSCardInfoPtr &pSSCardInfo = g_pDataCenter->GetSSCardInfo();
    ui->label_Hello->setText(QString("您好，%1").arg(pSSCardInfo->strName.c_str()));
    ui->label_SSCard->setText(QString("社保卡号:%1").arg(pSSCardInfo->strSSCardNumber.c_str()));
    ui->label_Bank->setText(QString("服务银行:%1").arg(pSSCardInfo->strBank.c_str()));
    ui->label_CardStatus->setText(QString("卡片状态:%1").arg(pSSCardInfo->strStatus.c_str()));

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
#ifdef _DEBUG
    g_pDataCenter->FillSSCardWithIDCard();
    g_pDataCenter->GetSSCardInfo()->strSSCardNumber = "H507FE8AX";
    g_pDataCenter->GetSSCardInfo()->strBank = u8"招商银行";
    g_pDataCenter->GetSSCardInfo()->strStatus = u8"正常";
#endif
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
