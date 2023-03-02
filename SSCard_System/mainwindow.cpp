#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MaskWidget.h"
#include "Gloabal.h"
#include <QMessageBox>
#include <QPaintEngine>
#include <QPainter>
#include "../update/Update.h"
#include "SystemManager.h"
#include "qmainstackpage.h"
#include "CheckPassword.h"
MaskWidget* g_pMaskWindow = nullptr;


int TestHost(string strIP, USHORT nPort, int nNetTimeout)
{
	bool bConnected = true;
	SOCKET s = INVALID_SOCKET;
	do
	{
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == INVALID_SOCKET)
		{
			bConnected = false;
			break;
		}

		//设置socket 选项
		//1.允许优雅关闭socket
		linger sLinger;
		sLinger.l_onoff = 1;	//(在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
		// 如果m_sLinger.l_onoff = 0;则功能和2.)作用相同;
		sLinger.l_linger = 2;//(容许逗留的时间为5秒)
		if (setsockopt(s, SOL_SOCKET, SO_LINGER, (const char*)&sLinger, sizeof(linger)) == SOCKET_ERROR)
		{
			bConnected = false;
			break;
		}

		//2.设置收发超时时限
		//int nNetTimeout = 500;//500毫秒
		//发送时限
		if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(int)) == SOCKET_ERROR)
		{
			bConnected = false;
			break;
		}

		//接收时限
		if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int)) == SOCKET_ERROR)
		{
			bConnected = false;
			break;
		}

		//3.设置数据收发的缓冲区
		//在send()的时候,返回的是实际发送出去的字节(同步)或发送到socket缓冲区的字节
		//(异步);系统默认的状态发送和接收一次为8688字节(约为8.5K)；在实际的过程中发送数据
		//和接收数据量比较大,可以设置socket缓冲区,而避免了send(),recv()不断的循环收发：
		//接收缓冲区
		int nRecvBuf = 1024;//设置为1024 byte
		if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int)) == SOCKET_ERROR)
		{
			bConnected = false;
			break;
		}

		// 设置为非阻塞模式
		unsigned long nBlockMode = 1;
		if (ioctlsocket(s, FIONBIO, (unsigned long*)&nBlockMode) == SOCKET_ERROR)
		{
			bConnected = false;
			break;
		}

		sockaddr_in ServerAddr;
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(nPort);
		ServerAddr.sin_addr.S_un.S_addr = inet_addr(strIP.c_str());
		//InetPton(AF_INET, strIP.c_str(), &ServerAddr.sin_addr);
		if (connect(s, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR &&
			GetLastError() != WSAEWOULDBLOCK)
		{  //连接失败
			bConnected = false;
			break;
		}
		struct timeval timeout;
		fd_set FDConnect;

		FD_ZERO(&FDConnect);
		FD_SET(s, &FDConnect);
		timeout.tv_sec = nNetTimeout / 1000;
		timeout.tv_usec = (nNetTimeout % 1000) * 1000;	//连接超时200 ms,tv_usec的时间单位是微秒
		int nRes = select(0, 0, &FDConnect, 0, &timeout);
		if (!nRes ||
			SOCKET_ERROR == nRes)
		{
			bConnected = false;
			break;
		}
		// 重新设置为阻塞模式
		nBlockMode = 0;
		if (ioctlsocket(s, FIONBIO, (unsigned long*)&nBlockMode) == SOCKET_ERROR)
		{
			bConnected = false;
			break;
		}
	} while (0);
	if (bConnected)
	{
		closesocket(s);
		return true;
	}
	else
		return false;
}

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	installEventFilter(this);
	BaseInfo Bi;
	RegionInfo& region = g_pDataCenter->GetSysConfigure()->Region;
	strcpy(Bi.strLicense, "1234567890");
	//strcpy(Bi.strLicense, region.strLicense.c_str());
	strcpy(Bi.strArea, region.strCityCode.c_str());
	strcpy(Bi.strEMUrl, region.strEMURL.c_str());
	strcpy(Bi.strAccount, region.strEMAccount.c_str());
	strcpy(Bi.strPassword, region.strEMPassword.c_str());
	gInfo() << "InitEnv" << "EMUrl:" << Bi.strEMUrl << "EMAccount:" << Bi.strAccount << "EMPassword:" << Bi.strPassword;
	if (!InitEnv(HN_SSCARD, Bi))
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
	m_pNewCard = new NewCard(this);
	m_pUpdateCard = new UpdateCard(this);
	m_pUpdatePassword = new UpdatePassword(this);
	m_pRegiserLost = new RegisterLost(this);
	//m_pMaskWindow = new MaskWidget(this);

	//m_pOperatorFailed = new OperatorFailed();
	ui->stackedWidget->addWidget(m_pMainpage);
	ui->stackedWidget->addWidget(m_pNewCard);
	ui->stackedWidget->addWidget(m_pUpdateCard);
	ui->stackedWidget->addWidget(m_pUpdatePassword);
	ui->stackedWidget->addWidget(m_pRegiserLost);
	ui->stackedWidget->setCurrentWidget(m_pMainpage);
	ui->checkBox_Debug->setVisible(g_pDataCenter->bDebug);
	m_pMainpage->show();
	QString strError;
	LoadConfigure(strError);		// 加载配置时会自动关闭已经打开的打印和读卡器

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
	setWindowFlags((Qt::WindowFlags)(windowFlags() /*| Qt::WindowStaysOnTopHint*/ | Qt::FramelessWindowHint));
	// Qt::WindowFlags flags = w.windowFlags();

	//connect(m_pUpdateCard, SIGNAL(ShowMaskWidget(QString ,MaskStatus ,PageOperation )), this, SLOT(On_ShowMaskWidget(QString ,MaskStatus ,PageOperation)));
	connect(m_pNewCard, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
	connect(m_pUpdateCard, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
	connect(m_pUpdatePassword, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
	connect(m_pRegiserLost, &QMainStackPage::ShowMaskWidget, this, &MainWindow::On_ShowMaskWidget);
	connect(this, &MainWindow::Shutdown, this, &MainWindow::on_Shutdown);
	connect(this, &MainWindow::LoadSystemManager, this, &MainWindow::On_LoadSystemManager);
	// 	bThreadUploadlogRunning = true;
	// 	ThreadUploadlog = std::thread(&MainWindow::fnThreadUploadlog, this);
	if (g_pDataCenter->bEnableUpdate)
	{
		bThreadUpdateLauncherRunning = true;
		pThreadUpdateLauncher = new std::thread(&MainWindow::ThreadUpdateLauncher, this);
	}
	else
	{
		gWarning() << GBKStr("警告:自动升级功能被关闭!");
	}

	m_nTimerTestHost = startTimer(5000);
	QString strMessage;
	string strBankName;
	if (QFailed(g_pDataCenter->CheckBankCode(strBankName, strMessage)))
	{
		strMessage = "银行代码配置有误或未配置银行代码!";
		QMessageBox_CN(QMessageBox::Critical, "严重错误", strMessage, QMessageBox::Ok, this);
	}
	ui->label_BankName->setText(strBankName.c_str());
	ui->label_Title->setText(g_pDataCenter->strTitle.c_str());

	QString strAppPath = QApplication::applicationFilePath();
	short nMajorVer = 0, nMinorVer = 0, nBuildNum = 0, nRevsion = 0;
	if (GetModuleVersion(QApplication::applicationFilePath().toStdString(), nMajorVer, nMinorVer, nBuildNum, nRevsion))
	{
		QString strVersion = QString("当前版本:%1.%2.%3.%4").arg(nMajorVer).arg(nMinorVer).arg(nBuildNum).arg(nRevsion);
		ui->label_Version->setText(strVersion);
	}
}

void MainWindow::On_LoadSystemManager()
{
	SystemTool& sysTool = SystemTool::Instance();
	sysTool.OpenScreenKeyboard();
	CheckPassword checkpwd;
	if (checkpwd.exec() == QDialog::Accepted)
	{
		QWaitCursor Wait;
		if (pThreadAsync)
		{
			pThreadAsync->join();
			delete pThreadAsync;
			pThreadAsync = nullptr;
		}
		Wait.RestoreCursor();
		SystemManager d(this);
		d.setGeometry(g_pCurScreen->geometry());
		d.exec();
	}
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
	if ((e->x() >= 1820 && e->y() >= 900) &&
		(e->x() <= 1920 && e->y() <= 990))
	{
		qDebug() << "Mouse X=" << e->x() << " Mouse Y=" << e->y();
		auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - m_tLastPress);
		if (tDuration.count() < 1000)
		{
			m_nContinuePressCount++;
			if (m_nContinuePressCount >= 6)
				emit LoadSystemManager();
		}
		else
			m_nContinuePressCount = 1;
	}
	else
		m_nContinuePressCount = 1;
	m_tLastPress = chrono::high_resolution_clock::now();
}

MainWindow::~MainWindow()
{
	g_pDataCenter->CloseCamera();
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
		g_pDataCenter->LoadAdminConfigure();
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

void MainWindow::on_pushButton_NewCard_clicked()
{
	QWaitCursor Wait;
	if (pThreadAsync)
	{
		pThreadAsync->join();
		delete pThreadAsync;
		pThreadAsync = nullptr;
	}
	if (pLastStackPage)
	{
		pLastStackPage->ResetAllPages(0);
		pLastStackPage = nullptr;
	}
	QString strError;
	if (LoadConfigure(strError))		// 加载配置时会自动关闭已经打开的打印和读卡器
	{
		gError() << strError.toLatin1().data();
		QMessageBox::critical(this, tr(""), tr(""), QMessageBox::Ok);
		return;
	}

	QString strMessage;
	m_pNewCard->ResetAllPages();
	int nResult = -1;
	if (QFailed(nResult = g_pDataCenter->OpenDevice(strMessage)))
	{
		m_pNewCard->emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}

	if (QFailed(nResult = g_pDataCenter->TestPrinter(strMessage)))
	{
		m_pNewCard->emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}

	/*if (QFailed(nResult = g_pDataCenter->TestCard(strMessage)))
	{
		m_pNewCard->emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}*/
	g_pDataCenter->ResetIDData();
	g_pDataCenter->nCardServiceType = ServiceType::Service_NewCard;
	m_pNewCard->StartBusiness();
	ui->stackedWidget->setCurrentWidget(m_pNewCard);
	m_pNewCard->show();
	pLastStackPage = m_pNewCard;
}

void MainWindow::on_pushButton_Updatecard_clicked()
{
	QWaitCursor Wait;
	if (pThreadAsync)
	{
		pThreadAsync->join();
		delete pThreadAsync;
		pThreadAsync = nullptr;
	}

	if (pLastStackPage)
	{
		pLastStackPage->ResetAllPages(0);
		pLastStackPage = nullptr;
	}
	QString strError;
	if (LoadConfigure(strError))		// 加载配置时会自动关闭已经打开的打印和读卡器
	{
		gError() << strError.toLatin1().data();
		QMessageBox::critical(this, tr(""), tr(""), QMessageBox::Ok);
		return;
	}

	QString strMessage;
	m_pUpdateCard->ResetAllPages();
	int nResult = -1;
	if (QFailed(nResult = g_pDataCenter->OpenDevice(strMessage)))
	{
		m_pUpdateCard->emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}

	if (QFailed(nResult = g_pDataCenter->TestPrinter(strMessage)))
	{
		m_pUpdateCard->emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}

	/*if (QFailed(nResult = g_pDataCenter->TestCard(strMessage)))
	{
		m_pUpdateCard->emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}*/
	g_pDataCenter->ResetIDData();
	g_pDataCenter->nCardServiceType = ServiceType::Service_ReplaceCard;
	m_pUpdateCard->StartBusiness();
	ui->stackedWidget->setCurrentWidget(m_pUpdateCard);

	m_pUpdateCard->show();
	pLastStackPage = m_pUpdateCard;
}

void MainWindow::on_pushButton_ChangePWD_clicked()
{
	QWaitCursor Wait;
	if (pThreadAsync)
	{
		pThreadAsync->join();
		delete pThreadAsync;
		pThreadAsync = nullptr;
	}
	if (pLastStackPage)
	{
		pLastStackPage->ResetAllPages(0);
		pLastStackPage = nullptr;
	}
	QString strError;
	if (LoadConfigure(strError))
	{
		gError() << strError.toLatin1().data();
		QMessageBox::critical(this, tr(""), tr(""), QMessageBox::Ok);
		return;
	}

	m_pUpdatePassword->ResetAllPages();
	m_pUpdatePassword->StartBusiness();
	ui->stackedWidget->setCurrentWidget(m_pUpdatePassword);

	m_pUpdatePassword->show();
	pLastStackPage = m_pUpdatePassword;
}

void MainWindow::on_pushButton_RegisterLost_clicked()
{
	QWaitCursor Wait;
	if (pThreadAsync)
	{
		pThreadAsync->join();
		delete pThreadAsync;
		pThreadAsync = nullptr;
	}
	if (pLastStackPage)
	{
		pLastStackPage->ResetAllPages(0);
		pLastStackPage = nullptr;
	}
	QString strError;
	if (LoadConfigure(strError))
	{
		gError() << strError.toLatin1().data();
		QMessageBox::critical(this, tr(""), tr(""), QMessageBox::Ok);
		return;
	}

	m_pRegiserLost->ResetAllPages();
	g_pDataCenter->nCardServiceType = ServiceType::Service_RegisterLost;
	m_pRegiserLost->StartBusiness();
	ui->stackedWidget->setCurrentWidget(m_pRegiserLost);

	m_pRegiserLost->show();
	pLastStackPage = m_pRegiserLost;
}

void MainWindow::on_pushButton_MainPage_clicked()
{
	gInfo() << "Return mainPage!";

	if (pThreadAsync)
	{
		pThreadAsync->join();
		delete pThreadAsync;
	}
	pThreadAsync = new std::thread([]() {
		char szRCode[128] = { 0 };
		if (g_pDataCenter->GetPrinter())
			g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);
		g_pDataCenter->CloseDevice();
		});

	// 回到主页时需清空所有身份数据
	if (pLastStackPage)
	{
		pLastStackPage->ResetAllPages(0);
		if (pLastStackPage->m_nTimerID)
		{
			killTimer(pLastStackPage->m_nTimerID);
			pLastStackPage->m_nTimerID = 0;
		}
		pLastStackPage = nullptr;
	}
	g_pDataCenter->ResetIDData();
	ui->stackedWidget->setCurrentWidget(m_pMainpage);
	m_pMainpage->show();
}

void MainWindow::On_ShowMaskWidget(QString strTitle, QString strDesc, int nStatus, int nOperation, int nPage)
{
	QMainStackPage* pCurPageSender = qobject_cast<QMainStackPage*>(sender());

	m_pMaskWindow = new MaskWidget(this);
	g_pMaskWindow = m_pMaskWindow;
	if (pCurPageSender)
	{
		m_pMaskWindow->setGeometry(pCurPageSender->geometry());
		QPoint ptLeftTop = pCurPageSender->mapToGlobal(QPoint(0, 0));
		m_pMaskWindow->move(ptLeftTop);
	}
	if (nOperation == Return_MainPage)
	{
		char szRCode[128] = { 0 };
		if (g_pDataCenter->GetPrinter())
			g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);
	}

	connect(m_pMaskWindow, &MaskWidget::MaskTimeout, this, &MainWindow::On_MaskWidgetTimeout);
	connect(m_pMaskWindow, &MaskWidget::MaskEnsure, this, &MainWindow::On_MaskWidgetTimeout);

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
	case Nop:
		break;
	}
	m_pMaskWindow->Popup(strTitle, strDesc, (int)nStatus, (int)nOperation, (int)nPage, nTimeout);
}

void MainWindow::On_MaskWidgetEnsure(int nOperation, int nPage)
{
	if (m_pMaskWindow)
	{
		//m_pMaskWindow->hide();
		disconnect(m_pMaskWindow, &MaskWidget::MaskEnsure, this, &MainWindow::On_MaskWidgetEnsure);
		delete m_pMaskWindow;
		m_pMaskWindow = nullptr;
	}
	QMainStackPage* pCurPage = (QMainStackPage*)ui->stackedWidget->currentWidget();
	pCurPage->emit SwitchPage(nOperation, nPage);
}

void MainWindow::On_MaskWidgetTimeout(int nOperation, int nPage)
{
	//gInfo() << __FUNCTION__ << " Operation = " << g_szPageOperation[nPageOperation];
	if (m_pMaskWindow)
	{
		//m_pMaskWindow->hide();
		disconnect(m_pMaskWindow, &MaskWidget::MaskTimeout, this, &MainWindow::On_MaskWidgetTimeout);
		delete m_pMaskWindow;
		m_pMaskWindow = nullptr;
	}
	if (pLastStackPage && nOperation != Stay_CurrentPage)
	{
		if (pLastStackPage->m_nTimerID)
		{
			killTimer(pLastStackPage->m_nTimerID);
			pLastStackPage->m_nTimerID = 0;
		}
	}

	QMainStackPage* pCurPage = (QMainStackPage*)ui->stackedWidget->currentWidget();
	pCurPage->emit SwitchPage(nOperation, nPage);
}

void MainWindow::SwitchPage(int nOperation)
{
	QMainStackPage* pCurPage = (QMainStackPage*)ui->stackedWidget->currentWidget();
	if (pCurPage)
		pCurPage->emit SwitchPage(Switch_NextPage, 0);
}

void MainWindow::on_Shutdown()
{
	if (m_nDateTimer)
	{
		killTimer(m_nDateTimer);
		m_nDateTimer = 0;
	}

	if (pThreadUpdateLauncher)
	{
		bThreadUpdateLauncherRunning = false;
		pThreadUpdateLauncher->join();
		delete pThreadUpdateLauncher;
		pThreadUpdateLauncher = nullptr;
	}
	close();
}

void MainWindow::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == m_nDateTimer)
	{
		QDateTime   tNow = QDateTime::currentDateTime();
		ui->label_DateTime->setText(tNow.toString("yyyy-MM-dd HH:mm:ss"));
	}
	else if (event->timerId() == m_nTimerTestHost)
	{
		RegionInfo& region = g_pDataCenter->GetSysConfigure()->Region;
		QString strUrl = region.strEMURL.c_str();
		QStringList  strField = strUrl.split(':');
		if (strField.size())
		{
			QString strIP = strField[1].mid(2);
			USHORT  nPort = (USHORT)strField[2].toShort();
			if (TestHost(strIP.toStdString(), nPort, g_pDataCenter->nNetTimeout))
			{// 连续成功
				m_nNetworkFailedCount = 0;
				if (m_nTimerNetWarning)
				{
					killTimer(m_nTimerNetWarning);
					m_nTimerNetWarning = 0;
					bDisconnect = false;
					ui->label_Net->setText("网络良好");
					ui->label_NetWarning->setStyleSheet("border-image:url(:/Image/network-connected.png);");
				}
			}
			else
			{// 连接失败
				m_nNetworkFailedCount++;
				if (m_nNetworkFailedCount >= 3)
				{
					m_nTimerNetWarning = startTimer(400);
					bDisconnect = true;
					ui->label_Net->setText("网络异常");
					ui->label_NetWarning->setStyleSheet("border-image:url(:/Image/network-disconnected.png);");
				}
			}
		}
	}
	else if (event->timerId() == m_nTimerNetWarning)
	{
		QString strQSS = "border-image:url(:/Image/network-disconnected.png);";
		if (bDisconnect)
		{
			bDisconnect = false;
		}
		else
		{
			strQSS = "";
			bDisconnect = true;
		}
		ui->label_NetWarning->setStyleSheet(strQSS);
	}
}
void MainWindow::closeEvent(QCloseEvent* event)
{
	if (m_nDateTimer)
	{
		killTimer(m_nDateTimer);
		m_nDateTimer = 0;
	}
}

// void MainWindow::fnThreadUploadlog()
// {
// 	auto tStart = chrono::high_resolution_clock::now();
// 	while (bThreadUploadlogRunning)
// 	{
// #pragma Warning("日志上传功能尚未完成!")
// 		this_thread::sleep_for(chrono::milliseconds(100));
// 	}
// }

void MainWindow::ThreadUpdateLauncher()
{
	auto tStart = chrono::high_resolution_clock::now() - chrono::seconds(15 * 60 - 1);
	while (bThreadUpdateLauncherRunning)
	{
		auto duration = duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - tStart);
		if (duration.count() > 15 * 60)
		{
			string strMessage;
			Update(UpdateType::Launcher, strMessage);
			/*string strMessage, strLocalVersion, strNewVersion, strFilePath;
			do
			{
				if (QFailed(LoadUpgradeInfo(strMessage)))
					break;

				if (QFailed(GetLocalVersion("Launcher.exe", strLocalVersion, strMessage)))
				{
					gError() << "Failed in GetLocalVersion:" << strMessage;
					break;
				}
				gInfo() << "Launcher.exe" << " Local Version" << strLocalVersion;
				if (QFailed(CheckNewVersion(UpdateType::Launcher, strLocalVersion, strNewVersion, strMessage)))
				{
					gError() << "Failed in CheckNewVersion" << strMessage;
					break;
				}
				gInfo() << "Launcher.exe" << " Remote Version" << strNewVersion;
				if (QFailed(DownloadNewVerion(UpdateType::Launcher, strNewVersion, strFilePath, strMessage)))
				{
					gError() << "Failed in DownloadNewVerion" << strMessage;
					break;
				}
				gInfo() << "Try to BackupOldVersion";
				BackupOldVersion(UpdateType::Launcher, strFilePath);
				if (QFailed(InstallNewVersion(strFilePath, strMessage)))
				{
					gError() << "Failed in InstallNewVersion" << strMessage;
					break;
				}

			} while (true);*/
			tStart = chrono::high_resolution_clock::now();
		}
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}
