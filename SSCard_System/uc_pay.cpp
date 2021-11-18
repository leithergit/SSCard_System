#pragma execution_character_set("utf-8")
#include "uc_pay.h"
#include "ui_uc_pay.h"
#include "Gloabal.h"
#include <QPainter>
#include <QImage>
#include <QString>
#include <QPixmap>
#include <QLabel>
#include "Payment.h"

//#include "mainwindow.h"

uc_Pay::uc_Pay(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::Pay)
{
	ui->setupUi(this);
}

uc_Pay::~uc_Pay()
{
    ShutDownDevice();
	delete ui;
}

int uc_Pay::ProcessBussiness()
{
    //m_nWaitTime = 300;                         // 支付页面等侍时间，单位秒
    m_nQueryPayResultInterval = g_pDataCenter->GetSysConfigure()->PaymentConfig.nQueryPayResultInterval;            // 支付结构查询时间间隔单 毫秒
    m_nSocketRetryCount = g_pDataCenter->GetSysConfigure()->PaymentConfig.nSocketRetryCount    ;                    // 网络失败重试次数
    // 这里可能是个耗时的过程，应该需要一个等待画面
    QString strMessage;
    int nResult = 0;
//    if (!QFailed(QueryPayResult(strMessage,nResult)))
//    {
//        emit ShowMaskWidget(strMessage,Fetal,Return_MainPage);
//        return -1;
//    }

//    if (QSucceed(nResult))
//    {
//        emit ShowMaskWidget("支持成功,准备开始制卡",Success,Skip_NextPage);
//        return 0;
//    }

    // 获取二维码，并生成图像
    QImage QRImage;
    if (QFailed(ReqestPaymentQR(strMessage,nResult,QRImage)))
    {
        emit ShowMaskWidget(strMessage,Fetal,Return_MainPage);
        return -1;
    }

    // 获取二维码保存路径
    QString strQRPath;
    if (!GetQRCodeStorePath(strQRPath))
    {
        emit ShowMaskWidget(strMessage,Fetal,Return_MainPage);
        return -1;
    }
    // 保存二维码图像，并显示
    QRImage.save(strQRPath);
    QString strQSS = QString(u8"border-image: url(%1)").arg(strQRPath);
    ui->label_PaymentQRCode->setStyleSheet(strQSS);

    m_bWorkThreadRunning = true;
    m_pWorkThread = new std::thread(&uc_Pay::ThreadWork, this);
    if (!m_pWorkThread)
    {
        QString strError = QString("内存不足,创建密码键盘线程失败!");
        gError()<<strError.toLocal8Bit().data();
        emit ShowMaskWidget(strError,Fetal,Return_MainPage);
        return -1;
    }
	return 0;
}

int  uc_Pay::uc_ReqestPaymentQR(QString &strMessage,int &nResult,QImage &Image)
{

}

#define OverSocketFailedCont   -2

int  uc_Pay::uc_QueryPayResult(QString &strMessage)
{
    while ( m_bWorkThreadRunning)
    {
        if (m_nSocketFailedCount >= m_nSocketRetryCount)
        {
            strMessage = QString("网络操作连续失败次超过%1次,支付流程中断!").arg(m_nSocketRetryCount).toLocal8Bit().data();
            return OverSocketFailedCont;
        }
        int nRespond = QueryPayResult(strMessage);
        if (QFailed(nRespond))
        {
           m_nSocketFailedCount ++;
           gError()<<QString("查询支付结果失败:%1").arg(strMessage).toLocal8Bit().data();
           this_thread::sleep_for(chrono::milliseconds(100));
        }
        m_nSocketFailedCount = 0;
        return 0;
    }
}

int  uc_Pay::GetQRCodeStorePath(QString &strFilePath)
{

}

int  uc_Pay::uc_GetPaymentUrl(QString &strMessage,QString &strPaymentUrl)
{

}

int  uc_Pay::uc_ApplyCardReplacement(QString &strMessage)
{
    int nRespond = ApplyCardReplacement(strMessage);
    if (QFailed(nRespond))
    {
         m_nSocketFailedCount ++;
         gError()<<QString("申请补换卡失败:%1").arg(strMessage).toLocal8Bit().data();
         if (m_nSocketFailedCount >= m_nSocketRetryCount)
         {
             gError()<<QString("网络操作连续失败次超过%1次,支付流程中断!").arg(nSocketRetryCount).toLocal8Bit().data();
             return OverSocketFailedCont;
         }
        return -1;
    }
    m_nSocketFailedCount = 0;
    if (QFailed(nResult))
    {
        strMessage = QString("申请补换卡结果:%1,稍后重试!").arg(nResult);
        gError()<<strMessage.toLocal8Bit().data();
        break;
    }
    bSSCardReplaceSucceed = true;
}

int  uc_Pay::uc_ResgisterPayment(QString &strMessage,int &nResult)
{

}

void uc_Pay::ThreadWork()
{
    auto tLast = high_resolution_clock::now();
    bool bPaySucceed = false;
    bool bSSCardReplaceSucceed = false;
    bool bRegisterPayment = false;
    bool bMarkCardSucced = false;
    bool bGetCardDataSucceed = false;
    int nResult = 0;
    QString strMessage;
    int     nSocketFailedCount = 0;
    while(m_bWorkThreadRunning)
    {
        auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
        if (tDuration.count() >= nQueryPayResultInterval)
        {
            do
            {
                if (!bPaySucceed)
                {

                    bPaySucceed = true;
                }

                if (!bSSCardReplaceSucceed)
                {

                }

                if (!bRegisterPayment)
                {
                    if (QFailed(ResgisterPayment(strMessage,nResult)))
                    {
                        gError()<<QString("缴费登记失败:%1").arg(strMessage).toLocal8Bit().data();
                        if (nSocketFailedCount >= nSocketRetryCount)
                        {
                            gError()<<QString("网络操作连续失败次超过%1次,支付流程中断!").arg(nSocketRetryCount).toLocal8Bit().data();
                            m_bWorkThreadRunning = false;
                            emit ShowMaskWidget(strMessage,Fetal,Return_MainPage);
                        }
                        break;
                    }
                    if (QFailed(nResult))
                    {
                        strMessage = QString("缴费登记失败结果:%1,稍后重试!").arg(nResult);
                        gError()<<strMessage.toLocal8Bit().data();
                        break;
                    }
                    bRegisterPayment = true;
                }


                emit ShowMaskWidget(strMessage,Success,Switch_NextPage);

            }while(0);
            tLast = high_resolution_clock::now();
        }
        else
        {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}



int  uc_Pay::GetQRCodeStorePath(QString &strFilePath)
{
    QString strStorePath = QCoreApplication::applicationDirPath();
    strStorePath += "/QRCode/";
    strStorePath += QDateTime::currentDateTime().toString("yyyyMMdd/");
    QFileInfo fi(strStorePath);
    if (!fi.isDir())
    {// 当天目录不存在？则创建目录
        QDir storeDir;
        if (!storeDir.mkpath(strStorePath))
        {
            char szError[1024] = {0};
            _strerror_s(szError,1024,nullptr);
            QString Info = QString("创建二维码保存目录'%1'失败:%2").arg(strStorePath,szError);
            gInfo()<<Info.toLocal8Bit().data();
            return -1;
        }
    }
    strFilePath = strStorePath + QString("QRCode_%1.bmp").arg((const char *)g_pDataCenter->GetIDCardInfo()->szIdentify);
    return 0;
}

int  uc_Pay::ReqestPaymentQR(QString &strMessage,int &nResult,QImage &QRImage)
{
    QString strPaymentUrl;
    if (QFailed(GetPaymentUrl(strMessage,strPaymentUrl)))
    {
        return -1;
    }

    QREnncodeImage(strPaymentUrl,2,QRImage);
    nResult = 0;
}

void uc_Pay::OnTimeout()
{
    ShutDownDevice();
}

void uc_Pay::ShutDownDevice()
{
    m_bWorkThreadRunning = false;
    if (m_pWorkThread && m_pWorkThread->joinable())
    {
        m_pWorkThread->join();
        delete m_pWorkThread;
        m_pWorkThread = nullptr;
    }
}


