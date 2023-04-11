#include "waitingprogress.h"
#include "ui_waitingprogress.h"
#include "Gloabal.h"

WaitingProgress::WaitingProgress(std::function<int(void*)> pInFunction, void* pInParam, int nTimeout, QString strFormat, bool bLoop, QWidget* parent) :
	QDialog(parent),
	pFunction(pInFunction),
	pParam(pInParam),
	ui(new Ui::WaitingProgress)
{
	ui->setupUi(this);
	this->bLoop = bLoop;
	nMaxTime = nTimeout * 10;
	this->nWatingTimeout = nTimeout * 10;
	this->strFormat = strFormat;
	setWindowFlags((Qt::WindowFlags)(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint));
	/*nWatingTimeout = g_pDataCenter->GetSysConfigure()->nTimeWaitForPrinter;
	nMaxTime = nWatingTimeout;*/
	nTimerEvent = startTimer(100);
	ui->progressBar->setRange(1, nWatingTimeout);
	ui->progressBar->setValue(1);
	ui->progressBar->setFormat(QString(strFormat).arg(QString::number(0, 'f', 0)));

	ui->progressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);  // 对齐方式
	pThread = new std::thread(&WaitingProgress::ThreadFunction, this);
	connect(this, &WaitingProgress::TheadFinished, this, &WaitingProgress::On_TheadFinished);
}

WaitingProgress::~WaitingProgress()
{
	bLoop = false;
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
		ui->progressBar->setFormat(QString(strFormat).arg(QString::number(fProgress, 'f', 0)));
		if (nWatingTimeout == 0)
		{
			killTimer(nTimerEvent);
			nTimerEvent = 0;
			QDialog::reject();
		}
	}
}

void WaitingProgress::ThreadFunction()
{
	QString strMessage;
	do
	{
		int nResult = pFunction(pParam);
		if (QSucceed(nResult))
		{
			break;
		}

		this_thread::sleep_for(milliseconds(100));
	} while (bLoop);
	emit TheadFinished();
}

void WaitingProgress::On_TheadFinished()
{
	bLoop = false;
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
	QDialog::accept();
}
