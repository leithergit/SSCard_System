#include "Sys_otheroptions.h"
#include "ui_Sys_otheroptions.h"

OtherOptions::OtherOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OtherOptions)
{
    ui->setupUi(this);
    int nRowCount = ui->tableWidgetBank->rowCount();
    for (int nItem = 0; nItem < nRowCount; nItem++)
    {
        QTableWidgetItem *pItem = ui->tableWidgetBank->item(nItem,0);
        pItem->setFlags(pItem->flags() &(~Qt::ItemIsEditable));

        pItem = ui->tableWidgetBank->item(nItem,1);
        pItem->setFlags(pItem->flags() &(~Qt::ItemIsEditable));
    }
    nRowCount = ui->tableWidget_Other->rowCount();
    for (int nItem = 0; nItem < nRowCount; nItem++)
    {
        QTableWidgetItem *pItem = ui->tableWidget_Other->item(nItem,0);
        pItem->setFlags(pItem->flags() &(~Qt::ItemIsEditable));

        pItem = ui->tableWidget_Other->item(nItem,1);
        pItem->setFlags(pItem->flags() &(~Qt::ItemIsEditable));
    }
    int nCols = ui->tableWidgetBank->columnCount();
    for (int nIndex = 0; nIndex < nCols; nIndex++)
    {
        ui->tableWidgetBank->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);
    }
    ui->tableWidgetBank->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    nCols = ui->tableWidget_Other->columnCount();
    for (int nIndex = 0; nIndex < nCols; nIndex++)
    {
        ui->tableWidget_Other->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);
    }
    ui->tableWidget_Other->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

OtherOptions::~OtherOptions()
{
    delete ui;
}

void OtherOptions::on_pushButton_TestUploadLog_clicked()
{

}

