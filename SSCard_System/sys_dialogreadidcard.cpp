#include "sys_dialogreadidcard.h"
#include "ui_sys_dialogreadidcard.h"
#include <QMovie>

Sys_DialogReadIDCard::Sys_DialogReadIDCard(QString strMessage, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Sys_DialogReadIDCard)
{
	ui->setupUi(this);
	QMovie* movie = new QMovie("./Image/SwipeIDCard.gif");
	ui->label_gif->setMovie(movie);
	QSize sz = ui->label_gif->size();
	movie->setScaledSize(sz);
	movie->start();
	ui->label_Notify->setText(strMessage);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	this->setStyleSheet(QString::fromUtf8("#Sys_DialogReadIDCard{border-image: url(:/Image/Bkgrnd.png);}"));
	QRect Rect(362, 300, 736, 468);
	setGeometry(Rect);
}

Sys_DialogReadIDCard::~Sys_DialogReadIDCard()
{
	delete ui;
}

void Sys_DialogReadIDCard::on_pushButton_Stop_clicked()
{
	this->reject();
}
