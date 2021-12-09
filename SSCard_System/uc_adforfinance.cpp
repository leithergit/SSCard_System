#pragma execution_character_set("utf-8")
#include "uc_adforfinance.h"
#include "ui_uc_adforfinance.h"

#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;

uc_AdforFinance::uc_AdforFinance(QLabel* pTitle, QString strStepImage, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nTimeout, parent),
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
