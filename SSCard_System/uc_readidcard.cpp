#include "uc_readidcard.h"
#include "ui_uc_readidcard.h"
#include "Gloabal.h"
#include <chrono>
using namespace std;
using namespace chrono;

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")
//#pragma comment(lib,"SDK/IDCard/IDCard_API")

uc_ReadIDCard::uc_ReadIDCard(QLabel *pTitle,int nTimeout,QWidget *parent) :
    QStackPage(pTitle,nTimeout,parent),
    ui(new Ui::ReadIDCard)
{
    info()<<"Test";
    ui->setupUi(this);

}

uc_ReadIDCard::~uc_ReadIDCard()
{
    delete ui;
}


int uc_ReadIDCard::ProcessBussiness()
{
    bRunning = true;
    ThreadReadCard = std::thread(&uc_ReadIDCard::ThreadWork,this);

    return 0;
}

void uc_ReadIDCard::OnTimeout()
{
    bRunning = false;
    if (ThreadReadCard.joinable())
    {
        ThreadReadCard.join();
    }

}

void uc_ReadIDCard::ThreadWork()
{
    auto tLast = high_resolution_clock::now();
    while(bRunning)
    {
        auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
        if (tDuration.count() >= 1000)
        {
            tLast = high_resolution_clock::now();
            int nResult = ReaderIDCard();
            if (nResult == IDCard_Status::IDCard_Succeed)
            {
                emit SwitchNextPage();
                break;
                info() << "Succeed in Reader IDCard!";
            }
            else
            {
                char szText[256] = {0};
                GetErrorMessage((IDCard_Status)nResult,szText,sizeof(szText));
                error() << "Failed in Reader IDCard:"<<szText;
            }
        }
        else
        {
            this_thread::sleep_for(chrono::milliseconds(200));
        }
    }
}
#define    Error_Not_IDCARD         (-1)

int uc_ReadIDCard::ReaderIDCard()
{
    int nResult = IDCard_Status::IDCard_Succeed;
    do
    {
        nResult = OpenReader(nullptr);
        if (bSucceed && nDelayCount <= 3)
        {
            nDelayCount ++;
            break;
        }
        else
        {
            nDelayCount = 0;
            bSucceed = false;
        }

        nResult = OpenReader(nullptr);;
        if (nResult != IDCard_Status::IDCard_Succeed)
        {
            break;
        }
        nResult = FindIDCard();
        if (nResult != IDCard_Status::IDCard_Succeed)
        {
            break;
        }

        nResult = ReadIDCard(IDCard);
        if (nResult != IDCard_Status::IDCard_Succeed)
        {
            break;
        }
//        else
//        {
//            ui->label_Name->setText(QString::fromLocal8Bit((const char *)IDCard.szName));
//            ui->label_Gender->setText(QString::fromLocal8Bit((const char *)IDCard.szGender));
//            ui->label_Nation->setText(QString::fromLocal8Bit((const char *)IDCard.szNation));
//            ui->label_Birthday->setText(QString::fromLocal8Bit((const char *)IDCard.szBirthday));
//            ui->label_Identify->setText(QString::fromLocal8Bit((const char *)IDCard.szIdentify));
//            ui->label_Address->setText(QString::fromLocal8Bit((const char *)IDCard.szAddress));
//            ui->label_IssueAuthority->setText(QString::fromLocal8Bit((const char *)IDCard.szIszssueAuthority));
//            ui->label_ExpirationDate->setText(QString::fromLocal8Bit((const char *)IDCard.szExpirationDate1) + "-" + QString::fromLocal8Bit((const char *)IDCard.szExpirationDate2));
//            QImage ImagePhoto = QImage::fromData(IDCard.szPhoto,IDCard.nPhotoSize);
//            ImagePhoto.save("./Identify.bmp");
//            ui->label_Photo->setPixmap(QPixmap::fromImage(QImage::fromData(IDCard.szPhoto,IDCard.nPhotoSize)));
//        }
        bSucceed = true;
        CloseReader();
    }while(0);
    return nResult;
}
void uc_ReadIDCard::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_nTimerReaderIDCard)
    {
        int nResult = ReaderIDCard();
        if (nResult == IDCard_Status::IDCard_Succeed)
        {
            killTimer(m_nTimerReaderIDCard);
            m_nTimerReaderIDCard = 0;
            info() << "Succeed in Reader IDCard!";
        }
        else
        {
            char szText[256] = {0};
            GetErrorMessage((IDCard_Status)nResult,szText,sizeof(szText));
            error() << "Failed in Reader IDCard:"<<szText;
        }
    }
}
