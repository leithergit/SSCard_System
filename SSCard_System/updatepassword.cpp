#pragma execution_character_set("utf-8")
#include "updatepassword.h"
#include "ui_updatepassword.h"
#include "mainwindow.h"
#include "up_changepwd.h"
#include "up_Inputpwd.h"
#include "up_readsscard.h"
#include "OperatorSucceed.h"
#include "MaskWidget.h"
#include <array>
#include <QListWidget>
using namespace  std;
#define _Stack_Count  (_ChangePWD + 1)

UpdatePassword::UpdatePassword(QWidget* parent) :
	QMainStackPage(parent),
	ui(new Ui::UpdatePassword)
{
	ui->setupUi(this);

	try
	{

		m_pStackWidget = ui->stackedWidget;
		//ui->stackedWidget->addWidget(new FailedWindow(nullptr));
		//SysConfigPtr& pSysConfig = g_pDataCenter->GetSysConfigure();
		ui->stackedWidget->addWidget(new up_ReadSSCard(ui->label_step, "changepassword1.png", Page_ReadSSCard));
		ui->stackedWidget->addWidget(new up_InputPWD(ui->label_step, "changepassword2.png", Page_InputSSCardPWD));
		ui->stackedWidget->addWidget(new up_ChangePWD(ui->label_step, "changepassword3.png", Page_ChangeSSCardPWD));
		ui->stackedWidget->addWidget(new OperatorSucceed(nullptr, "", Page_Succeed));
		for (int i = 0; i < ui->stackedWidget->count(); i++)
		{
			QStackPage* pPage = dynamic_cast<QStackPage*>(ui->stackedWidget->widget(i));
			connect(pPage, &QStackPage::SwitchNextPage, this, &QMainStackPage::on_SwitchNextPage);
			connect(pPage, &QStackPage::SwitchPage, this, &QMainStackPage::on_SwitchPage);
			connect(pPage, &QStackPage::ShowMaskWidget, this, &QMainStackPage::On_ShowMaskWidget);
		}
	}
	catch (exception& e)
	{
		qDebug() << "Catch a exception:" << e.what();
	}
}

UpdatePassword::~UpdatePassword()
{
	delete ui;
}

void  UpdatePassword::ShowTimeOut(int nTimeout)
{
	ui->label_CountDown->setText(QString("%1").arg(nTimeout));
}

void UpdatePassword::OnTimerEvent()
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
