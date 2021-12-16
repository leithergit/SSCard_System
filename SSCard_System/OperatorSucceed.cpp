#pragma execution_character_set("utf-8")
#include "OperatorSucceed.h"
#include "ui_OperatorSucceed.h"

OperatorSucceed::OperatorSucceed(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
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
