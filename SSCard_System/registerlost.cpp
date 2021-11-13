#include "registerlost.h"
#include "ui_registerlost.h"
#include "mainwindow.h"
#include "uc_readidcard.h"
#include "rl_ensureinformation.h"
#include "OperatorSucceed.h"
#include <array>
using namespace std;
#define _Stack_Count     (_EnsureRegisterlog + 1)

RegisterLost::RegisterLost(QWidget *parent) :
    QMainStackPage(parent),
    ui(new Ui::RegisterLost)
{
    ui->setupUi(this);
    try
    {
        m_pStackWidget = ui->stackedWidget;
        //ui->stackedWidget->addWidget(new FailedWindow(nullptr));
        ui->stackedWidget->addWidget(new uc_ReadIDCard(ui->label_ReadCard));
        ui->stackedWidget->addWidget(new rl_EnsureInformation(ui->label_EnsureInformation));
        ui->stackedWidget->addWidget(new OperatorSucceed(nullptr));
        for (int i = 0;i < ui->stackedWidget->count();i ++)
        {
            QStackPage *pPage = dynamic_cast<QStackPage *>(ui->stackedWidget->widget(i));
            connect(pPage,&QStackPage::SwitchNextPage,this,&QMainStackPage::on_SwitchNextPage);
        }
    }
    catch (exception &e)
    {
        qDebug()<<"Catch a exception:"<<e.what();
    }
}

RegisterLost::~RegisterLost()
{
    delete ui;
}

void RegisterLost::OnTimerEvent()
{
    ui->label_CountDown->setText(QString("%1").arg(--m_nTimeout));
    if (!m_nTimeout )
    {
        killTimer(m_nTimerID);
        m_nTimerID = 0;
        ((QStackPage *)ui->stackedWidget->currentWidget())->OnTimeout();
        ((MainWindow *)m_pMainWindow)->on_pushButton_MainPage_clicked();
    }
}
