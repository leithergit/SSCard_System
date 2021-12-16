#pragma execution_character_set("utf-8")
#include "rl_ensureinformation.h"
#include "ui_rl_ensureinformation.h"

rl_EnsureInformation::rl_EnsureInformation(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::rl_EnsureInformation)
{
	ui->setupUi(this);
}

rl_EnsureInformation::~rl_EnsureInformation()
{
	delete ui;
}


int rl_EnsureInformation::ProcessBussiness()
{
	return 0;
}

void rl_EnsureInformation::OnTimeout()
{

}


void rl_EnsureInformation::on_pushButton_OK_clicked()
{
	emit SwitchNextPage(Switch_NextPage);
}
