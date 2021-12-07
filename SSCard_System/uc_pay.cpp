#pragma execution_character_set("utf-8")
#include "uc_pay.h"
#include "ui_uc_pay.h"

#include <QPainter>
#include <QImage>
#include <QString>
#include <QPixmap>
#include <QLabel>
#include "Payment.h"
#include "Gloabal.h"

//#include "mainwindow.h"

uc_Pay::uc_Pay(QLabel* pTitle, QString strStepImage, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nTimeout, parent),
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

	switch (m_nPayStatus)
	{
	case Pay_Not:
	{
		int nRespond = uc_ReqestPaymentQR(strMessage, QRImage);
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
		break;
	}
	default:
	{
		gError() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strMessage, Fetal, Return_MainPage);
		return -1;
	}
	}

	m_bWorkThreadRunning = true;
	m_pWorkThread = new std::thread(&uc_Pay::ThreadWork, this);
	if (!m_pWorkThread)
	{
		QString strError = QString("内存不足,创建支付查询线程失败!");
		gError() << strError.toLocal8Bit().data();
		emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
		return -1;
	}

	return 0;
}

int  uc_Pay::uc_ReqestPaymentQR(QString& strMessage, QImage& QRImage)
{
	int nRespond = -1;
	QString strPaymentUrl;

	nRespond = RequestPaymentUrl(strPaymentUrl, strMessage);
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
		strMessage = QString("查询支付结果失败:%1").arg(strMessage);
		return -1;
	}
}

void uc_Pay::ThreadWork()
{
	auto tLast = high_resolution_clock::now();

	int nResult = 0;
	QString strMessage;
	QString strPayUrl;
	int nStatus = 0;
#ifdef _DEBUG
	int nDelay = 15000;
	auto tStart = high_resolution_clock::now();
#endif

	while (m_bWorkThreadRunning)
	{
		auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
		if (tDuration.count() >= m_nQueryPayResultInterval)
		{
			if (m_nPayStatus == Pay_Succeed)										// 初始状态为未支付
				break;

			nResult = QueryPayment(strMessage, m_nPayStatus);				// 查询支付结果
#ifdef _DEBUG
			auto tDelay = duration_cast<milliseconds>(high_resolution_clock::now() - tStart);
			if (tDelay.count() >= nDelay)
				m_nPayStatus = Pay_Succeed;
#pragma Warning("未支付也将流程继续进行，即作测试用!")
#endif
			if (m_nPayStatus == Pay_Succeed)
				break;

			tLast = high_resolution_clock::now();
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		this_thread::sleep_for(chrono::milliseconds(100));
	}

	if (m_nPayStatus == Pay_Succeed)
	{
		strMessage = "费用已支付,现将进入制卡流程!";
		emit ShowMaskWidget("操作成功", strMessage, Success, Switch_NextPage);
	}
	else if (QFailed(nResult))
	{
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
