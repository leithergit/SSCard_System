#ifndef UC_PAY_H
#define UC_PAY_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
#include "Payment.h"

namespace Ui {
	class Pay;
}

class uc_Pay : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_Pay(QLabel* pTitle, QString strStepImage, int nTimeout = 300, QWidget* parent = nullptr);
	~uc_Pay();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	void    ShutDownDevice();
	int     Pay(QString& strError);
	void    ThreadWork();
	int     uc_ReqestPaymentQR(QString& strMessage, QImage& Image);
	int     uc_QueryPayment(QString& strMessage, int& nPayStatus);
	int     GetQRCodeStorePath(QString& strFilePath);
	int     uc_ApplyCardReplacement(QString& strMessage, int nStatus);
	int     uc_ResgisterPayment(QString& strMessage, int nStatus);
	int     uc_MarkCard(QString& strMessage, int nStatus);
	int     uc_GetDataCard(QString& strMessage, SSCardInfoPtr& pSSCardInfo);
	int     uc_CancelMarkCard(QString& strMessage, int nStatus);
	int     uc_CancelCardReplacement(QString& strMessage, int nStatus);
	int     m_nWaitTime = 300;                         // 支付页面等侍时间，单位秒
	int     m_nQueryPayResultInterval = 500;            // 支付结构查询时间间隔单 毫秒
	int     m_nSocketRetryCount = 5;                    // 网络失败重试次数
	int     m_nSocketFailedCount = 0;
	int     m_nPayStatus = Pay_Not;
	int     uc_ProcessNotPay(QString& strMessage);
	int     uc_ProcessPayed(QString& strMessage);
	//int     uc_CancelCardment(QString& strMessage);
private:
	Ui::Pay* ui;
};

#endif // UC_PAY_H
