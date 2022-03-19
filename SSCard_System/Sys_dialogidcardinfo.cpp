#include "Sys_dialogidcardinfo.h"
#include "ui_Sys_dialogidcardinfo.h"

#include "Payment.h"

DialogIDCardInfo::DialogIDCardInfo(IDCardInfo& IDCard, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogIDCardInfo)
{
	ui->setupUi(this);
	ui->label_Name->setText(QString::fromLocal8Bit((const char*)IDCard.szName));
	ui->label_Gender->setText(QString::fromLocal8Bit((const char*)IDCard.szGender));
	ui->label_Nation->setText(QString::fromLocal8Bit((const char*)IDCard.szNationalty));
	ui->label_Birthday->setText(QString::fromLocal8Bit((const char*)IDCard.szBirthday));
	ui->label_Identify->setText(QString::fromLocal8Bit((const char*)IDCard.szIdentity));
	ui->label_Address->setText(QString::fromLocal8Bit((const char*)IDCard.szAddress));
	ui->label_IssueAuthority->setText(QString::fromLocal8Bit((const char*)IDCard.szIszssueAuthority));
	ui->label_ExpirationDate->setText(QString::fromLocal8Bit((const char*)IDCard.szExpirationDate1) + "-" + QString::fromLocal8Bit((const char*)IDCard.szExpirationDate2));
	QString strImage = "./Image/TempPhoto.bmp";
	QImage ImagePhoto = QImage::fromData(IDCard.szPhoto, IDCard.nPhotoSize);
	ImagePhoto.save(strImage);
	QString strQSS = QString("border-image: url(%1);").arg(strImage);
	ui->label_Photo->setStyleSheet(strQSS);
}

DialogIDCardInfo::~DialogIDCardInfo()
{
	delete ui;
}

void DialogIDCardInfo::on_pushButton_OK_clicked()
{
	QDialog::accept();
}
