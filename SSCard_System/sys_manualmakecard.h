﻿#ifndef SYS_MANUALMAKECARD_H
#define SYS_MANUALMAKECARD_H

#include <QWidget>
#include "Gloabal.h"
#include "Payment.h"

namespace Ui {
	class Sys_ManualMakeCard;
}

class Sys_ManualMakeCard : public QWidget
{
	Q_OBJECT

public:
	explicit Sys_ManualMakeCard(QWidget* parent = nullptr);
	~Sys_ManualMakeCard();
	int ReaderIDCard(const char* szPort, IDCardInfo& CardInfo);
	void fnThreadReadIDCard(string strPort);
	IDCardInfo CardInfo;
	SSCardInfoPtr pSSCardInfo;
	bool bThreadReadIDCardRunning = false;
	std::thread* pThreadReadIDCard;
	void EnableUI(QObject* pUIObj, bool bEnable = true);
	QString strCurrentProgress = "";
	optional<CJsonObjectPtr> ProgressJson;
	int ReadSSCardInfo(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);
	int	QuerySSCardStatus(QString& strMessage, SSCardInfoPtr& pSSCardInfo);
	void ProceBatchLock();
	void ProcessPowerOnFailed();
	void PrintCardData();
	void PrintPhoto();
	void EnableCard();
    void TestApplyNewCard();

    void TestQueryPayUrl();

    int uc_ReqestPaymentQR2(QString& strMessage, QString& strPayCode, QString& strTransTime, QImage& QRImage);

	int LoadProgress(QString &strProgressFile,QString& strMesssage);

	void GetProgressFile(IDCardInfoPtr& pIDCard, QString &strFile,bool bFinished = false);

signals:
	void ShowIDCardInfo(bool bSuccceed, QString strMessage);

private slots:
	void on_pushButton_ReadID_clicked();

	void on_pushButton_QueryCardStatus_clicked();

	void on_pushButton_LoadCardData_clicked();

	void on_pushButton_MakeCard_clicked();

	void on_ShowIDCardInfo(bool bSucceed, QString strMessage);

private:
	Ui::Sys_ManualMakeCard* ui;
};

#endif // SYS_MANUALMAKECARD_H
