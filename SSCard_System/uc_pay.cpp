#include "uc_pay.h"
#include "ui_uc_pay.h"

uc_Pay::uc_Pay(QLabel *pTitle,int nTimeout,QWidget *parent) :
    QStackPage(pTitle,nTimeout,parent),
    ui(new Ui::Pay)
{
    ui->setupUi(this);
}

uc_Pay::~uc_Pay()
{
    delete ui;
}

int uc_Pay::ProcessBussiness()
{
    return 0;
}

void uc_Pay::OnTimeout()
{

}
