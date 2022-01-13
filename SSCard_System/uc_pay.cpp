#pragma execution_character_set("utf-8")
#include "uc_pay.h"
#include "ui_uc_pay.h"

#include <QPainter>
#include <QImage>
#include <QString>
#include <QPixmap>
#include <QLabel>
#include "Payment.h"
#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;
#include "Gloabal.h"

//#include "mainwindow.h"

uc_Pay::uc_Pay(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::Pay)
{
	ui->setupUi(this);
}

uc_Pay::~uc_Pay()
{
	ShutDown();
	delete ui;
}


int uc_Pay::ProcessBussiness()
{
	/*if (g_pMaskWindow)
		g_pMaskWindow->hide();*/
	ui->label_PaymentQRCode->setStyleSheet(QString::fromUtf8("border-image: url(:/Image/qr_pay.png);"));

	//m_nWaitTime = 300;                         // 支付页面等侍时间，单位秒
	m_nQueryPayResultInterval = g_pDataCenter->GetSysConfigure()->PaymentConfig.nQueryPayResultInterval;            // 支付结构查询时间间隔单 毫秒
	m_nSocketRetryCount = g_pDataCenter->GetSysConfigure()->PaymentConfig.nSocketRetryCount;                    // 网络失败重试次数
	// 这里可能是个耗时的过程，应该需要一个等待画面
	QString strMessage;
	// 获取二维码，并生成图像
	QImage QRImage;
	QString strTitle;

	if (QFailed(QueryPayment(strMessage, m_nPayStatus)))
	{
		gError() << gQStr(strMessage);
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}
	QString strPayCode;
	switch (m_nPayStatus)
	{
	case Pay_Not:
	{
		int nRespond = uc_ReqestPaymentQR(strMessage, strPayCode, QRImage);
		if (QFailed(nRespond))
		{
			gError() << strMessage.toLocal8Bit().data();
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return -1;
		}
		gInfo() << strMessage.toLocal8Bit().data();
		// 获取二维码保存路径
		QString strQRPath;
		if (GetQRCodeStorePath(strQRPath))
		{
			gError() << strMessage.toLocal8Bit().data();
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return -1;
		}
		g_pDataCenter->strPayCode = strPayCode.toStdString();
		// 保存二维码图像，并显示
		QRImage.save(strQRPath);
		QString strQSS = QString("border-image: url(%1)").arg(strQRPath);
		ui->label_PaymentQRCode->setStyleSheet(strQSS);
		break;
		//[[fallthrough]];
	}
	case Pay_Succeed:
	{
		gInfo() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("操作成功", "制卡费用已经成功支付,准备制卡", Success, Stay_CurrentPage);
		return 0;
	}
	default:
	{
		gError() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strMessage, Fetal, Return_MainPage);
		return -1;
	}
	}

	if (!m_pWorkThread)
	{
		m_bWorkThreadRunning = true;
		m_pWorkThread = new std::thread(&uc_Pay::ThreadWork, this);
		if (!m_pWorkThread)
		{
			QString strError = QString("内存不足,创建支付查询线程失败!");
			gError() << strError.toLocal8Bit().data();
			emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
			return -1;
		}
	}

	return 0;
}

int  uc_Pay::uc_ReqestPaymentQR(QString& strMessage, QString& strPayCode, QImage& QRImage)
{
	int nRespond = -1;
	QString strPaymentUrl;

	nRespond = RequestPaymentUrl(strPaymentUrl, strPayCode, strMessage);
	if (QSucceed(nRespond))
	{
		if (QFailed(QREnncodeImage(strPaymentUrl, 2, QRImage)))
		{
			return Failed_QREnocode;
		}
		else
			return  0;
	}
	else
	{
		strMessage = QString("查询支付二维码失败:%1").arg(strMessage);
		return -1;
	}
}

bool Delay(bool& bFlag, int nDelay, int nStep = 100)
{
	if (nDelay <= nStep)
	{
		this_thread::sleep_for(chrono::milliseconds(nDelay));
		return bFlag;
	}
	auto tStart = high_resolution_clock::now();
	while (bFlag)
	{
		this_thread::sleep_for(chrono::milliseconds(nStep));
		auto tDuration = high_resolution_clock::now() - tStart;
		if (tDuration.count() >= nDelay)
		{
			break;
		}
	}
	return bFlag;
}

void uc_Pay::ThreadWork()
{
	auto tLast = high_resolution_clock::now();
	int nResult = 0;
	QString strMessage;
	QString strPayUrl;
	SysConfigPtr& pSysConfig = g_pDataCenter->GetSysConfigure();
	//int nDelay = pSysConfig->nPageTimeout[Page_Payment];
	/*if (g_pDataCenter->bDebug)
		nDelay = 20000;*/
	auto tStart = high_resolution_clock::now();

	PayResult nPayResult = PayResult::WaitforPay;

	while (m_bWorkThreadRunning)
	{
		nResult = queryPayResult(g_pDataCenter->strPayCode, strMessage, nPayResult);				// 查询支付结果
		if (QFailed(nResult))
		{
			if (!Delay(m_bWorkThreadRunning, pSysConfig->PaymentConfig.nQueryPayFailedInterval))
				break;
			continue;
		}
		//		if (g_pDataCenter->bDebug)
		//		{
		//#pragma Warning("未支付也将流程继续进行，即作演示用!")
		//			auto tDelay = duration_cast<milliseconds>(high_resolution_clock::now() - tStart);
		//			if (tDelay.count() >= nDelay)
		//				nPayResult = PayResult::PaySucceed;
		//		}

		if (nPayResult == PayResult::PaySucceed)
			break;

		if (!Delay(m_bWorkThreadRunning, pSysConfig->PaymentConfig.nQueryPayResultInterval))
			break;
	}

	if (QFailed(nResult))
	{
		emit ShowMaskWidget("操作失败", strMessage, Failed, Return_MainPage);
		return;
	}
	if (nPayResult == PayResult::PaySucceed)
	{
		strMessage = "费用已支付,现将进入制卡流程,请确认进卡口已放入空白社保卡片!";
		emit ShowMaskWidget("操作成功", strMessage, Success, Switch_NextPage);
	}
	else
	{
		QString strPayResultString[] =
		{
			"待支付",
			"待确认",
			"支付完成",
			"支付失败",
			"订单不存在",
			"订单取消"
		};
		if (nPayResult >= PayResult::WaitforPay && nPayResult <= PayResult::OrderCanceled)
			strMessage = QString("支付结果状态:%1").arg(strPayResultString[(int)nPayResult]);
		else
			strMessage = QString("非预期的支付结果状态:%1").arg((int)nPayResult);
		gError() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("操作失败", strMessage, Failed, Return_MainPage);
	}
}

int  uc_Pay::GetQRCodeStorePath(QString& strFilePath)
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
			char szError[1024] = { 0 };
			_strerror_s(szError, 1024, nullptr);
			QString Info = QString("创建二维码保存目录'%1'失败:%2").arg(strStorePath, szError);
			gInfo() << Info.toLocal8Bit().data();
			return -1;
		}
	}
	strFilePath = strStorePath + QString("QRCode_%1.bmp").arg((const char*)g_pDataCenter->GetIDCardInfo()->szIdentify);
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
	ShutDown();
}
