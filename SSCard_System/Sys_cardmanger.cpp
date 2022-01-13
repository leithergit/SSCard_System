#pragma execution_character_set("utf-8")
#include "Sys_cardmanger.h"
#include "ui_Sys_cardmanger.h"

enum cardTable_Column
{
	cardTable_No = 0,
	cardTable_Name,
	cardTable_IDCardID,
	cardTable_SSCardID,
	cardTable_Status,
};

CardManger::CardManger(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::CardManger)
{
	ui->setupUi(this);
	int nRowCount = ui->tableWidget->rowCount();
	int nColCount = ui->tableWidget->columnCount();
	for (int nRow = 0; nRow < nRowCount; nRow++)
	{
		for (int nCol = 0; nCol < nColCount; nCol++)
		{
			QTableWidgetItem* pItem = ui->tableWidget->item(nRow, 0);
			pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		}
	}
	QString strDir_path = QCoreApplication::applicationDirPath();
	strDir_path += "/Debug";
	QDirIterator dir_iterator(strDir_path, { "Carddata_*.ini" }, QDir::Files);
	vector<QString> vecFile;
	while (dir_iterator.hasNext())
	{
		dir_iterator.next();
		QFileInfo file_info = dir_iterator.fileInfo();
		QString files = file_info.absoluteFilePath();

		vecFile.push_back(files);
	}
	int nRows = 0;
	for (auto var : vecFile)
	{
		ui->tableWidget->insertRow(nRows);
		ui->tableWidget->setItem(nRows, cardTable_No, new QTableWidgetItem(QString("%1").arg(nRows + 1)));
		QSettings cardata(var, QSettings::IniFormat);

	}

}

CardManger::~CardManger()
{
	delete ui;
}

void CardManger::on_pushButton_clicked()
{

}

bool CardManger::Save(QString& strMessage)
{
	return false;
}
