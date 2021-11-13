#include "qmainstackpage.h"
#include "mainwindow.h"
#include <QTimer>
#include <QTimerEvent>
#include <QApplication>
#include <QKeyEvent>
#include <qstackpage.h>

QMainStackPage::QMainStackPage(QWidget *parent)
    : QWidget(parent)
{
    foreach (QWidget *w, qApp->topLevelWidgets())
    {
        if ((w = qobject_cast<QMainWindow*>(w)))
            m_pMainWindow = w;
    }
    installEventFilter(this);
}

void QMainStackPage::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_nTimerID)
    {
       OnTimerEvent();
    }
}

void QMainStackPage::OnTimerEvent()
{

}

bool QMainStackPage::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
//    if (event->type() == QEvent::Show)
//    {
//        if (!m_nTimerID)
//            m_nTimerID = startTimer(1000);
//        return true;
//    }
//    else
    if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Return)
        {
            on_SwitchNextPage();
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
            emit PopupFailedWindow("操作失败",5);
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
    ((MainWindow *)m_pMainWindow)->on_pushButton_MainPage_clicked();
}

void QMainStackPage::ResetAllPages()
{
    if (m_nTimerID)
        killTimer(m_nTimerID);
    m_nTimerID = startTimer(1000);
    if (m_pStackWidget)
    {
        int nPageCount = m_pStackWidget->count();
        for (int i = StartPageIndex + 1;i < nPageCount;i ++)
        {
            QStackPage *pCurPage = dynamic_cast<QStackPage *>(m_pStackWidget->widget(i));
            pCurPage->DisActiveTitle();
        }
        m_pStackWidget->setCurrentIndex(StartPageIndex);
        QStackPage *pCurPage = dynamic_cast<QStackPage *>(m_pStackWidget->currentWidget());
        pCurPage->ProcessBussiness();
        m_nTimeout = pCurPage->m_nTimeout;
    }
}


void QMainStackPage::on_SwitchNextPage()
{
    if (m_nTimerID)
        killTimer(m_nTimerID);
    if (m_pStackWidget)
    {   
        int nCurIndex = m_pStackWidget->currentIndex();
        if (nCurIndex < m_pStackWidget->count() - 1)
        {
           m_pStackWidget->setCurrentIndex(++nCurIndex);
           QStackPage *pCurPage = dynamic_cast<QStackPage *>(m_pStackWidget->currentWidget());
           pCurPage->ActiveTitle();
           pCurPage->ProcessBussiness();
           m_nTimeout = pCurPage->m_nTimeout;
           m_nTimerID = startTimer(1000);
        }
        else
        {
            ((MainWindow *)m_pMainWindow)->on_pushButton_MainPage_clicked();
        }
    }
}
