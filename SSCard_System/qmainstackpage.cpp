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
            QString strMessage = u8"按下ESC键模拟因错误返回主页!";
            emit ShowMaskWidget(strMessage,Failed,Stay_CurrentPage);

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
		pCurPage->ProcessBussiness();
		m_nTimeout = pCurPage->m_nTimeout;
	}
}

void QMainStackPage::on_SwitchNextPage(int nOperation)
{
	if (m_pStackWidget)
	{
        switch (nOperation)
        {
        case Stay_CurrentPage:
            break;
        case Switch_NextPage:
        case Skip_NextPage:
        default:
        {
            if (m_nTimerID)
                killTimer(m_nTimerID);
            int nCurIndex = m_pStackWidget->currentIndex();
            QStackPage* pCurPage = dynamic_cast<QStackPage*>(m_pStackWidget->currentWidget());
            pCurPage->ShutDown();

            int nNewPage = nCurIndex + nOperation - Stay_CurrentPage;
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
