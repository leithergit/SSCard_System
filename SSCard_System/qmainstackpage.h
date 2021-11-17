#ifndef QMAINSTACKPAGE_H
#define QMAINSTACKPAGE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
#include <vector>
using namespace std;

#define     StartPageIndex     (0)
#define     FailedPageIndex     (0)

class QMainStackPage : public QWidget
{
	Q_OBJECT
public:
	explicit QMainStackPage(QWidget* parent = nullptr);
	int     m_nTimerID = 0;
	int     m_nTimeout = 0;
	int     m_nCurPageIndex = 0;
	QWidget* m_pMainWindow = nullptr;
	QStackedWidget* m_pStackWidget = nullptr;
	virtual void timerEvent(QTimerEvent* event);
	virtual void OnTimerEvent();
	bool eventFilter(QObject* object, QEvent* event);
	virtual void  ResetAllPages();

public slots:
	void on_pushButton_MainPage_clicked();
    virtual void on_SwitchNextPage(PageOperation nOperation);
    void On_ShowMaskWidget(QString strMessage,MaskStatus nStatus,PageOperation nOperation)
    {
        emit ShowMaskWidget(strMessage,nStatus, nOperation);
    }
signals:
   void ShowMaskWidget(QString strMessage,MaskStatus nStatus,PageOperation nOperation);
};

#endif // QMAINSTACKPAGE_H
