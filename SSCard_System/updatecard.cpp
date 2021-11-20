#pragma execution_character_set("utf-8")
#include "updatecard.h"
#include "ui_updatecard.h"
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

UpdateCard::UpdateCard(QWidget* parent) :
	QMainStackPage(parent),
	ui(new Ui::UpdateCard)
{
	ui->setupUi(this);

	try
	{
		m_pStackWidget = ui->stackedWidget;
		// ui->stackedWidget->addWidget(new FailedWindow(nullptr));

		m_pInputMobile = new uc_InputMobile(ui->label_InputMobile);
		ui->stackedWidget->addWidget(new uc_ReadIDCard(ui->label_ReadCard));
		ui->stackedWidget->addWidget(new uc_FaceCapture(ui->label_FaceCompare));
		ui->stackedWidget->addWidget(new uc_EnsureInformation(ui->label_EnsureInformation));
		//ui->stackedWidget->addWidget(m_pInputMobile);
		ui->stackedWidget->addWidget(new uc_InputMobile(ui->label_InputMobile));
		ui->stackedWidget->addWidget(new uc_Pay(ui->label_Pay));
		ui->stackedWidget->addWidget(new uc_MakeCard(ui->label_MakeCard));
		ui->stackedWidget->addWidget(new uc_AdforFinance(nullptr));
		ui->stackedWidget->addWidget(new OperatorSucceed(nullptr));
		for (int i = 0; i < ui->stackedWidget->count(); i++)
		{
			//connect(ui->stackedWidget->widget(i),SIGNAL(SwitchNextPage),this,SLOT(on_SwitchNextPage));
			QStackPage* pPage = dynamic_cast<QStackPage*>(ui->stackedWidget->widget(i));
			//connect(pPage, &QStackPage::SwitchNextPage, this, &QMainStackPage::on_SwitchNextPage);
			connect(pPage, &QStackPage::ShowMaskWidget, this, &QMainStackPage::On_ShowMaskWidget);
		}
		connect(this, &QMainStackPage::SwitchNextPage, this, &QMainStackPage::on_SwitchNextPage);
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

void UpdateCard::OnTimerEvent()
{
	ui->label_CountDown->setText(QString("%1").arg(--m_nTimeout));
	if (!m_nTimeout)
	{
		killTimer(m_nTimerID);
		m_nTimerID = 0;
		((QStackPage*)ui->stackedWidget->currentWidget())->OnTimeout();
		((MainWindow*)m_pMainWindow)->on_pushButton_MainPage_clicked();
	}
}
