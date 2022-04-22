#include "sys_dialogreadidcard.h"
#include "ui_sys_dialogreadidcard.h"
#include "Gloabal.h"
#include <QMovie>

Sys_DialogReadIDCard::Sys_DialogReadIDCard(QString strMessage, bool bBatchMode, QWidget* parent) :
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
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setWindowModality(Qt::WindowModal);
	this->setStyleSheet(QString::fromUtf8("#Sys_DialogReadIDCard{border-image: url(:/Image/Bkgrnd.png);}"));
	if (!bBatchMode)
	{
		QRect Rect(362, 300, 736, 468);
		setGeometry(Rect);
		ui->pushButton_Stop->setText("取消");
	}
	else
	{
		ui->pushButton_Stop->setText("确定");
	}
}

Sys_DialogReadIDCard::~Sys_DialogReadIDCard()
{
	delete ui;
}

void Sys_DialogReadIDCard::on_pushButton_Stop_clicked()
{
	this->reject();
}
