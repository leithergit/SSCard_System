#pragma execution_character_set("utf-8")
#include "uc_pay.h"
#include "ui_uc_pay.h"
#include "Gloabal.h"
//#include "mainwindow.h"

uc_Pay::uc_Pay(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::Pay)
{
	ui->setupUi(this);
}

uc_Pay::~uc_Pay()
{
    ShutDownDevice();
	delete ui;
}

int uc_Pay::ProcessBussiness()
{
    m_bWorkThreadRunning = true;
    m_pWorkThread = new std::thread(&uc_Pay::ThreadWork, this);
    if (!m_pWorkThread)
    {
        QString strError = QString("内存不足,创建密码键盘线程失败!");
        gError()<<strError.toLocal8Bit().data();
        emit ShowMaskWidget(strError,Fetal,Return_MainPage);
        return -1;
    }
	return 0;
}

void uc_Pay::OnTimeout()
{
    ShutDownDevice();
}

void uc_Pay::ShutDownDevice()
{

}

void uc_Pay::ThreadWork()
{

}
