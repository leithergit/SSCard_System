﻿#include "nc_commitpersoninfo_Sandong.h"
#include "ui_nc_commitpersoninfo_Sandong.h"
#include <QCoreApplication>
#include <QObject>
#include "Gloabal.h"

nc_commitPersonInfo_Sandong::nc_commitPersonInfo_Sandong(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::nc_commitPersonInfo_Sandong)
{
	ui->setupUi(this);
	struct _occuption
	{
		QString strCode;
		QString strName;
	};
	_occuption occuArray[] = {
							{"1000000","单位负责人"},
							{"2000000","专业、技术人员"},
							{"3000000","办事人员"},
							{"4000000","商业、服务人员"},
							{"5000000","农林牧渔水利生产人员"},
							{"6000000","生产运输工人"},
							{"8000000","其他从业人员"} };
	for (auto& var : occuArray)
		ui->comboBox_Occupation->addItem(var.strName, var.strName);
}

nc_commitPersonInfo_Sandong::~nc_commitPersonInfo_Sandong()
{
	delete ui;
}

void nc_commitPersonInfo_Sandong::on_pushButton_OK_clicked()
{

}