#include "sys_dialogreadidcard.h"
#include "ui_sys_dialogreadidcard.h"
#include <QMovie>

Sys_DialogReadIDCard::Sys_DialogReadIDCard(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Sys_DialogReadIDCard)
{
	ui->setupUi(this);
	QMovie* movie = new QMovie("./Image/SwipeIDCard.gif");
	ui->label_gif->setMovie(movie);
	QSize sz = ui->label_gif->size();
	movie->setScaledSize(sz);
	movie->start();
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}

Sys_DialogReadIDCard::~Sys_DialogReadIDCard()
{
	delete ui;
}

void Sys_DialogReadIDCard::on_pushButton_Stop_clicked()
{
	this->accept();
}
