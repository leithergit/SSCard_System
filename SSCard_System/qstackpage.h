#ifndef QSTACKPAGE_H
#define QSTACKPAGE_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QThread>
#include <thread>
#include <string>
#include <memory>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QtDebug>
#include <QFileInfo>
#include <QCoreApplication>
#include <QPaintEvent>
#include <QPainter>
#include "Gloabal.h"


enum PageOperation
{
	Return_MainPage,
	Stay_CurrentPage,
	Switch_NextPage,
	Skip_NextPage,
	Retry_CurrentPage=1024
};
extern const char* g_szPageOperation[4];

using namespace std;
class QStackPage : public QWidget
{
	Q_OBJECT
public:
	explicit QStackPage(QLabel* pTitle, QString& strStepImage, Page_Index  nPageIndex, QWidget* parent = nullptr) :
		QWidget(parent),
		m_pTitle(pTitle),
		m_nPageIndex(nPageIndex)
	{
		SysConfigPtr& pSysConfig = g_pDataCenter->GetSysConfigure();
		if (nPageIndex < Page_ReaderIDCard ||
			nPageIndex > Page_AdforFinance)
			m_nTimeout = 30;
		else
			m_nTimeout = pSysConfig->nPageTimeout[nPageIndex];
		if (strStepImage.size())
		{
			QString strAppPath = QCoreApplication::applicationDirPath();
			QString strFullImagePath = QString("%1/Image/%2").arg(strAppPath).arg(strStepImage);
			qDebug() << "strImagePath = " << strFullImagePath;
			QFileInfo fi(strFullImagePath);
			if (!fi.isFile())
			{
				qDebug() << strFullImagePath << "not exist!";
			}
			m_strStepImage = QString("border-image: url(%1);").arg(strFullImagePath);;
		}
	}
	virtual ~QStackPage()
	{
		m_bWorkThreadRunning = false;
		if (m_pWorkThread && m_pWorkThread->joinable())
		{
			m_pWorkThread->join();
			delete m_pWorkThread;
			m_pWorkThread = nullptr;
		}
	}
	// 处理业务接口，成功时返回0，超时返回-1，其它值为具体错误码
	virtual int ProcessBussiness() { return 0; };
	virtual void OnTimeout() = 0;

	virtual void ActiveTitle(QString strTitle = "")
	{
		m_nCountDown = m_nTimeout;
		if (m_pTitle && m_strStepImage.size())
		{
			m_pTitle->setStyleSheet(m_strStepImage);
		}
	}

	/*virtual void DisActiveTitle()
	{
		ShutDown();
	}*/

	virtual void  ShutDown() = 0;
	QString m_strStepImage;
	QLabel* m_pTitle = nullptr;
	int     m_nTimeout = 0;
	int     m_nCountDown = 0;
	bool    m_bWorkThreadRunning = false;
	std::thread* m_pWorkThread = nullptr;
	Page_Index m_nPageIndex;
	// 	virtual void paintEvent(QPaintEvent* event) override
	// 	{
	// 		QPainter painter(this);
	// 		painter.setRenderHint(/*QPainter::Antialiasing | */QPainter::TextAntialiasing, true);
	// 		QWidget::paintEvent(event);
	// 	}
public slots:
	// 显示遮罩层
	// strMessage   遮罩层消息
	// 遮罩层图标状态  0为成功，1为失败，2为警告
	// 遮罩层滞留时间  单位毫秒
	// nNextPages   0 为保留当前页，1为切换下一页，2..3否则为需要跳过的页数,直接当前页+nNextPages页
	virtual void  OnShowMaskWidget(QString strMessage, int nStatus, int nOperation)
	{
		Q_UNUSED(strMessage);
		Q_UNUSED(nStatus);
		Q_UNUSED(nOperation);
		return;
	}

signals:
	void InputPin(unsigned char ch);
	void SwitchNextPage(int nOperation);
	//	void RetryCurrentPage(QStackPage* pCurrentPage);
	void SwitchPage(int nPage);
	void ErrorMessage(QString strMessage);
	void ShowMaskWidget(QString strTitle, QString strDesc, int nStatus, int nOperation);

};

#endif // QSTACKPAGE_H
