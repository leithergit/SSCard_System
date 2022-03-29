#ifndef SYS_MANUALMAKECARD_H
#define SYS_MANUALMAKECARD_H

#include <QWidget>
#include "Gloabal.h"
#include "Payment.h"
#include <QButtonGroup>

namespace Ui {
	class Sys_ManualMakeCard;
}

class Sys_ManualMakeCard : public QWidget
{
	Q_OBJECT

public:
	explicit Sys_ManualMakeCard(QWidget* parent = nullptr);
	~Sys_ManualMakeCard();
	int ReaderIDCard(const char* szPort, IDCardInfo* pCardInfo);
	void fnThreadReadIDCard(string strPort);
	IDCardInfo CardInfo;
	bool bThreadReadIDCardRunning = false;
	std::thread* pThreadReadIDCard = nullptr;
	bool bThreadMakeCardRunning = false;
	std::thread* pThreadMakeCard = nullptr;
	void EnableUI(QObject* pUIObj, bool bEnable = true);
	QButtonGroup* pButtonGrpGender = nullptr;
	QButtonGroup* pButtonGrpBusiness = nullptr;
	QButtonGroup* pButtonGrpServiceType = nullptr;
	void ThreadMakeCard();
	void ProcessPowerOnFailed();
	void PrintCardData();
	void PrintPhoto();
	void EnableCard();
	void ShowSSCardInfo();
	int LoadPersonSSCardData(QString& strMesssage);
	QVector<QLabel*> m_LableStep;

signals:
	void    ShowIDCardInfo(bool bSuccceed, QString strMessage);
	void	UpdateProgress(int nStep);
	void    ShowMessage(QMessageBox::Icon nIcon, QString strTitle, QString strMessage);
private slots:
	void on_pushButton_ReadID_clicked();

	void on_pushButton_QueryCardInfo_clicked();

	void on_pushButton_LoadCardData_clicked();

	void on_pushButton_MakeCard_clicked();

	void on_ShowIDCardInfo(bool bSucceed, QString strMessage);

	void on_checkBox_WithoutIDCard_stateChanged(int arg1);

	void on_checkBox_Debug_stateChanged(int arg1);

	void on_pushButton_PremakeCard_clicked();

	void on_ShowMessage(QMessageBox::Icon nIcon, QString strTitle, QString strMessage);

	void OnUpdateProgress(int nStep);

private:
	Ui::Sys_ManualMakeCard* ui;
};

#endif // SYS_MANUALMAKECARD_H
