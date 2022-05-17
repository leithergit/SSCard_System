#ifndef QMAINSTACKPAGE_H
#define QMAINSTACKPAGE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
#include <vector>
#include <QPaintEvent>
#include <QPainter>
using namespace std;

#define     StartPageIndex     (0)
#define     FailedPageIndex     (0)

class QMainStackPage : public QWidget
{
	Q_OBJECT
public:
	explicit QMainStackPage(QWidget* parent = nullptr);
	~QMainStackPage();
	void AddPage(QWidget* pWidget);
	int     m_nTimerID = 0;
	int     m_nTimeout = 0;
	int     m_nCurPageIndex = 0;
	vector<QStackPage*>vecStackPage;
	QWidget* m_pMainWindow = nullptr;
	QStackedWidget* m_pStackWidget = nullptr;
	virtual void timerEvent(QTimerEvent* event) override;
	virtual void OnTimerEvent();
	bool eventFilter(QObject* object, QEvent* event) override;
	virtual void  ResetAllPages(int nStartPage = 1);
	virtual void  StartBusiness();
	virtual void  ShowTimeOut(int nTimeout) {};
	virtual void  ResetTimer(bool boolStop, QStackPage* pCurPage = nullptr);

public slots:
	void on_pushButton_MainPage_clicked();
	virtual void on_SwitchPage(int nOperation, int nPage = 0);
	void On_ShowMaskWidget(QString strTitle, QString strDesc, int nStatus, int nOperation, int nPage)
	{
		qDebug() << __FUNCTION__ << "strTitle = " << strTitle << "strDesc = " << strDesc << "nStatus = " << nStatus << "nOperation = " << nOperation;
		emit ShowMaskWidget(strTitle, strDesc, nStatus, nOperation, nPage);
	}
signals:
	void ShowMaskWidget(QString strTitle, QString strDesc, int nStatus, int nOperation, int nPage = 0);
	void SwitchPage(int nOperation, int nPage = 0);
};

#endif // QMAINSTACKPAGE_H
