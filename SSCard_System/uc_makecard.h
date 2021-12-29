#ifndef UC_MAKECARD_H
#define UC_MAKECARD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

#include <QCoreApplication>
#include <QDebug>
#include <string.h>
#include <exception>
#include <Windows.h>
#include "Gloabal.h"
#include "DevBase.h"

using namespace std;
//using namespace Kingaotech;

namespace Ui {
	class MakeCard;
}

class uc_MakeCard : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_MakeCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~uc_MakeCard();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	int OpenDevice(QString& strMessage);
	int OpenPrinter(QString& strMesssage);
	int OpenReader(QString& strMesssage);
	string MakeCardInfo(string strATR, SSCardInfoPtr& pSSCardInfo);
	int WriteCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage);
	int PrintCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage);
	//int GetCA(string& strPublicKey, SSCardInfoPtr& pSSCardInfo, CAInfo& caInfo, QString& strMessage);
	int Depense(QString& strMessage);
	int PrecessCardInMaking(QString& strMessage);
	int PrepareMakeCard(QString& strMessage);
	void CloseDevice();
	void ThreadWork();
	virtual void ShutDown() override;
	int     m_nSocketRetryInterval = 500;            // 支付结构查询时间间隔单 毫秒
	int     m_nSocketRetryCount = 5;                    // 网络失败重试次数
	int     m_nSocketFailedCount = 0;

private:
	Ui::MakeCard* ui;
	KT_PrinterLibPtr	m_pPrinterlib = nullptr;
	KT_ReaderLibPtr		m_pReaderLib = nullptr;
	KT_Printer* m_pPrinter = nullptr;
	KT_Reader* m_pReader = nullptr;

	SSCardInfoPtr m_pSSCardInfo = nullptr;
};

#endif // UC_MAKECARD_H
