#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
#include "./sdk/idcard_api.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")
//#pragma comment(lib,"./sdk/IDCard_API.lib")

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    ui->pushButton_Start->setEnabled(true);
    ui->comboBox_Port->setEnabled(true);
}

Dialog::~Dialog()
{
    delete ui;
}
#define    Error_Not_IDCARD         (-1)


void Dialog::ReadIDCard()
{

}

void Dialog::timerEvent(QTimerEvent *event)
{
    int nResult = OpenReader(nullptr);

    if (bSucceed && nDelayCount <= 3)
    {
        nDelayCount ++;
        return ;
    }
    else
    {
        nDelayCount = 0;
        bSucceed = false;
    }
    IDCardInfo IDCard;
    nResult = OpenReader(nullptr);;
    if (nResult != IDCard_Status::IDCard_Succeed)
    {
        char szText[256] = {0};
        GetErrorMessage((IDCard_Status)nResult,szText,sizeof(szText));
        ui->label_Information->setStyleSheet("QLabel{color: rgb(255, 0, 0);}");
        ui->label_Information->setText(QString(szText));
        return ;
    }
    nResult = FindIDCard();
    if (nResult != IDCard_Status::IDCard_Succeed)
    {
        char szText[256] = {0};
        GetErrorMessage((IDCard_Status)nResult,szText,sizeof(szText));
        ui->label_Information->setStyleSheet("QLabel{color: rgb(255, 0, 0);}");
        ui->label_Information->setText(QString(szText));
        return ;
    }

    nResult = ReadIDCard(IDCard);

    if (nResult != IDCard_Status::IDCard_Succeed)
    {
        char szText[256] = {0};
        GetErrorMessage((IDCard_Status)nResult,szText,sizeof(szText));
        ui->label_Information->setStyleSheet("QLabel{color: rgb(255, 0, 0);}");
        ui->label_Information->setText(QString(szText));
        return ;
    }
    else
    {
        ui->label_Name->setText(QString::fromLocal8Bit((const char *)IDCard.szName));
        ui->label_Gender->setText(QString::fromLocal8Bit((const char *)IDCard.szGender));
        ui->label_Nation->setText(QString::fromLocal8Bit((const char *)IDCard.szNation));
        ui->label_Birthday->setText(QString::fromLocal8Bit((const char *)IDCard.szBirthday));
        ui->label_Identify->setText(QString::fromLocal8Bit((const char *)IDCard.szIdentify));
        ui->label_Address->setText(QString::fromLocal8Bit((const char *)IDCard.szAddress));
        ui->label_IssueAuthority->setText(QString::fromLocal8Bit((const char *)IDCard.szIszssueAuthority));
        ui->label_ExpirationDate->setText(QString::fromLocal8Bit((const char *)IDCard.szExpirationDate1) + "-" + QString::fromLocal8Bit((const char *)IDCard.szExpirationDate2));
        QImage ImagePhoto = QImage::fromData(IDCard.szPhoto,IDCard.nPhotoSize);
        ImagePhoto.save("./Identify.bmp");
        ui->label_Photo->setPixmap(QPixmap::fromImage(QImage::fromData(IDCard.szPhoto,IDCard.nPhotoSize)));
    }
    bSucceed = true;
    ui->label_Information->setStyleSheet("QLabel{color: rgb(0, 255, 0);}");
    ui->label_Information->setText(tr("读卡成功"));
    CloseReader();
}

void Dialog::on_pushButton_Start_clicked()
{
    m_nTimerID = startTimer(1000,Qt::PreciseTimer);
    ui->pushButton_Start->setEnabled(false);
    ui->pushButton_Stop->setEnabled(true);
}


void Dialog::on_pushButton_Stop_clicked()
{
    killTimer(m_nTimerID);
    m_nTimerID = -1;
    ui->pushButton_Start->setEnabled(true);
    ui->pushButton_Stop->setEnabled(false);
}


void Dialog::on_pushButton_Start_2_clicked()
{

}

