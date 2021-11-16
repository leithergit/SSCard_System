#pragma execution_character_set("utf-8")
#include "uc_makecard.h"
#include "ui_uc_makecard.h"

uc_MakeCard::uc_MakeCard(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::MakeCard)
{
	ui->setupUi(this);
}

uc_MakeCard::~uc_MakeCard()
{
	delete ui;
}


int uc_MakeCard::ProcessBussiness()
{

	return 0;
}

void uc_MakeCard::OnTimeout()
{

}
