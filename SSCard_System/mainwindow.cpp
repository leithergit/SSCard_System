#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MaskWidget.h"
#include "Gloabal.h"
#include <QMessageBox>
#include <QPaintEngine>
#include <QPainter>

#include "qmainstackpage.h"
MaskWidget* g_pMaskWindow = nullptr;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	installEventFilter(this);
	BaseInfo Bi;
	RegionInfo& region = g_pDataCenter->GetSysConfigure()->Region;
	strcpy(Bi.strEMUrl, region.strEMURL.c_str());
	strcpy(Bi.strAccount, region.strEMAccount.c_str());
	strcpy(Bi.strPassword, region.strEMPassword.c_str());
	if (!InitEnv(Bi))
	{
		gInfo() << "Failed in InitEnv";
	}

	//setStyleSheet(QString::fromUtf8("background-image:url(./Image/backgroud.jpg);"));
	//setStyleSheet(QString::fromUtf8("background-image:url(D:/Work/Henan_shangqiu/HNBXZM/Image/backgroud.jpg);"));
	//this->setStyleSheet(QString::fromUtf8(".QMainWindow{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(26, 37, 223, 255), stop:1 rgba(3, 152, 252, 255));}"));
	this->setStyleSheet(QString::fromUtf8("#MainWindow{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(26, 37, 223, 255), stop:1 rgba(3, 152, 252, 255));}"));
	ui->label_logo->setStyleSheet("border-image: url(./Image/banklogo.png);");

	m_nDateTimer = startTimer(1000);
	m_pMainpage = new MainPage(this);
	m_pUpdateCard = new UpdateCard(this);
	m_pUpdatePassword = new UpdatePassword(this);
	m_pRegiserLost = new RegisterLost(this);
	//m_pMaskWindow = new MaskWidget(this);

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
	flags |= Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint;
	w.setWindowFlags(flags);
	*/
	//setWindowFlags((Qt::WindowFlags)(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags((Qt::WindowFlags)(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint));
	// Qt::WindowFlags flags = w.windowFlags();

	//connect(m_pUpdateCard, SIGNAL(ShowMaskWidget(QString ,MaskStatus ,PageOperation )), this, SLOT(On_ShowMaskWidget(QString ,MaskStatus ,PageOperation)));
	connect(m_pUpdateCard, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
	connect(m_pUpdatePassword, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
	connect(m_pRegiserLost, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
	qDebug() << "Mouse X=" << e->x() << " Mouse Y=" << e->y();
	/*
 Mouse X= 1887  Mouse Y= 959
Mouse X= 1887  Mouse Y= 959
Mouse X= 1887  Mouse Y= 959
Mouse X= 1887  Mouse Y= 959
Mouse X= 1887  Mouse Y= 959
Mouse X= 1887  Mouse Y= 959
The thread 0xb9c has exited with code 0 (0x0).
The thread 0xc08 has exited with code 0 (0x0).
The thread 0x838 has exited with code 0 (0x0).
The thread 0x1328 has exited with code 0 (0x0).
Mouse X= 1887  Mouse Y= 959
Mouse X= 1849  Mouse Y= 924
Mouse X= 1849  Mouse Y= 924
Mouse X= 1849  Mouse Y= 924
Mouse X= 1849  Mouse Y= 924
Mouse X= 1915  Mouse Y= 985
Mouse X= 1915  Mouse Y= 985
Mouse X= 1915  Mouse Y= 985
Mouse X= 1915  Mouse Y= 985

	*/
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
	QMainStackPage* pCurPageSender = qobject_cast<QMainStackPage*>(sender());
	//if (m_pMaskWindow)
	//{
	//	delete m_pMaskWindow;
	//}

	m_pMaskWindow = new MaskWidget(this);
	g_pMaskWindow = m_pMaskWindow;
	if (pCurPageSender)
	{
		m_pMaskWindow->setGeometry(pCurPageSender->geometry());
		QPoint ptLeftTop = pCurPageSender->mapToGlobal(QPoint(0, 0));
		m_pMaskWindow->move(ptLeftTop);
	}

	connect(m_pMaskWindow, &MaskWidget::MaskTimeout, this, &MainWindow::On_MaskWidgetTimeout);

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
		//m_pMaskWindow->hide();
		disconnect(m_pMaskWindow, &MaskWidget::MaskTimeout, this, &MainWindow::On_MaskWidgetTimeout);
		delete m_pMaskWindow;
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
