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
	//    if (event->type() == QEvent::Show)
	//    {
	//        if (!m_nTimerID)
	//            m_nTimerID = startTimer(1000);
	//        return true;
	//    }
	//    else
	if (event->type() == QEvent::KeyRelease)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		if (keyEvent->key() == Qt::Key_Return)
		{
			on_SwitchNextPage(Switch_NextPage);
			return true;
		}
		else if (keyEvent->key() == Qt::Key_Backspace)
		{
			on_pushButton_MainPage_clicked();
			return true;
		}
		else if (keyEvent->key() == Qt::Key_Escape)
		{
			killTimer(m_nTimerID);
			QString strMessage = "按下ESC键模拟因错误返回主页!";
			emit ShowMaskWidget(strMessage, "", Failed, Stay_CurrentPage);

			return true;
		}
	}
	return false;
}

void QMainStackPage::on_pushButton_MainPage_clicked()
{
	if (m_nTimerID)
	{
		killTimer(m_nTimerID);
		m_nTimeout = 30;
	}
	((MainWindow*)m_pMainWindow)->on_pushButton_MainPage_clicked();
}

void QMainStackPage::ResetAllPages()
{
	if (m_nTimerID)
		killTimer(m_nTimerID);
	m_nTimerID = startTimer(1000);
	if (m_pStackWidget)
	{
		int nPageCount = m_pStackWidget->count();
		for (int i = StartPageIndex + 1; i < nPageCount; i++)
		{
			QStackPage* pCurPage = dynamic_cast<QStackPage*>(m_pStackWidget->widget(i));
			pCurPage->DisActiveTitle();
		}
		m_pStackWidget->setCurrentIndex(StartPageIndex);

		QStackPage* pCurPage = dynamic_cast<QStackPage*>(m_pStackWidget->currentWidget());
		pCurPage->ActiveTitle();
		pCurPage->ProcessBussiness();
		m_nTimeout = pCurPage->m_nTimeout;
	}
}

void QMainStackPage::on_SwitchNextPage(int nPageOperation)
{
	if (nPageOperation < sizeof(g_szPageOperation) / sizeof(char*))
		gInfo() << __FUNCTION__ << "Operation = " << g_szPageOperation[nPageOperation];
	if (m_pStackWidget)
	{
		switch (nPageOperation)
		{
		case Return_MainPage:
			((MainWindow*)m_pMainWindow)->on_pushButton_MainPage_clicked();
			break;
		case Stay_CurrentPage:
			break;
		case Switch_NextPage:
			[[fallthrough]];
		case Skip_NextPage:
			[[fallthrough]];
		default:
		{
			if (m_nTimerID)
				killTimer(m_nTimerID);
			int nCurIndex = m_pStackWidget->currentIndex();
			QStackPage* pCurPage = dynamic_cast<QStackPage*>(m_pStackWidget->currentWidget());
			pCurPage->ShutDown();

			int nNewPage = nCurIndex + nPageOperation - Switch_NextPage + 1;
			gInfo() << __FUNCTION__ << " nNewPage = " << nNewPage;
			if (nNewPage < m_pStackWidget->count() - 1)
			{
				m_pStackWidget->setCurrentIndex(nNewPage);
				QStackPage* pNewPage = dynamic_cast<QStackPage*>(m_pStackWidget->currentWidget());
				pNewPage->ActiveTitle();
				pNewPage->ProcessBussiness();
				m_nTimeout = pNewPage->m_nTimeout;
				m_nTimerID = startTimer(1000);
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
