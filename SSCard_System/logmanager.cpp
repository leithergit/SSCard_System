#include "logmanager.h"
#include "ui_logmanager.h"

logManager::logManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::logManager)
{
    ui->setupUi(this);
}

logManager::~logManager()
{
    delete ui;
}
