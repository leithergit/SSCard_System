#include "showlicense.h"
#include "ui_showlicense.h"

ShowLicense::ShowLicense(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowLicense)
{
    ui->setupUi(this);
}

ShowLicense::~ShowLicense()
{
    delete ui;
}

void ShowLicense::on_pushButton_OK_clicked()
{
    close();
}

