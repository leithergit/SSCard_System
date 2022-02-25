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

enum MakeCard_Progress
{
	MP_PreMakeCard = 0,
	//Depense,
	MP_WriteCard,
	MP_PrintCard,
	MP_EnableCard,
	MP_RejectCard
};

class uc_MakeCard : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_MakeCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~uc_MakeCard();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	//int OpenDevice(QString& strMessage);
	//int OpenPrinter(QString& strMesssage);
	//int OpenSSCardReader(QString& strMesssage);
	//void CloseDevice();
	//int TestPrinter(QString& strMessage);
	//int Depense(QString& strMessage);
	//string MakeCardInfo(string strATR, SSCardInfoPtr& pSSCardInfo);
	//int WriteCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage);
	//int PrintCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage);
	int PrecessCardInMaking(QString& strMessage);
	int PrepareMakeCard(QString& strMessage);
	void ShowSSCardInfo();
	void ThreadWork();
	virtual void ShutDown() override;
	int     m_nSocketRetryInterval = 500;            // 支付结构查询时间间隔单 毫秒
	int     m_nSocketRetryCount = 5;                    // 网络失败重试次数
	int     m_nSocketFailedCount = 0;

public slots:
	void	OnUpdateProgress(int nStep);
signals:
	void	UpdateProgress(int nStep);
private slots:
    void on_pushButton_OK_clicked();

private:
	Ui::MakeCard* ui;
	QVector<QLabel*> m_LableStep;
};

#endif // UC_MAKECARD_H
