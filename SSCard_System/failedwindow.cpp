#include "failedwindow.h"
#include "ui_failedwindow.h"
#include <qdesktopwidget.h>
#include <QScreen>
#include<QDebug>

FailedWindow::FailedWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FailedWindow)
{
    ui->setupUi(this);

    hide();
}

FailedWindow::~FailedWindow()
{
    delete ui;
}

void FailedWindow::Popup(QString strTitle,int nTimeout)
{
    qDebug()<<__FUNCTION__;
    setWindowOpacity(0.8);
    show();
    setFocus();
//   以下代码为下设置全屏遮罩，暂弃用
//    QPoint pos = QWidget::mapToGlobal(this->pos()); //获取窗口在桌面上的绝对位置
//    qDebug("Pos = (%d,%d)",pos.x(),pos.y());
//    QScreen *pScreen  = QGuiApplication::screenAt(pos);
//    if (!pScreen)
//    {
//        qDebug()<<"Get Screen Failed.";
//    }
//    else
//    {
//        qDebug()<<"Get Screen Succeed.";
//    }
//    this->setGeometry(pScreen->geometry());
    setWindowFlags((Qt::WindowFlags)(windowFlags()|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::WindowActive));
    m_nTimerID = startTimer(1000);
    m_nTimeout = nTimeout;
    ui->label_FailedText->setText(strTitle);

}

void FailedWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_nTimerID )
    {
        m_nTimeout --;
        if (m_nTimeout <= 0)
        {
            killTimer(m_nTimerID);
            m_nTimerID = 0;
            emit FailedWindowTimeout();
        }
    }
}
