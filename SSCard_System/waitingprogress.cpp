#include "waitingprogress.h"
#include "ui_waitingprogress.h"
#include "Gloabal.h"

WaitingProgress::WaitingProgress(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::WaitingProgress)
{
	ui->setupUi(this);
	setWindowFlags((Qt::WindowFlags)(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint));
	nWatingTimeout = g_pDataCenter->GetSysConfigure()->nTimeWaitForPrinter;
	nMaxTime = nWatingTimeout;
	nTimerEvent = startTimer(1000);
	ui->progressBar->setRange(1, nWatingTimeout);
	ui->progressBar->setValue(1);
	ui->progressBar->setFormat(tr("正在初始化打印机:%1%").arg(QString::number(0, 'f', 0)));
	ui->progressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);  // 对齐方式


	bThreadTestPrinter = true;
	pThread = new std::thread(&WaitingProgress::ThreadTestPrinter, this);
	connect(this, &WaitingProgress::PrinterIsReady, this, &WaitingProgress::On_PrinterIsReady);
}

WaitingProgress::~WaitingProgress()
{
	bThreadTestPrinter = false;
	if (pThread)
	{
		pThread->join();
		delete pThread;
	}
	delete ui;
}

void WaitingProgress::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == nTimerEvent)
	{
		QString strMessage;
		nWatingTimeout--;
		ui->progressBar->setValue(nMaxTime - nWatingTimeout);
		float fProgress = (nMaxTime - nWatingTimeout) * 100 / nMaxTime;
		ui->progressBar->setFormat(tr("正在初始化打印机:%1%").arg(QString::number(fProgress, 'f', 0)));
		if (nWatingTimeout == 0)
		{
			killTimer(nTimerEvent);
			nTimerEvent = 0;
			close();
		}
	}
}

void WaitingProgress::ThreadTestPrinter()
{
	int nResult = -1;
	QString strMessage;
	while (bThreadTestPrinter)
	{
		if (QFailed(nResult = g_pDataCenter->OpenPrinter(strMessage)))
		{
			gInfo() << gQStr(strMessage);
			continue;
		}
		if (nResult == 0)
		{
			bPrinterReady = true;
			emit PrinterIsReady();
			break;
		}
	}
}

void WaitingProgress::On_PrinterIsReady()
{
	bThreadTestPrinter = false;
	if (pThread)
	{
		pThread->join();
		delete pThread;
		pThread = nullptr;
	}
	if (nTimerEvent)
	{
		killTimer(nTimerEvent);
		nTimerEvent = 0;
	}
	close();
}
