#include "regionconfigure.h"
#include "ui_regionconfigure.h"

RegionConfigure::RegionConfigure(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegionConfigure)
{
    ui->setupUi(this);
}

RegionConfigure::~RegionConfigure()
{
    delete ui;
}
