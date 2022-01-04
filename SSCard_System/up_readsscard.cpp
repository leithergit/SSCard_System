#pragma execution_character_set("utf-8")
#include "up_readsscard.h"
#include "ui_up_readsscard.h"
#include <QMovie>


up_ReadSSCard::up_ReadSSCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::ReadSSCard)
{
	ui->setupUi(this);
	QMovie* movie = new QMovie("./Image/InsertSSCard.gif");
	ui->label_Swipecard->setMovie(movie);
	movie->start();
}

up_ReadSSCard::~up_ReadSSCard()
{
	delete ui;
}

int up_ReadSSCard::ProcessBussiness()
{

	return 0;
}

void up_ReadSSCard::OnTimeout()
{

}
