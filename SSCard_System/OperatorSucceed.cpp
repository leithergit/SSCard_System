#pragma execution_character_set("utf-8")
#include "OperatorSucceed.h"
#include "ui_OperatorSucceed.h"

OperatorSucceed::OperatorSucceed(QLabel* pTitle,QString strStepImage, int nTimeout, QWidget* parent) :
    QStackPage(pTitle,strStepImage,nTimeout, parent),
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
