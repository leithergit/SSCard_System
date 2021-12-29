#pragma execution_character_set("utf-8")
#include "pageconfigure.h"
#include "ui_pageconfigure.h"
#include <QCheckBox>
#include <QStyleFactory>
#include <QBrush>
#include <QColor>

PageConfigure::PageConfigure(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::PageConfigure)
{
	ui->setupUi(this);
	int nCols = ui->tableWidget->columnCount();
	for (int nIndex = 0; nIndex < nCols; nIndex++)
    {
		ui->tableWidget->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);
    }
	ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	nCols = ui->tableWidget_2->columnCount();
	for (int nIndex = 0; nIndex < nCols; nIndex++)
		ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);
	ui->tableWidget_2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	int nRows = ui->tableWidget->rowCount();
	m_pCheckBoxGroup = new QButtonGroup;
	m_pCheckBoxGroup->setExclusive(false);
	for (int nItem = 0; nItem < nRows; nItem++)
	{
        QTableWidgetItem *pItem = ui->tableWidget->item(nItem,0);
        pItem->setFlags(pItem->flags() &(~Qt::ItemIsEditable));
        //pItem->setBackground(QBrush(Qt::lightGray));

        pItem = ui->tableWidget->item(nItem,1);
        pItem->setFlags(pItem->flags() &(~Qt::ItemIsEditable));
        pItem->setBackground(QBrush(Qt::lightGray));

		QWidget* pCellWidget = new QWidget(ui->tableWidget);
		QHBoxLayout* pHLayout = new QHBoxLayout();
		pHLayout->setMargin(0);
		pHLayout->setSpacing(0);

		QCheckBox* pCheckBox = new QCheckBox();
		pCheckBox->setChecked(false);
		pCheckBox->setFont(ui->tableWidget->font());
		pCheckBox->setFocusPolicy(Qt::NoFocus);
		pCheckBox->setStyle(QStyleFactory::create("fusion"));
		pCheckBox->setStyleSheet(QString(".QCheckBox {margin:3px;border:0px;}QCheckBox::indicator {width: %1px; height: %1px; }").arg(20));

		pHLayout->addWidget(pCheckBox);
		pHLayout->setAlignment(pCheckBox, Qt::AlignCenter);
		pCellWidget->setLayout(pHLayout);
		m_pCheckBoxGroup->addButton(pCheckBox, nItem);
		ui->tableWidget->setCellWidget(nItem, 3, pCellWidget);
		connect(m_pCheckBoxGroup, &QButtonGroup::idClicked, this, &PageConfigure::on_TableCheckBoxStateChanged);
	}
}

PageConfigure::~PageConfigure()
{
	delete ui;
}

bool PageConfigure::Save(QString& strMessage)
{
	return false;
}

void PageConfigure::on_TableCheckBoxStateChanged(int nIndex)
{
	QCheckBox* pCheckBox = (QCheckBox*)m_pCheckBoxGroup->button(nIndex);
	bool bStatus = false;
	if (pCheckBox->checkState() == Qt::Checked)
	{
		bStatus = true;
	}
	auto [it, Inserted] = m_mapPageStatus.try_emplace(nIndex, bStatus);
	qDebug() << "Item[" << nIndex << "]" << bStatus;
}
