#pragma execution_character_set("utf-8")
#include "Sys_pageconfigure.h"
#include "ui_Sys_pageconfigure.h"


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

	int nRows = ui->tableWidget->rowCount();
	m_pCheckBoxGroup = new QButtonGroup;
	m_pCheckBoxGroup->setExclusive(false);
	connect(m_pCheckBoxGroup, &QButtonGroup::idClicked, this, &PageConfigure::on_TableCheckBoxStateChanged);
	for (int nItem = 0; nItem < nRows; nItem++)
	{
		QTableWidgetItem* pItem = ui->tableWidget->item(nItem, 0);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		//pItem->setBackground(QBrush(Qt::lightGray));

//        pItem = ui->tableWidget->item(nItem,1);
//        pItem->setFlags(pItem->flags() &(~Qt::ItemIsEditable));
//        pItem->setBackground(QBrush(Qt::lightGray));

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
		pCheckBox->setEnabled(false);
		m_pCheckBoxGroup->addButton(pCheckBox, nItem);
		ui->tableWidget->setCellWidget(nItem, 3, pCellWidget);
	}

	//m_pCheckBoxGroup->button(0)->setEnabled(false); // 读取身份证
	m_pCheckBoxGroup->button(1)->setEnabled(true); // 人脸识别
	//m_pCheckBoxGroup->button(2)->setEnabled(false); // 信息确认
	m_pCheckBoxGroup->button(3)->setEnabled(true); // 输入手机号码
	//m_pCheckBoxGroup->button(4)->setEnabled(false); // 支付
	//m_pCheckBoxGroup->button(5)->setEnabled(false); // 制卡
	//m_pCheckBoxGroup->button(6)->setEnabled(false); // 输入社保卡密码
	//m_pCheckBoxGroup->button(7)->setEnabled(false); // 修改社保卡密码
	//m_pCheckBoxGroup->button(8)->setEnabled(false); // 挂失/解挂
	m_pCheckBoxGroup->button(9)->setEnabled(true); // 开通金融功能广告

	nRows = ui->tableWidget->rowCount();
	for (int nItem = 0; nItem < nRows; nItem++)
	{
		QTableWidgetItem* pItem = ui->tableWidget->item(nItem, 0);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		//pItem->setBackground(QBrush(Qt::lightGray));

		pItem = ui->tableWidget->item(nItem, 1);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		pItem->setBackground(QBrush(Qt::lightGray));
	}
	nCols = ui->tableWidget_2->columnCount();
	for (int nIndex = 0; nIndex < nCols; nIndex++)
		ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);
	ui->tableWidget_2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	nRows = ui->tableWidget_2->rowCount();
	for (int nItem = 0; nItem < nRows; nItem++)
	{
		QTableWidgetItem* pItem = ui->tableWidget_2->item(nItem, 0);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		//pItem->setBackground(QBrush(Qt::lightGray));

		pItem = ui->tableWidget_2->item(nItem, 1);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		pItem->setBackground(QBrush(Qt::lightGray));
	}
}

PageConfigure::~PageConfigure()
{
	delete ui;
}

bool PageConfigure::Save(QString& strMessage)
{
	QString strPageName[] = { "读取身份证"
							 "读取社保卡",
							 "信息确认",
							 "输入手机号码",
							 "支付页面",
							 "制卡页面",
							 "读取社保卡",
							 "输入社保卡密码",
							 "修改社保卡密码",
							 "挂失/解挂",
							 "开通金融页面"
	};
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
	m_mapPageStatus.insert(pair<int, bool>(nIndex, bStatus));
	//auto [it, Inserted] = m_mapPageStatus.try_emplace(nIndex, bStatus);
	qDebug() << "Item[" << nIndex << "]" << bStatus;
}
