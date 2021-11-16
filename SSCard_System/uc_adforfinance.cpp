#pragma execution_character_set("utf-8")
#include "uc_adforfinance.h"
#include "ui_uc_adforfinance.h"

uc_AdforFinance::uc_AdforFinance(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::AdforFinance)
{
	ui->setupUi(this);
}

uc_AdforFinance::~uc_AdforFinance()
{
	delete ui;
}


void uc_AdforFinance::OnTimeout()
{

}
