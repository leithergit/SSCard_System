#include "Sys_regionconfigure.h"
#include "ui_Sys_regionconfigure.h"

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
	int nRows = ui->tableWidget->rowCount();
	for (int nItem = 0; nItem < nRows; nItem++)
	{
		QTableWidgetItem* pItem = ui->tableWidget->item(nItem, 0);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		//pItem->setBackground(QBrush(Qt::lightGray));

		pItem = ui->tableWidget->item(nItem, 1);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		pItem->setBackground(QBrush(Qt::lightGray));
	}
}

RegionConfigure::~RegionConfigure()
{
	delete ui;
}

bool RegionConfigure::Save(QString& strMessage)
{
	RegionInfo& regInfo = g_pDataCenter->GetSysConfigure()->Region;
	int nRow = 0;
    string strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();

    if (strTemp.size() != 6)
	{
		strMessage = "城市代码必须是6位数字!";
		return false;
    }
    regInfo.strCityCode = strTemp;

    strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
    if (strTemp.size() != 8)
	{
		strMessage = "区县代码必须是8位数字!";
		return false;
	}
    regInfo.strCountry = strTemp;
    strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
    if (strTemp.size() !=8)
    {
        strMessage = "机构代码必须是8位数字!";
        return false;
    }
    regInfo.strAgency = strTemp;
    strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
    if (strTemp.size() < 0)
    {
        strMessage = "卡商代码不能为空!";
        return false;
    }
    regInfo.strCardVendor = strTemp;

    strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
    if (strTemp.size() < 5)
    {
        strMessage = "银行代码至少为5位数字!";
        return false;
    }
    regInfo.strBankCode = strTemp;

	if (g_pDataCenter->GetManagerLevel() == Manager_Level::SuperVisor)
	{
        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() != 6)
        {
            strMessage = "默认密码必须为6位以内的数字!";
            return false;
        }
        regInfo.strSSCardDefaulutPin = strTemp;

        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() <1 )
        {
            strMessage = "主控密码不能为空!";
            return false;
        }
        regInfo.strPrimaryKey = strTemp;

        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() <1 )
        {
            strMessage = "加密机地址不能为空!";
            return false;
        }

        regInfo.strEMURL  = strTemp;
        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() <1 )
        {
            strMessage = "加密机帐号不能为空!";
            return false;
        }
        regInfo.strEMAccount = strTemp;
        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() <1 )
        {
            strMessage = "加密机密码不能为空!";
            return false;
        }
        regInfo.strEMPassword = strTemp;

        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() <1 )
        {
            strMessage = "卡管地址不能为空!";
            return false;
        }
        regInfo.strCMURL = strTemp;
        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() <1 )
        {
            strMessage = "卡管帐号不能为空!";
            return false;
        }
        regInfo.strCMAccount = strTemp;
        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() <1 )
        {
            strMessage = "卡管密码不能为空!";
            return false;
        }
        regInfo.strCMPassword = strTemp;

        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() <1 )
        {
            strMessage = "卡管CA帐号不能为空!";
            return false;
        }
        regInfo.strCM_CA_Account = strTemp;
        strTemp = ui->tableWidget->item(nRow++, 2)->text().toStdString();
        if (strTemp.size() <1 )
        {
            strMessage = "卡管CA密码不能为空!";
            return false;
        }
        regInfo.strCM_CA_Password = strTemp;
	}
    return true;
}
