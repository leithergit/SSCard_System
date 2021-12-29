#include "qwidgetreadidcard.h"
#include "ui_qwidgetreadidcard.h"
#include "Payment.h"

QWidgetReadIDCard::QWidgetReadIDCard(IDCardInfo& IDCard, QWidget* parent) :
	QWidget(parent),
	ui(new Ui::QWidgetReadIDCard)
{
	ui->setupUi(this);
	ui->label_Name->setText(QString::fromLocal8Bit((const char*)IDCard.szName));
	ui->label_Gender->setText(QString::fromLocal8Bit((const char*)IDCard.szGender));
	ui->label_Nation->setText(QString::fromLocal8Bit((const char*)IDCard.szNation));
	ui->label_Birthday->setText(QString::fromLocal8Bit((const char*)IDCard.szBirthday));
	ui->label_Identify->setText(QString::fromLocal8Bit((const char*)IDCard.szIdentify));
	ui->label_Address->setText(QString::fromLocal8Bit((const char*)IDCard.szAddress));
	ui->label_IssueAuthority->setText(QString::fromLocal8Bit((const char*)IDCard.szIszssueAuthority));
	ui->label_ExpirationDate->setText(QString::fromLocal8Bit((const char*)IDCard.szExpirationDate1) + "-" + QString::fromLocal8Bit((const char*)IDCard.szExpirationDate2));
	QImage ImagePhoto;
	QString strImage = "./Image/TempPhoto.bmp";
	ImagePhoto.save(strImage);
	QString strQSS = QString("border-image: url(%1);").arg(strImage);
	ui->label_Photo->setStyleSheet(strQSS);

}

QWidgetReadIDCard::~QWidgetReadIDCard()
{
	delete ui;
}

void QWidgetReadIDCard::on_pushButton_OK_clicked()
{
	close();
}
