#include "pageconfigure.h"
#include "ui_pageconfigure.h"

PageConfigure::PageConfigure(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageConfigure)
{
    ui->setupUi(this);
}

PageConfigure::~PageConfigure()
{
    delete ui;
}
