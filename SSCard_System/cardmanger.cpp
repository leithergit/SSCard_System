#pragma execution_character_set("utf-8")
#include "cardmanger.h"
#include "ui_cardmanger.h"

CardManger::CardManger(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::CardManger)
{
	ui->setupUi(this);
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
