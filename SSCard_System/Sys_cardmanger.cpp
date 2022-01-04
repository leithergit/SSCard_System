#pragma execution_character_set("utf-8")
#include "Sys_cardmanger.h"
#include "ui_Sys_cardmanger.h"

CardManger::CardManger(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::CardManger)
{
	ui->setupUi(this);
    int nRowCount = ui->tableWidget->rowCount();
    int nColCount = ui->tableWidget->columnCount();
    for (int nRow = 0; nRow < nRowCount; nRow++)
    {
        for (int nCol = 0;nCol < nColCount;nCol ++ )
        {
            QTableWidgetItem *pItem = ui->tableWidget->item(nRow,0);
            pItem->setFlags(pItem->flags() &(~Qt::ItemIsEditable));
        }
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
