#include "qnationwidget.h"
#include "ui_qnationwidget.h"
#include "uc_readidcard.h"

QNationWidget::QNationWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::QNationWidget)
{
	ui->setupUi(this);
	connect(ui->pushButton_OK, &QPushButton::clicked, this, &QNationWidget::on_pushButton_Ok_clicked);
}

QNationWidget::~QNationWidget()
{
	delete ui;
}

void QNationWidget::on_pushButton_Ok_clicked()
{
	this->hide();
	((uc_ReadIDCard*)parent())->emit ShowNationWidget(false);
}
