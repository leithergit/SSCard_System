#include "mainwindow.h"
#include "queryinfo.h"
#include "ui_queryinfo.h"
#include "uc_readidcard.h"
#include "qi_information.h"
#include "qu_modifyinfo.h"
#include "uc_inputidcardinfo.h"
#include "nc_commitpersoninfo.h"
#include "OperatorSucceed.h"

QueryInfo::QueryInfo(QWidget* parent) :
	QMainStackPage(parent),
	ui(new Ui::QueryInfo)
{
	ui->setupUi(this);
	try
	{
		m_pStackWidget = ui->stackedWidget;
		//ui->stackedWidget->addWidget(new FailedWindow(nullptr));
		//SysConfigPtr& pSysConfig = g_pDataCenter->GetSysConfigure();
		AddPage(new uc_ReadIDCard(ui->label_step, "QueryInfo1.png", Page_ReaderIDCard, ReadID_QueryInfo));
		uc_InputIDCardInfo* puc_InputIDCardInfo = new uc_InputIDCardInfo(ui->label_step, "QueryInfo1.png", Page_InputIDCardInfo);
		AddPage(puc_InputIDCardInfo);		// step 1
		AddPage(new qi_Information(ui->label_step, "QueryInfo2.png", Page_QueryInformation));
		qu_ModifyInfo* pnc_ModifyInfo = new qu_ModifyInfo(ui->label_step, "Sandong/newcard3.png", Page_ModifyInfo);
		AddPage(pnc_ModifyInfo);		// step 2
		AddPage(new OperatorSucceed(nullptr, "", Page_Succeed));
		for (int i = 0; i < m_pStackWidget->count(); i++)
		{
			QStackPage* pPage = dynamic_cast<QStackPage*>(ui->stackedWidget->widget(i));
			//connect(pPage, &QStackPage::SwitchNextPage, this, &QMainStackPage::on_SwitchNextPage);
			connect(pPage, &QStackPage::SwitchPage, this, &QMainStackPage::on_SwitchPage);
			connect(pPage, &QStackPage::ShowMaskWidget, this, &QMainStackPage::On_ShowMaskWidget);
		}
		puc_InputIDCardInfo->HideItem();

	}
	catch (exception& e)
	{
		qDebug() << "Catch a exception:" << e.what();
	}
}

QueryInfo::~QueryInfo()
{
	delete ui;
}

void  QueryInfo::ShowTimeOut(int nTimeout)
{
	ui->label_CountDown->setText(QString("%1").arg(nTimeout));
}

void QueryInfo::OnTimerEvent()
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
