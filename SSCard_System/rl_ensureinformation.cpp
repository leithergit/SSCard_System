#include "rl_ensureinformation.h"
#include "ui_rl_ensureinformation.h"

rl_EnsureInformation::rl_EnsureInformation(QLabel *pTitle,int nTimeout,QWidget *parent) :
    QStackPage(pTitle,nTimeout,parent),
    ui(new Ui::rl_EnsureInformation)
{
    ui->setupUi(this);
}

rl_EnsureInformation::~rl_EnsureInformation()
{
    delete ui;
}


int rl_EnsureInformation::ProcessBussiness()
{

    return 0;
}

void rl_EnsureInformation::OnTimeout()
{

}

void rl_EnsureInformation::on_pushButton_MainPage_clicked()
{
    emit SwitchNextPage();
}

