#pragma execution_character_set("utf-8")
#include "uc_adforfinance.h"
#include "ui_uc_adforfinance.h"

#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;

uc_AdforFinance::uc_AdforFinance(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
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
