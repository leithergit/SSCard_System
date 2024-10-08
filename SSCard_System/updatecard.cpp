﻿#pragma execution_character_set("utf-8")
#include "updatecard.h"
#include "ui_updatecard.h"
#include "mainwindow.h"
#include "uc_readidcard.h"
#include "uc_facecapture.h"
//#include "uc_facecapture_ocx.h"
#include "uc_ensureinformation.h"
#include "uc_inputmobile.h"
#include "uc_pay.h"
#include "uc_makecard.h"
#include "uc_adforfinance.h"
#include "OperatorSucceed.h"
#include <exception>
#include "Gloabal.h"
#include "idcard_api.h"
using namespace  std;

#define _Stack_Count  (_AdforFinance + 1)

UpdateCard::UpdateCard(QWidget* parent) :
	QMainStackPage(parent),
	ui(new Ui::UpdateCard)
{
	ui->setupUi(this);

	try
	{
		m_pStackWidget = ui->stackedWidget;
		//SysConfigPtr& pSysConfig = g_pDataCenter->GetSysConfigure();
		AddPage(new uc_ReadIDCard(ui->label_step, "updatecard1.png", Page_ReaderIDCard));				// step 0
		//if (g_pDataCenter->GetSysConfigure()->DevConfig.nCameraDrv == CameraDriver::Driver_OCX)
		//	ui->stackedWidget->addWidget(new uc_FaceCapture_ocx(ui->label_step, "updatecard2.png", Page_FaceCapture));		// step 1
		//else
		AddPage(new uc_FaceCapture(ui->label_step, "updatecard2.png", Page_FaceCapture));			// step 1
		AddPage(new uc_EnsureInformation(ui->label_step, "updatecard3.png", Page_EnsureInformation));	// step 2
		AddPage(new uc_InputMobile(ui->label_step, "updatecard4.png", Page_InputMobile));				// step 3
		AddPage(new uc_Pay(ui->label_step, "updatecard5.png", Page_Payment));							// step 4
		AddPage(new uc_MakeCard(ui->label_step, "updatecard6.png", Page_MakeCard));					// step 5
		AddPage(new uc_AdforFinance(nullptr, "", Page_AdforFinance));									// step 6
		AddPage(new OperatorSucceed(nullptr, "", Page_Succeed));
		for (int i = 0; i < m_pStackWidget->count(); i++)
		{
			//connect(ui->stackedWidget->widget(i),SIGNAL(SwitchNextPage),this,SLOT(on_SwitchNextPage));
			QStackPage* pPage = dynamic_cast<QStackPage*>(ui->stackedWidget->widget(i));
			connect(pPage, &QStackPage::SwitchPage, this, &QMainStackPage::on_SwitchPage);
			connect(pPage, &QStackPage::ShowMaskWidget, this, &QMainStackPage::On_ShowMaskWidget);
			//connect(pPage, &QStackPage::RetryCurrentPage, this, &QMainStackPage::On_RetryCurrentPage);
		}
	}
	catch (exception& e)
	{
		qDebug() << "Catch a exception:" << e.what();
	}
}

UpdateCard::~UpdateCard()
{
	delete ui;
}

void  UpdateCard::ShowTimeOut(int nTimeout)
{
	ui->label_CountDown->setText(QString("%1").arg(nTimeout));
}

void UpdateCard::OnTimerEvent()
{
	ui->label_CountDown->setText(QString("%1").arg(--m_nTimeout));
	qDebug() << __FUNCTION__ << "m_nTimeout:" << m_nTimeout;
	if (!m_nTimeout)
	{
		killTimer(m_nTimerID);
		qDebug() << "KillTimer:" << m_nTimerID;
		;		m_nTimerID = 0;
		((QStackPage*)ui->stackedWidget->currentWidget())->OnTimeout();
		((MainWindow*)m_pMainWindow)->on_pushButton_MainPage_clicked();
	}
}
