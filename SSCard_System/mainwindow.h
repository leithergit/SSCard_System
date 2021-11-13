#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainpage.h"
#include "updatecard.h"
#include "updatepassword.h"
#include "registerlost.h"
#include "failedwindow.h"
//#include "operatorfailed.h"
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MainPage     *m_pMainpage = nullptr;
    UpdateCard   *m_pUpdateCard = nullptr;
    UpdatePassword *m_pUpdatePassword = nullptr;
    RegisterLost   *m_pRegiserLost = nullptr;
    FailedWindow     *m_pFailedWindow = nullptr;

    void on_pushButton_Updatecard_clicked();

    void on_pushButton_ChangePWD_clicked();

    void on_pushButton_RegisterLost_clicked();

    void on_pushButton_MainPage_clicked();
    int m_nDateTimer = 0;
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void On_FailedWindowTimeout();
    void On_PopupFailedWindow(QString strText,int nTimeout);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
