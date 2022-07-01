#ifndef WAITINGPROGRESS_H
#define WAITINGPROGRESS_H

#include <QDialog>
#include <thread>
#include <functional>
using namespace  std;
namespace Ui {
	class WaitingProgress;
}

class WaitingProgress : public QDialog
{
	Q_OBJECT

public:
	explicit WaitingProgress(std::function<int(void*)> pInFunction, void* pParam, int nTimeout, QString strFormat, bool bLoop = true,QWidget* parent = nullptr);
	~WaitingProgress();
	int     nWatingTimeout = 180;
	int		nMaxTime = 180;
	int     nTimerEvent = -1;
	QString strFormat;
	QString strMessage;
	virtual void timerEvent(QTimerEvent* event) override;
	void ThreadFunction();
	std::thread* pThread = nullptr;
	volatile bool bLoop = true;

signals:
	void TheadFinished(/*int ,QString strMessage*/);
public slots:
	void On_TheadFinished(/*int, QString strMessage*/);

private:
	Ui::WaitingProgress* ui;
	std::function<int(void*)> pFunction;
	void* pParam = nullptr;
};

#endif // WAITINGPROGRESS_H
