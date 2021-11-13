#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "failedwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet(QString::fromUtf8("background-image:url(./Image/backgroud.jpg);"));
    //setStyleSheet(QString::fromUtf8("background-image:url(D:/Work/Henan_shangqiu/HNBXZM/Image/backgroud.jpg);"));
    //D:\Work\Henan_shangqiu\HNBXZM\Image

    m_nDateTimer = startTimer(1000);
    m_pMainpage = new MainPage(this);
    m_pUpdateCard = new UpdateCard(this);
    m_pUpdatePassword = new UpdatePassword(this);
    m_pRegiserLost = new RegisterLost(this);
    m_pFailedWindow = new FailedWindow(nullptr);
    //m_pOperatorFailed = new OperatorFailed();
    ui->stackedWidget->addWidget(m_pMainpage);
    ui->stackedWidget->addWidget(m_pUpdateCard);
    ui->stackedWidget->addWidget(m_pUpdatePassword);
    ui->stackedWidget->addWidget(m_pRegiserLost);
    ui->stackedWidget->setCurrentWidget(m_pMainpage);
    m_pMainpage->show();
    /*
    Constant                Value           Description
    Qt::WindowNoState       0x00000000      The window has no state set (in normal state).
    Qt::WindowMinimized     0x00000001      The window is minimized (i.e. iconified).
    Qt::WindowMaximized     0x00000002      The window is maximized with a frame around it.
    Qt::WindowFullScreen    0x00000004      The window fills the entire screen without any frame around it.
    Qt::WindowActive        0x00000008      The window is the active window, i.e. it has keyboard focus.
    */
    //setWindowFlags((Qt::WindowFlags)(windowFlags()|Qt::WindowStaysOnTopHint));
    //setWindowFlags((Qt::WindowFlags)(windowFlags()));
    connect(m_pUpdateCard,&QMainStackPage::PopupFailedWindow,this,&MainWindow::On_PopupFailedWindow);
    connect(m_pUpdatePassword,&QMainStackPage::PopupFailedWindow,this,&MainWindow::On_PopupFailedWindow);
    connect(m_pRegiserLost,&QMainStackPage::PopupFailedWindow,this,&MainWindow::On_PopupFailedWindow);
    connect(m_pFailedWindow,&FailedWindow::FailedWindowTimeout,this,&MainWindow::On_FailedWindowTimeout);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Updatecard_clicked()
{
    ui->stackedWidget->setCurrentWidget(m_pUpdateCard);
    m_pUpdateCard->ResetAllPages();
    m_pUpdateCard->show();
}

void MainWindow::on_pushButton_ChangePWD_clicked()
{
    ui->stackedWidget->setCurrentWidget(m_pUpdatePassword);
    m_pUpdatePassword->ResetAllPages();
    m_pUpdatePassword->show();
}

void MainWindow::on_pushButton_RegisterLost_clicked()
{
    ui->stackedWidget->setCurrentWidget(m_pRegiserLost);
    m_pRegiserLost->ResetAllPages();
    m_pRegiserLost->show();
}

void MainWindow::on_pushButton_MainPage_clicked()
{
    ui->stackedWidget->setCurrentWidget(m_pMainpage);
    m_pMainpage->show();
}

void MainWindow::On_PopupFailedWindow(QString strText,int nTimeout)
{
    QPoint ptLeftTop = m_pMainpage->mapToGlobal(QPoint(0,0));
    m_pFailedWindow->setGeometry(m_pMainpage->geometry());
    m_pFailedWindow->move(ptLeftTop);
    m_pFailedWindow->Popup(strText,nTimeout);
}
void MainWindow::On_FailedWindowTimeout()
{
    if (m_pFailedWindow)
    {
       m_pFailedWindow->hide();
    }
    on_pushButton_MainPage_clicked();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_nDateTimer)
    {
       QDateTime   tNow = QDateTime::currentDateTime();
       ui->label_DateTime->setText(tNow.toString("yyyy-MM-dd HH:mm:ss"));
    }
}

