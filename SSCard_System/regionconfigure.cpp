#include "regionconfigure.h"
#include "ui_regionconfigure.h"

RegionConfigure::RegionConfigure(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::RegionConfigure)
{
	ui->setupUi(this);

	RegionInfo& regInfo = g_pDataCenter->GetSysConfigure()->Region;
	int nRow = 0;
	ui->tableWidget->item(nRow++, 2)->setText(regInfo.strCityCode.c_str());
	ui->tableWidget->item(nRow++, 2)->setText(regInfo.strCountry.c_str());
	ui->tableWidget->item(nRow++, 2)->setText(regInfo.strAgency.c_str());
	ui->tableWidget->item(nRow++, 2)->setText(regInfo.strCardVendor.c_str());
	ui->tableWidget->item(nRow++, 2)->setText(regInfo.strBankCode.c_str());
	if (g_pDataCenter->GetManagerLevel() != Manager_Level::SuperVisor)
	{
		int nRows = ui->tableWidget->rowCount();
		for (int r = nRows - 1; r >= 5; r--)
		{
			ui->tableWidget->removeRow(r);
		}
	}
	else
	{
		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strSSCardDefaulutPin.c_str()));
		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strPrimaryKey.c_str()));

		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strEMURL.c_str()));
		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strEMAccount.c_str()));
		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strEMPassword.c_str()));

		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strCMURL.c_str()));
		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strCMAccount.c_str()));
		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strCMPassword.c_str()));

		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strCM_CA_Account.c_str()));
		ui->tableWidget->setItem(nRow++, 2, new QTableWidgetItem(regInfo.strCM_CA_Password.c_str()));
		ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	}
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	int nCols = ui->tableWidget->columnCount();
	for (int nIndex = 0; nIndex < nCols - 1; nIndex++)
		ui->tableWidget->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);
}

RegionConfigure::~RegionConfigure()
{
	delete ui;
}

bool RegionConfigure::Save(QString& strMessage)
{
	return false;
}
