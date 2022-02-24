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
	void ThreadWork();
	int  PremakeCard(QString& strMessage);
	int  CommitPersionInfo(QString& strMessage);
	int	 WriteCard(QString& strMessage);
	int  EnsureData(QString& strMessage);
	int  ActiveCard(QString& strMessage);
	//int  PrintCard(QString& strMessage);

	virtual void ShutDown() override;
	int     m_nSocketRetryInterval = 500;            // 支付结构查询时间间隔单 毫秒
	int     m_nSocketRetryCount = 5;                    // 网络失败重试次数
	int     m_nSocketFailedCount = 0;

private:
	Ui::MakeCard* ui;

	SSCardInfoPtr m_pSSCardInfo = nullptr;
};

#endif // UC_MAKECARD_H
