#ifndef WAITINGPROGRESS_H
#define WAITINGPROGRESS_H

#include <QWidget>
#include <thread>
using namespace  std;
namespace Ui {
	class WaitingProgress;
}

class WaitingProgress : public QWidget
{
	Q_OBJECT

public:
	explicit WaitingProgress(QWidget* parent = nullptr);
	~WaitingProgress();
	int     nWatingTimeout = 180;
	int		nMaxTime = 180;
	int     nTimerEvent = -1;
	bool	bPrinterReady = false;
	virtual void timerEvent(QTimerEvent* event) override;
	void ThreadTestPrinter();
	volatile bool bThreadTestPrinter = false;
	std::thread* pThread = nullptr;
signals:
	void PrinterIsReady();
public slots:
	void On_PrinterIsReady();

private:
	Ui::WaitingProgress* ui;
};

#endif // WAITINGPROGRESS_H
