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

// 
// int  uc_Pay::uc_QueryPayment(QString& strMessage, int& nPayStatus)
// {
// 	int nRespond = -1;
// 	while (m_bWorkThreadRunning)
// 	{
// 		if (m_nSocketFailedCount >= m_nSocketRetryCount)
// 		{
// 			strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
// 			nRespond = OverSocketFailedCont;
// 			break;
// 		}
// 		nRespond = QueryPayment(strMessage, nPayStatus);
// 		if (QFailed(nRespond))
// 		{
// 			m_nSocketFailedCount++;
// 			strMessage = QString("查询支付结果失败:%1").arg(strMessage);
// 			int nWaitTimems = 0;
// 			while (m_bWorkThreadRunning && nWaitTimems < m_nQueryPayResultInterval)
// 			{
// 				this_thread::sleep_for(chrono::milliseconds(100));
// 				m_nQueryPayResultInterval += 100;
// 			}
// 		}
// 		else
// 		{
// 			m_nSocketFailedCount = 0;
// 			break;
// 		}
// 	}
// 	return nRespond;
// }

// int  uc_Pay::uc_ApplyCardReplacement(QString& strMessage, int nStatus)
// {
// 	int nRespond = -1;
// 	while (m_bWorkThreadRunning)
// 	{
// 		if (m_nSocketFailedCount >= m_nSocketRetryCount)
// 		{
// 			strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
// 			return OverSocketFailedCont;
// 		}
// 
// 		int nRespond = ApplyCardReplacement(strMessage, nStatus);
// 		if (QFailed(nRespond))
// 		{
// 			m_nSocketFailedCount++;
// 			strMessage = QString("申请补换卡失败:%1").arg(strMessage);
// 			this_thread::sleep_for(chrono::milliseconds(200));
// 		}
// 		else
// 		{
// 			m_nSocketFailedCount = 0;
// 			break;
// 		}
// 	}
// 	return nRespond;
// }

// int  uc_Pay::uc_ResgisterPayment(QString& strMessage, int nStatus)
// {
// 	int nRespond = -1;
// 	while (m_bWorkThreadRunning)
// 	{
// 		if (m_nSocketFailedCount >= m_nSocketRetryCount)
// 		{
// 			strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
// 			return OverSocketFailedCont;
// 		}
// 		nRespond = ResgisterPayment(strMessage, nStatus);
// 		if (QFailed(nRespond))
// 		{
// 			m_nSocketFailedCount++;
// 			strMessage = QString("缴费登记失败:%1").arg(strMessage);
// 			this_thread::sleep_for(chrono::milliseconds(200));
// 		}
// 		else
// 		{
// 			m_nSocketFailedCount = 0;
// 			break;
// 		}
// 	}
// 	return nRespond;
// }


// int   uc_Pay::uc_MarkCard(QString& strMessage, int nStatus)
// {
// 	int nRespond = -1;
// 	while (m_bWorkThreadRunning)
// 	{
// 		if (m_nSocketFailedCount >= m_nSocketRetryCount)
// 		{
// 			strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
// 			return OverSocketFailedCont;
// 		}
// 		int nStatus = 0;
// #pragma Warning("标注时可能返回00或01，都是申请成功，否则为失败，需要取消标注")
// 		nRespond = MarkCard(strMessage, nStatus);
// 		if (QFailed(nRespond))
// 		{
// 			m_nSocketFailedCount++;
// 			strMessage = QString("即制卡标注失败:%1").arg(strMessage);
// 			this_thread::sleep_for(chrono::milliseconds(200));
// 		}
// 		else
// 		{
// 			m_nSocketFailedCount = 0;
// 			break;
// 		}
// 	}
// 	return nRespond;
// }

// int   uc_Pay::uc_GetDataCard(QString& strMessage, SSCardInfoPtr& pSSCardInfo)
// {
// 	int nRespond = -1;
// 	while (m_bWorkThreadRunning)
// 	{
// 		if (m_nSocketFailedCount >= m_nSocketRetryCount)
// 		{
// 			strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
// 			return OverSocketFailedCont;
// 		}
// 
// 		nRespond = GetCardData(strMessage, pSSCardInfo);
// 		if (QFailed(nRespond))
// 		{
// 			m_nSocketFailedCount++;
// 			strMessage = QString("获取即制卡数据失败:%1").arg(strMessage);
// 			this_thread::sleep_for(chrono::milliseconds(200));
// 		}
// 		else
// 		{
// 			m_nSocketFailedCount = 0;
// 			break;
// 		}
// 	}
// 	return nRespond;
// }

// int  uc_Pay::uc_CancelMarkCard(QString& strMessage, int nStatus)
// {
// 	int nRespond = -1;
// 	while (m_bWorkThreadRunning)
// 	{
// 		if (m_nSocketFailedCount >= m_nSocketRetryCount)
// 		{
// 			strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
// 			return OverSocketFailedCont;
// 		}
// 		nRespond = CancelMarkCard(strMessage, nStatus);
// 		if (QFailed(nRespond))
// 		{
// 			m_nSocketFailedCount++;
// 			strMessage = QString("撤销即制卡标注失败:%1").arg(strMessage);
// 			this_thread::sleep_for(chrono::milliseconds(200));
// 		}
// 		else
// 		{
// 			m_nSocketFailedCount = 0;
// 			break;
// 		}
// 	}
// 	return nRespond;
// }

// int  uc_Pay::uc_CancelCardReplacement(QString& strMessage, int nStatus)
// {
// 	int nRespond = -1;
// 	while (m_bWorkThreadRunning)
// 	{
// 		if (m_nSocketFailedCount >= m_nSocketRetryCount)
// 		{
// 			strMessage = QString("%1网络操作连续失败次超过限定次数,支付流程中断!").arg(__FUNCTION__);
// 			return OverSocketFailedCont;
// 		}
// 
// 		nRespond = CancelCardReplacement(strMessage, nStatus);
// 		if (QFailed(nRespond))
// 		{
// 			m_nSocketFailedCount++;
// 			strMessage = QString("撤销申请补换卡失败:%1").arg(strMessage);
// 			this_thread::sleep_for(chrono::milliseconds(200));
// 		}
// 		else
// 		{
// 			m_nSocketFailedCount = 0;
// 			break;
// 		}
// 	}
// 	return nRespond;
// }

// int uc_Pay::uc_CancelCardment(QString& strMessage)
// {
// 	int nResult = uc_CancelMarkCard(strMessage);
// 	if (QFailed(nResult))
// 		return nResult;
// 
// 	return uc_CancelCardReplacement(strMessage);
// }

void uc_Pay::ThreadWork()
{
	auto tLast = high_resolution_clock::now();

	int nResult = 0;
	QString strMessage;
	QString strPayUrl;
	int nStatus = 0;

	while (m_bWorkThreadRunning)
	{
		auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
		if (tDuration.count() >= m_nQueryPayResultInterval)
		{
			do
			{
				if (m_nPayStatus == Pay_Not)										// 初始状态为未支付
				{
					nResult = QueryPayment(strMessage, m_nPayStatus);				// 查询支付结果
					m_nPayStatus = Pay_Succeed;
#pragma Warning("未支付也将流程继续进行，即作测试用!")
					if (m_nPayStatus == Pay_Not)
					{
						break;
					}
					else if (m_nPayStatus == Pay_Succeed)
					{
						nResult = ApplyCardReplacement(strMessage, nStatus);     //  申请补换卡
						if (QFailed(nResult))
							break;
						if (nStatus != 0 && nStatus != 1)
						{
							nResult = -1;
							break;
						}

						nResult = ResgisterPayment(strMessage, nStatus);          // 缴费登记
						if (QFailed(nResult))
							break;

						if (nStatus != 0 && nStatus != 1)
						{
							nResult = -1;
							break;
						}
					}
					else
						break;
				}
				else
				{
					m_nPayStatus = nResult;
					nResult = ApplyCardReplacement(strMessage, nStatus);          //  之前已经支付过，只不作缴费登录申请补换卡
					if (QFailed(nResult))
						break;
					if (nStatus != 0 && nStatus != 1)
					{
						nResult = -1;
						break;
					}
				}

				nResult = MarkCard(strMessage, nStatus);
				if (QFailed(nResult))
				{
					nResult = CancelMarkCard(strMessage, nStatus);
					if (QFailed(nResult))
						break;

					nResult = CancelCardReplacement(strMessage, nStatus);
					if (QFailed(nResult))
						break;
				}
				if (nStatus != 0 && nStatus != 1)
				{
					nResult = -1;
					break;
				}

				nResult = GetCardData(strMessage, nStatus);
				if (QFailed(nResult))
				{
					nResult = CancelMarkCard(strMessage, nStatus);
					if (QFailed(nResult))
						break;

					nResult = CancelCardReplacement(strMessage, nStatus);
					if (QFailed(nResult))
						break;
				}
				if (nStatus != 0 && nStatus != 1)
				{
					nResult = -1;
					break;
				}

			} while (0);
			tLast = high_resolution_clock::now();
			if (nResult == Pay_Not)
			{
				this_thread::sleep_for(chrono::milliseconds(100));
				continue;
			}
			else if (QSucceed(nResult))
			{
				emit ShowMaskWidget("操作成功", strMessage, Success, Switch_NextPage);
				break;
			}
			else if (QFailed(nResult))
			{
				gError() << strMessage.toLocal8Bit().data();
				emit ShowMaskWidget("操作失败", strMessage, Failed, Return_MainPage);
				break;
			}
		}

		this_thread::sleep_for(chrono::milliseconds(100));
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
