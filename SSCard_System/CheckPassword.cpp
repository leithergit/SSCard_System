#include "CheckPassword.h"

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
	if (strPassword == "12345678")
		QDialog::accept();
	else
		QDialog::reject();
}


void CheckPassword::on_pushButton_Cancel_clicked()
{
	QDialog::reject();
}
