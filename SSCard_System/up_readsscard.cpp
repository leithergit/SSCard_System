#pragma execution_character_set("utf-8")
#include "up_readsscard.h"
#include "ui_up_readsscard.h"

up_ReadSSCard::up_ReadSSCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::ReadSSCard)
{
	ui->setupUi(this);
}

up_ReadSSCard::~up_ReadSSCard()
{
	delete ui;
}

int up_ReadSSCard::ProcessBussiness()
{
	return 0;
}

void up_ReadSSCard::OnTimeout()
{

}
