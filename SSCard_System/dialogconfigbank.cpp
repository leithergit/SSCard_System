#include "dialogconfigbank.h"
#include "Gloabal.h"
#include "ui_dialogconfigbank.h"

DialogConfigBank::DialogConfigBank(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogConfigBank)
{
	ui->setupUi(this);
	for (auto var : g_pDataCenter->GetSysConfigure()->MapBankSupported)
		ui->comboBox->addItem(var.second.c_str(), var.first.c_str());
}

DialogConfigBank::~DialogConfigBank()
{
	delete ui;
}

void DialogConfigBank::on_pushButton_OK_clicked()
{
	g_pDataCenter->GetSysConfigure()->Region.strBankCode = ui->comboBox->currentData().toString().toStdString();
	QString strConfigPath = QCoreApplication::applicationDirPath();
	strConfigPath += "/Configure.ini";
	QSettings ConfigIni(strConfigPath, QSettings::IniFormat);
	ConfigIni.beginGroup("Region");
	ConfigIni.setValue("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode.c_str());
	ConfigIni.endGroup();
	QDialog::accept();
}

void DialogConfigBank::on_pushButton_Cancel_clicked()
{
	QDialog::reject();
}
