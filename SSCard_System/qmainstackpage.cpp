#pragma execution_character_set("utf-8")
#include "qmainstackpage.h"
#include "mainwindow.h"
#include <QTimer>
#include <QTimerEvent>
#include <QApplication>
#include <QKeyEvent>
#include "qstackpage.h"
//#include "mainwindow.h"

QMainStackPage::QMainStackPage(QWidget* parent)
	: QWidget(parent)
{
	foreach(QWidget * w, qApp->topLevelWidgets())
	{
		if ((w = qobject_cast<QMainWindow*>(w)))
			m_pMainWindow = w;
	}

	installEventFilter(this);
	connect(this, &QMainStackPage::SwitchNextPage, this, &QMainStackPage::on_SwitchNextPage);
	connect(this, &QMainStackPage::SwitchPage, this, &QMainStackPage::on_SwitchPage);
}

QMainStackPage::~QMainStackPage()
{
	if (m_nTimerID)
	{
		killTimer(m_nTimerID);
		m_nTimerID = 0;
	}

}
void QMainStackPage::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == m_nTimerID)
	{
		OnTimerEvent();
	}
}

void QMainStackPage::OnTimerEvent()
{

}

bool QMainStackPage::eventFilter(QObject* object, QEvent* event)
{
	Q_UNUSED(object);

#ifdef _DEBUG
	if (event->type() == QEvent::KeyRelease)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		if (g_pDataCenter->bDebug)
		{
			if (keyEvent->key() == Qt::Key_Return)
			{
				on_SwitchNextPage(Switch_NextPage);
				return true;
			}
		}

		//else if (keyEvent->key() == Qt::Key_Backspace)
		//{
		//	//on_pushButton_MainPage_clicked();
		//	return true;
		//}
		else if (keyEvent->key() == Qt::Key_Escape)
		{
			killTimer(m_nTimerID);
			QString strMessage = "按下ESC键模拟因错误返回主页!";
			emit ShowMaskWidget(strMessage, "", Failed, Stay_CurrentPage);
			return true;
		}
	}
#endif
	return false;
}

void QMainStackPage::on_pushButton_MainPage_clicked()
{
	if (m_nTimerID)
	{
		killTimer(m_nTimerID);
		m_nTimerID = 0;
		//m_nTimeout = 30;
	}
	((MainWindow*)m_pMainWindow)->on_pushButton_MainPage_clicked();
}

void QMainStackPage::ResetAllPages(int nStartPage)
{
	if (m_nTimerID)
	{
		killTimer(m_nTimerID);
		m_nTimerID = 0;
		qDebug() << "KillTimer:" << m_nTimerID;
	}
	g_pDataCenter->ResetIDData();
	if (m_pStackWidget)
	{
		int nPageCount = m_pStackWidget->count();
		for (int i = nStartPage; i < nPageCount; i++)
		{
			QStackPage* pCurPage = dynamic_cast<QStackPage*>(m_pStackWidget->widget(i));
			pCurPage->ShutDown();
		}

	}
}

void QMainStackPage::StartBusiness()
{
	m_nTimerID = startTimer(1000);
	qDebug() << "m_nTimerID:" << m_nTimerID;
	if (m_pStackWidget)
	{
		m_pStackWidget->setCurrentIndex(StartPageIndex);
		QStackPage* pCurPage = dynamic_cast<QStackPage*>(m_pStackWidget->widget(0));
		pCurPage->ActiveTitle();
		ShowTimeOut(pCurPage->m_nTimeout);
		pCurPage->ProcessBussiness();
		m_nTimeout = pCurPage->m_nTimeout;
	}
}

void  QMainStackPage::ResetTimer(bool boolStop, QStackPage* pCurPage)
{
	if (boolStop)
	{
		if (m_nTimerID)
		{
			killTimer(m_nTimerID);
			m_nTimerID = 0;
			qDebug() << "KillTimer:" << m_nTimerID;
		}
	}
	else
	{
		m_nTimerID = startTimer(1000);
		qDebug() << "m_nTimerID:" << m_nTimerID;
		m_nTimeout = pCurPage->m_nTimeout;
		ShowTimeOut(pCurPage->m_nTimeout);
	}
}

void QMainStackPage::on_SwitchNextPage(int nOperation)
{
	//if (nPageOperation < sizeof(g_szPageOperation) / sizeof(char*))
	//	gInfo() << __FUNCTION__ << "Operation = " << g_szPageOperation[nPageOperation];
	if (m_pStackWidget)
	{
		switch (nOperation)
		{
		case Return_MainPage:
			((MainWindow*)m_pMainWindow)->on_pushButton_MainPage_clicked();
			break;
		case Stay_CurrentPage:
			break;
			//case Retry_CurrentPage:
			//{
			//	if (m_nTimerID)
			//	{
			//		killTimer(m_nTimerID);
			//		qDebug() << "KillTimer:" << m_nTimerID;
			//	}
			//	//int nCurIndex = m_pStackWidget->currentIndex();
			//	QStackPage* pCurPage = dynamic_cast<QStackPage*>(m_pStackWidget->currentWidget());
			//	pCurPage->ProcessBussiness();
			//	m_nTimerID = startTimer(1000);
			//	break;
			//}
			//break;
		case Switch_NextPage:
			[[fallthrough]];
		default:
		{
			if (m_nTimerID)
			{
				killTimer(m_nTimerID);
				m_nTimerID = 0;
				qDebug() << "KillTimer:" << m_nTimerID;
			}
			int nCurIndex = m_pStackWidget->currentIndex();
			QStackPage* pCurPage = dynamic_cast<QStackPage*>(m_pStackWidget->currentWidget());
			pCurPage->ShutDown();
			pCurPage->hide();

			int nNewPage = nCurIndex + 1;

			gInfo() << __FUNCTION__ << " nNewPage = " << nNewPage;
			if (nNewPage < m_pStackWidget->count() - 1)
			{
				m_pStackWidget->setCurrentIndex(nNewPage);
				QStackPage* pNewPage = dynamic_cast<QStackPage*>(m_pStackWidget->currentWidget());
				ShowTimeOut(pNewPage->m_nTimeout);
				pNewPage->ActiveTitle();
				pNewPage->show();
				pNewPage->ProcessBussiness();
				qDebug() << __FUNCTION__ << "m_nTimeout=" << m_nTimeout;
				m_nTimeout = pNewPage->m_nTimeout;

				m_nTimerID = startTimer(1000);
				qDebug() << "m_nTimerID:" << m_nTimerID;
			}
			else
			{
				((MainWindow*)m_pMainWindow)->on_pushButton_MainPage_clicked();
			}
			break;
		}
		}
	}
}

void QMainStackPage::on_SwitchPage(int nPage)
{
	if (m_nTimerID)
	{
		killTimer(m_nTimerID);
		m_nTimerID = 0;
		qDebug() << "KillTimer:" << m_nTimerID;
	}
	QStackPage* pCurPage = dynamic_cast<QStackPage*>(m_pStackWidget->currentWidget());
	pCurPage->ShutDown();
	pCurPage->hide();
	int nNewPage = -1;
	QStackPage* pNewPage = nullptr;
	for (int i = 0; i < m_pStackWidget->count(); i++)
	{
		QStackPage* pPage = dynamic_cast<QStackPage*>(m_pStackWidget->currentWidget());
		if (pPage->m_nPageIndex == nPage)
		{
			pNewPage = pPage;
			nNewPage = i;
			break;
		}
	}
	if (pNewPage)
	{
		m_pStackWidget->setCurrentIndex(nNewPage);
		ShowTimeOut(pNewPage->m_nTimeout);
		pNewPage->ActiveTitle();
		pNewPage->show();
		pNewPage->ProcessBussiness();
		qDebug() << __FUNCTION__ << "m_nTimeout=" << m_nTimeout;
		m_nTimeout = pNewPage->m_nTimeout;
		m_nTimerID = startTimer(1000);
		qDebug() << "m_nTimerID:" << m_nTimerID;
	}
	else
	{
		((MainWindow*)m_pMainWindow)->on_pushButton_MainPage_clicked();
	}
}
