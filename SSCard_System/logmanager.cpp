#pragma execution_character_set("utf-8")
#include "logmanager.h"
#include "ui_logmanager.h"

logManager::logManager(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::logManager)
{
	ui->setupUi(this);
}

logManager::~logManager()
{
	delete ui;
}

void logManager::on_pushButton_clicked()
{

}

void logManager::on_pushButton_2_clicked()
{

}
