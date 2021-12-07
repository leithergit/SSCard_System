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
	int     m_nTimerID = 0;
	int     m_nTimeout = 0;
	int     m_nCurPageIndex = 0;
	QWidget* m_pMainWindow = nullptr;
	QStackedWidget* m_pStackWidget = nullptr;
	virtual void timerEvent(QTimerEvent* event) override;
	virtual void OnTimerEvent();
	bool eventFilter(QObject* object, QEvent* event) override;
	virtual void  ResetAllPages();
	// 	virtual void paintEvent(QPaintEvent* event) override
	// 	{
	// 		QPainter painter(this);
	// 		painter.setRenderHint(/*QPainter::Antialiasing | */QPainter::TextAntialiasing, true);
	// 		QWidget::paintEvent(event);
	// 	}

public slots:
	void on_pushButton_MainPage_clicked();
	virtual void on_SwitchNextPage(int nPageOperation);
	virtual void on_SwitchPage(int nPage);
	void On_ShowMaskWidget(QString strTitle, QString strDesc, int nStatus, int nPageOperation)
	{
		qDebug() << __FUNCTION__ << "strTitle = " << strTitle << "strDesc = " << strDesc << "nStatus = " << nStatus << "nOperation = " << nPageOperation;
		emit ShowMaskWidget(strTitle, strDesc, nStatus, nPageOperation);
	}
signals:
	void ShowMaskWidget(QString strTitle, QString strDesc, int nStatus, int nPageOperation);
	void SwitchNextPage(int nOperation);
	void SwitchPage(int nPage);
};

#endif // QMAINSTACKPAGE_H
