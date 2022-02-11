#pragma execution_character_set("utf-8")
#include "newcard.h"
#include "ui_Newcard.h"
#include "mainwindow.h"
#include "uc_readidcard.h"
#include "uc_facecapture.h"
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

NewCard::NewCard(QWidget* parent) :
	QMainStackPage(parent),
	ui(new Ui::NewCard)
{
	ui->setupUi(this);
	try
	{
		m_pStackWidget = ui->stackedWidget;
		ui->stackedWidget->addWidget(new uc_ReadIDCard(ui->label_step, "updatecard1.png", Page_ReaderIDCard));				// step 0
		ui->stackedWidget->addWidget(new uc_FaceCapture(ui->label_step, "updatecard2.png", Page_FaceCapture));				// step 1
		ui->stackedWidget->addWidget(new uc_EnsureInformation(ui->label_step, "updatecard3.png", Page_EnsureInformation));	// step 2
		ui->stackedWidget->addWidget(new uc_InputMobile(ui->label_step, "updatecard4.png", Page_InputMobile));				// step 3
		ui->stackedWidget->addWidget(new uc_Pay(ui->label_step, "updatecard5.png", Page_Payment));							// step 4
		ui->stackedWidget->addWidget(new uc_MakeCard(ui->label_step, "updatecard6.png", Page_MakeCard));					// step 5
		ui->stackedWidget->addWidget(new uc_AdforFinance(nullptr, "", Page_AdforFinance));									// step 6
		ui->stackedWidget->addWidget(new OperatorSucceed(nullptr, "", Page_Succeed));
		for (int i = 0; i < m_pStackWidget->count(); i++)
		{
			//connect(ui->stackedWidget->widget(i),SIGNAL(SwitchNextPage),this,SLOT(on_SwitchNextPage));
			QStackPage* pPage = dynamic_cast<QStackPage*>(ui->stackedWidget->widget(i));
			connect(pPage, &QStackPage::SwitchNextPage, this, &QMainStackPage::on_SwitchNextPage);
			connect(pPage, &QStackPage::ShowMaskWidget, this, &QMainStackPage::On_ShowMaskWidget);
			//connect(pPage, &QStackPage::RetryCurrentPage, this, &QMainStackPage::On_RetryCurrentPage);
		}
		//connect(this, &QMainStackPage::SwitchNextPage, this, &QMainStackPage::on_SwitchNextPage);
		//connect(this, &QMainStackPage::SwitchPage, this, &QMainStackPage::on_SwitchPage);
	}
	catch (exception& e)
	{
		qDebug() << "Catch a exception:" << e.what();
	}
}

NewCard::~NewCard()
{
	delete ui;
}

void  NewCard::SetTimeOut(int nTimeout)
{
	ui->label_CountDown->setText(QString("%1").arg(nTimeout));
}

void NewCard::OnTimerEvent()
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
