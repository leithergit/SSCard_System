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
    m_nPayStatus = uc_QueryPayment(strMessage);

    switch(m_nPayStatus)
    {
    case Pay_Not:
    {
        int nRespond = uc_ReqestPaymentQR(strMessage,QRImage);
        if (QFailed(nRespond))
        {
            gError()<<strMessage.toLocal8Bit().data();
            emit ShowMaskWidget(strMessage,Fetal,Return_MainPage);
            return -1;
        }
        gInfo()<<strMessage.toLocal8Bit().data();
        // 获取二维码保存路径
        QString strQRPath;
        if (!GetQRCodeStorePath(strQRPath))
        {
            gError()<<strMessage.toLocal8Bit().data();

        }
        // 保存二维码图像，并显示
        QRImage.save(strQRPath);
        QString strQSS = QString(u8"border-image: url(%1)").arg(strQRPath);
        ui->label_PaymentQRCode->setStyleSheet(strQSS);

        break;
        //[[fallthrough]];
    }
    case Pay_Succeed:
    {
        gInfo()<<strMessage.toLocal8Bit().data();
        emit ShowMaskWidget("制卡费用已经成功支付,准备",Success,Stay_CurrentPage);
        break;
    }
    default:
    {
        gError()<<strMessage.toLocal8Bit().data();
        emit ShowMaskWidget(strMessage,Fetal,Return_MainPage);
        return -1;
    }
    }

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



int  uc_Pay::uc_ReqestPaymentQR(QString &strMessage,QImage &QRImage)
{
    int nRespond = -1;
    QString strPaymentUrl;
    while (true)
    {
        if (m_nSocketFailedCount >= m_nSocketRetryCount)
        {
            strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
            nRespond OverSocketFailedCont;
            break;
        }

        nRespond = RequestPaymentUrl(strMessage,strPaymentUrl);
        if (QSucceed(nRespond))
        {
            if (QFailed(QREnncodeImage(strPaymentUrl ,2,QRImage)))
            {
                nRespond = Failed_QREnocode;
            }
            else
                nRespond = 0;
            break;
        }
        else
        {
           m_nSocketFailedCount ++;
           strMessage = QString("查询支付结果失败:%1").arg(strMessage);
           this_thread::sleep_for(chrono::milliseconds(200));
        }

    }
    return nRespond;
}

int  uc_Pay::uc_QueryPayment(QString &strMessage)
{
    int nRespond = -1;
    while (m_bWorkThreadRunning)
    {
        if (m_nSocketFailedCount >= m_nSocketRetryCount)
        {
            strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
            nRespond OverSocketFailedCont;
            break;
        }
        nRespond = QueryPayment(strMessage);
        if (QFailed(nRespond))
        {
           m_nSocketFailedCount ++;
           strMessage = QString("查询支付结果失败:%1").arg(strMessage);
           this_thread::sleep_for(chrono::milliseconds(200));
        }
        else
        {
            m_nSocketFailedCount = 0;
            break;
        }
    }
    return nRespond;
}


int  uc_Pay::uc_ApplyCardReplacement(QString &strMessage)
{
    int nRespond = -1;
    while (m_bWorkThreadRunning)
    {
        if (m_nSocketFailedCount >= m_nSocketRetryCount)
        {
            strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
            return OverSocketFailedCont;
        }
        int nRespond = ApplyCardReplacement(strMessage);
        if (QFailed(nRespond))
        {
           m_nSocketFailedCount ++;
           strMessage = QString("申请补换卡失败:%1").arg(strMessage);
           this_thread::sleep_for(chrono::milliseconds(200));
        }
        else
        {
            m_nSocketFailedCount = 0;
            break;
        }
    }
    return nRespond;
}

int  uc_Pay::uc_ResgisterPayment(QString &strMessage)
{
    int nRespond = -1;
    while (m_bWorkThreadRunning)
    {
        if (m_nSocketFailedCount >= m_nSocketRetryCount)
        {
            strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
            return OverSocketFailedCont;
        }
        nRespond = ResgisterPayment(strMessage);
        if (QFailed(nRespond))
        {
           m_nSocketFailedCount ++;
           strMessage = QString("缴费登记失败:%1").arg(strMessage);
           this_thread::sleep_for(chrono::milliseconds(200));
        }
        else
        {
            m_nSocketFailedCount = 0;
            break;
        }
    }
    return nRespond;
}


int   uc_Pay::uc_MarkCard(QString &strMessage)
{
    int nRespond = -1;
    while (m_bWorkThreadRunning)
    {
        if (m_nSocketFailedCount >= m_nSocketRetryCount)
        {
            strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
            return OverSocketFailedCont;
        }
        nRespond = MarkCard(strMessage);
        if (QFailed(nRespond))
        {
           m_nSocketFailedCount ++;
           strMessage = QString("即制卡标注失败:%1").arg(strMessage);
           this_thread::sleep_for(chrono::milliseconds(200));
        }
        else
        {
            m_nSocketFailedCount = 0;
            break;
        }
    }
    return nRespond;
}

int   uc_Pay::uc_GetDataCard(QString &strMessage)
{
    int nRespond = -1;
    while (m_bWorkThreadRunning)
    {
        if (m_nSocketFailedCount >= m_nSocketRetryCount)
        {
            strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
            return OverSocketFailedCont;
        }
        nRespond = GetCardData(strMessage);
        if (QFailed(nRespond))
        {
           m_nSocketFailedCount ++;
           strMessage = QString("获取即制卡数据失败:%1").arg(strMessage);
           this_thread::sleep_for(chrono::milliseconds(200));
        }
        else
        {
            m_nSocketFailedCount = 0;
            break;
        }
    }
    return nRespond;
}

int  uc_Pay::uc_CancelMarkCard(QString &strMessage)
{
    int nRespond = -1;
    while (m_bWorkThreadRunning)
    {
        if (m_nSocketFailedCount >= m_nSocketRetryCount)
        {
            strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
            return OverSocketFailedCont;
        }
        nRespond = CancelMarkCard(strMessage);
        if (QFailed(nRespond))
        {
           m_nSocketFailedCount ++;
           strMessage = QString("撤销即制卡标注失败:%1").arg(strMessage);
           this_thread::sleep_for(chrono::milliseconds(200));
        }
        else
        {
            m_nSocketFailedCount = 0;
            break;
        }
    }
    return nRespond;
}
int  uc_Pay::uc_CancelCardReplacement(QString &strMessage)
{
    int nRespond = -1;
    while (m_bWorkThreadRunning)
    {
        if (m_nSocketFailedCount >= m_nSocketRetryCount)
        {
            strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
            return OverSocketFailedCont;
        }
        nRespond = CancelCardReplacement(strMessage);
        if (QFailed(nRespond))
        {
           m_nSocketFailedCount ++;
           strMessage = QString("撤销申请补换卡失败:%1").arg(strMessage);
           this_thread::sleep_for(chrono::milliseconds(200));
        }
        else
        {
            m_nSocketFailedCount = 0;
            break;
        }
    }
    return nRespond;
}

int uc_Pay::uc_CancelCardment(QString &strMessage)
{
    int nResult = uc_CancelMarkCard(strMessage);
    if (QFailed(nResult))
        return nResult;

    return uc_CancelCardReplacement(strMessage);
}

void uc_Pay::ThreadWork()
{
    auto tLast = high_resolution_clock::now();

    int nResult = 0;
    QString strMessage;   
    QString strPayUrl;

    while(m_bWorkThreadRunning)
    {
        auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
        if (tDuration.count() >= m_nQueryPayResultInterval)
        {
            do
            {
                if (m_nPayStatus == Pay_Not)                    // 初始状态为未支付
                {
                    nResult = QueryPayment(strMessage);         // 查询支付结果
                    if (m_nPayStatus == Pay_Not)
                    {
                        break;
                    }
                    else if (nResult == Pay_Succeed)
                    {
                        nResult = uc_ApplyCardReplacement(strMessage);     //  申请补换卡
                        if (QFailed(nResult))
                            break;

                        nResult = uc_ResgisterPayment(strMessage);          // 缴费登记
                        if (QFailed(nResult))
                            break;
                    }
                    else
                        break;
                }
                else
                {
                    m_nPayStatus = nResult;
                    nResult = uc_ApplyCardReplacement(strMessage);          //  之前已经支付过，只不作缴费登录申请补换卡
                    if (QFailed(nResult))
                        break;
                }

                nResult = uc_MarkCard(strMessage);
                if (QFailed(nResult))
                {
                    nResult = uc_CancelCardment(strMessage);
                    if (QFailed(nResult))
                        break;
                }

                nResult = uc_GetDataCard(strMessage);
                if (QFailed(nResult))
                {
                    nResult = uc_CancelCardment(strMessage);
                    if (QFailed(nResult))
                        break;
                }

            }while(0);
            tLast = high_resolution_clock::now();
            if (nResult == Pay_Not)
            {
                this_thread::sleep_for(chrono::milliseconds(100));
                continue;
            }
            else if (QSucceed(nResult))
            {
                emit ShowMaskWidget(strMessage,Success,Switch_NextPage);
            }
            else if (QFailed(nResult))
            {
                gError()<<strMessage.toLocal8Bit().data();
                emit ShowMaskWidget(strMessage,Fetal,Return_MainPage);
            }
        }

        this_thread::sleep_for(chrono::milliseconds(100));
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

//int  uc_Pay::ReqestPaymentQR(QString &strMessage,int &nResult,QImage &QRImage)
//{
//    QString strPaymentUrl;
//    if (QFailed(GetPaymentUrl(strMessage,strPaymentUrl)))
//    {
//        return -1;
//    }

//    QREnncodeImage(strPaymentUrl,2,QRImage);
//    nResult = 0;
//}

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


