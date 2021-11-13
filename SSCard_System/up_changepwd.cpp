#include "up_changepwd.h"
#include "ui_up_changepwd.h"

up_ChangePWD::up_ChangePWD(QLabel *pTitle,int nTimeout,QWidget *parent) :
    QStackPage(pTitle,nTimeout,parent),
    ui(new Ui::up_ChangePWD)
{
    ui->setupUi(this);
}

up_ChangePWD::~up_ChangePWD()
{
    delete ui;
}

int up_ChangePWD::ProcessBussiness()
{
    return 0;
}

void up_ChangePWD::OnTimeout()
{

}

void up_ChangePWD::on_pushButton_ChangePassword_clicked()
{
    emit SwitchNextPage();
}

