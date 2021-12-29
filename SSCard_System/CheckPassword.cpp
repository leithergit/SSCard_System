#pragma execution_character_set("utf-8")
#include "CheckPassword.h"
#include "Gloabal.h"

CheckPassword::CheckPassword(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

CheckPassword::~CheckPassword()
{

}

void CheckPassword::on_pushButton_OK_clicked()
{
	strPassword = ui.lineEdit_Password->text();
	if (strPassword == "3edcBGT%")
	{
		g_pDataCenter->SetMannagerLevel(Manager_Level::Manager);
		QDialog::accept();
	}
	else if (strPassword == "KingaoTech@2021")
	{
		g_pDataCenter->SetMannagerLevel(Manager_Level::SuperVisor);
		QDialog::accept();
	}
	else
		QDialog::reject();
}


void CheckPassword::on_pushButton_Cancel_clicked()
{
	QDialog::reject();
}
