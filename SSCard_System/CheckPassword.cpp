#pragma execution_character_set("utf-8")
#include "CheckPassword.h"
#include "Gloabal.h"
#include <DevBase.h>

CheckPassword::CheckPassword(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags((Qt::WindowFlags)(windowFlags() | Qt::WindowStaysOnTopHint ));
}

CheckPassword::~CheckPassword()
{

}

void CheckPassword::on_pushButton_OK_clicked()
{
	strPassword = ui.lineEdit_Password->text();
#ifdef _DEBUG
	if (strPassword == "1")
	{
		g_pDataCenter->SetMannagerLevel(Manager_Level::Manager);
		QDialog::accept();
	}
	else if (strPassword == "2")
	{
		g_pDataCenter->SetMannagerLevel(Manager_Level::SuperVisor);
		QDialog::accept();
	}
	else
		QDialog::reject();
#else
	if (strPassword == "#edcVFR$")
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
#endif
}


void CheckPassword::on_pushButton_Cancel_clicked()
{
	QDialog::reject();
}
