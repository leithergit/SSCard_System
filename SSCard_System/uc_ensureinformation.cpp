#pragma execution_character_set("utf-8")
#include "uc_ensureinformation.h"
#include "ui_uc_ensureinformation.h"

uc_EnsureInformation::uc_EnsureInformation(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::EnsureInformation)
{
	ui->setupUi(this);
}

uc_EnsureInformation::~uc_EnsureInformation()
{
	delete ui;
}

int uc_EnsureInformation::ProcessBussiness()
{
	return 0;
}

void uc_EnsureInformation::OnTimeout()
{

}

void uc_EnsureInformation::on_pushButton_MainPage_clicked()
{
	emit SwitchNextPage();
}
