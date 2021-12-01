#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MaskWidget.h"
#include "Gloabal.h"
#include <QMessageBox>
#include <QPaintEngine>
#include <QPainter>

#include "qmainstackpage.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	//setStyleSheet(QString::fromUtf8("background-image:url(./Image/backgroud.jpg);"));
	//setStyleSheet(QString::fromUtf8("background-image:url(D:/Work/Henan_shangqiu/HNBXZM/Image/backgroud.jpg);"));
	//this->setStyleSheet(QString::fromUtf8(".QMainWindow{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(26, 37, 223, 255), stop:1 rgba(3, 152, 252, 255));}"));
	this->setStyleSheet(QString::fromUtf8("#MainWindow{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(26, 37, 223, 255), stop:1 rgba(3, 152, 252, 255));}"));
	//D:\Work\Henan_shangqiu\HNBXZM\Image

	m_nDateTimer = startTimer(1000);
	m_pMainpage = new MainPage(this);
	m_pUpdateCard = new UpdateCard(this);
	m_pUpdatePassword = new UpdatePassword(this);
	m_pRegiserLost = new RegisterLost(this);
	m_pMaskWindow = new MaskWidget(this);
	//m_pOperatorFailed = new OperatorFailed();
	ui->stackedWidget->addWidget(m_pMainpage);
	ui->stackedWidget->addWidget(m_pUpdateCard);
	ui->stackedWidget->addWidget(m_pUpdatePassword);
	ui->stackedWidget->addWidget(m_pRegiserLost);
	ui->stackedWidget->setCurrentWidget(m_pMainpage);
	m_pMainpage->show();

	/*
	Constant                Value           Description
	Qt::WindowNoState       0x00000000      The window has no state set (in normal state).
	Qt::WindowMinimized     0x00000001      The window is minimized (i.e. iconified).
	Qt::WindowMaximized     0x00000002      The window is maximized with a frame around it.
	Qt::WindowFullScreen    0x00000004      The window fills the entire screen without any frame around it.
	Qt::WindowActive        0x00000008      The window is the active window, i.e. it has keyboard focus.
	*/
	//setWindowFlags((Qt::WindowFlags)(windowFlags()|Qt::WindowStaysOnTopHint));
	//setWindowFlags((Qt::WindowFlags)(windowFlags()));
	//connect(m_pUpdateCard, SIGNAL(ShowMaskWidget(QString ,MaskStatus ,PageOperation )), this, SLOT(On_ShowMaskWidget(QString ,MaskStatus ,PageOperation)));
	connect(m_pUpdateCard, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
	connect(m_pUpdatePassword, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
	connect(m_pRegiserLost, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
	connect(m_pMaskWindow, &MaskWidget::MaskTimeout, this, &MainWindow::On_MaskWidgetTimeout);

}

MainWindow::~MainWindow()
{
	delete ui;
}

// void MainWindow::paintEvent(QPaintEvent* event)
// {
//     QPainter painter(this);
//     painter.setRenderHint(QPainter::Antialiasing, true);
//     QMainWindow::paintEvent(event);
// }

int MainWindow::LoadConfigure(QString& strError)
{
	if (g_pDataCenter)
	{
		QString strError;
		int nRes = 0;
		nRes = g_pDataCenter->LoadSysConfigure(strError);
		if (nRes != 0)
			return nRes;
		nRes = g_pDataCenter->LoadCardForm(strError);
		if (nRes != 0)
			return nRes;
		return 0;
	}
	else
	{
		gError() << "The Datacenter is not initialized!";
		return -1;
	}
}
void MainWindow::on_pushButton_Updatecard_clicked()
{
	QString strError;
	if (LoadConfigure(strError))
	{
		gError() << strError.toLatin1().data();
		QMessageBox::critical(this, tr(""), tr(""), QMessageBox::Ok);
		return;
	}
	ui->stackedWidget->setCurrentWidget(m_pUpdateCard);
	m_pUpdateCard->ResetAllPages();
	m_pUpdateCard->show();
}

void MainWindow::on_pushButton_ChangePWD_clicked()
{
	QString strError;
	if (LoadConfigure(strError))
	{
		gError() << strError.toLatin1().data();
		QMessageBox::critical(this, tr(""), tr(""), QMessageBox::Ok);
		return;
	}
	ui->stackedWidget->setCurrentWidget(m_pUpdatePassword);
	m_pUpdatePassword->ResetAllPages();
	m_pUpdatePassword->show();
}

void MainWindow::on_pushButton_RegisterLost_clicked()
{
	QString strError;
	if (LoadConfigure(strError))
	{
		gError() << strError.toLatin1().data();
		QMessageBox::critical(this, tr(""), tr(""), QMessageBox::Ok);
		return;
	}
	ui->stackedWidget->setCurrentWidget(m_pRegiserLost);
	m_pRegiserLost->ResetAllPages();
	m_pRegiserLost->show();
}

void MainWindow::on_pushButton_MainPage_clicked()
{
	gInfo() << "Return mainPage!";
	// 回到主页时需清空所有身份数据
	g_pDataCenter->ResetIDData();
	ui->stackedWidget->setCurrentWidget(m_pMainpage);
	m_pMainpage->show();
}

void MainWindow::On_ShowMaskWidget(QString strTitle, QString strDesc, int nStatus, int nPageOperation)
{
	QPoint ptLeftTop = m_pMainpage->mapToGlobal(QPoint(0, 0));
	m_pMaskWindow->setGeometry(m_pMainpage->geometry());
	m_pMaskWindow->move(ptLeftTop);
	int nTimeout = 2;
	switch (nStatus)
	{
	case Success:
		nTimeout = g_pDataCenter->GetSysConfigure()->nMaskTimeout[Success];
		[[fallthrough]];
	case Information:
		nTimeout = g_pDataCenter->GetSysConfigure()->nMaskTimeout[Information];
		break;
	case Error:
		nTimeout = g_pDataCenter->GetSysConfigure()->nMaskTimeout[Error];
		[[fallthrough]];
	case Failed:
		nTimeout = g_pDataCenter->GetSysConfigure()->nMaskTimeout[Failed];
		break;
	case Fetal:
		nTimeout = g_pDataCenter->GetSysConfigure()->nMaskTimeout[Fetal];
		break;
	}
	m_pMaskWindow->Popup(strTitle, strDesc, (int)nStatus, (int)nPageOperation, nTimeout);
}
void MainWindow::On_MaskWidgetTimeout(int nPageOperation)
{
	gInfo() << __FUNCTION__ << " Operation = " << g_szPageOperation[nPageOperation];
	if (m_pMaskWindow)
	{
		m_pMaskWindow->hide();
	}
	QMainStackPage* pCurPage = (QMainStackPage*)ui->stackedWidget->currentWidget();
	pCurPage->emit SwitchNextPage(nPageOperation);
}

void MainWindow::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == m_nDateTimer)
	{
		QDateTime   tNow = QDateTime::currentDateTime();
		ui->label_DateTime->setText(tNow.toString("yyyy-MM-dd HH:mm:ss"));
	}
}
