#include "cardmanger.h"
#include "ui_cardmanger.h"

CardManger::CardManger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CardManger)
{
    ui->setupUi(this);
}

CardManger::~CardManger()
{
    delete ui;
}
