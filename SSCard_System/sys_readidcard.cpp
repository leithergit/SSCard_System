#include "sys_readidcard.h"
#include "ui_sys_readidcard.h"

Sys_ReadIDCard::Sys_ReadIDCard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Sys_ReadIDCard)
{
    ui->setupUi(this);
}

Sys_ReadIDCard::~Sys_ReadIDCard()
{
    delete ui;
}
