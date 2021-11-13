#include "OperatorSucceed.h"
#include "ui_OperatorSucceed.h"

OperatorSucceed::OperatorSucceed(QLabel *pTitle,int nTimeout,QWidget *parent) :
    QStackPage(pTitle,nTimeout,parent),
    ui(new Ui::OperatorSucceed)
{
    ui->setupUi(this);
}

OperatorSucceed::~OperatorSucceed()
{
    delete ui;
}

void OperatorSucceed::OnTimeout()
{

}
