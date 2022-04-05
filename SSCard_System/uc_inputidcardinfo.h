#ifndef UC_INPUTIDCARDINFO_H
#define UC_INPUTIDCARDINFO_H

#include <QWidget>
#include <QButtonGroup>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>
#include "qstackpage.h"
#include "sys_dialogreadidcard.h"
#include "Gloabal.h"

namespace Ui {
	class uc_InputIDCardInfo;
}

class uc_InputIDCardInfo : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_InputIDCardInfo(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~uc_InputIDCardInfo();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	virtual void  ShutDown() override;
	virtual void timerEvent(QTimerEvent* event) override;
	int		GetCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage);
	int		GetSSCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage);
	void	ResetPage();
	IDCardInfoPtr pIDCard = nullptr;
	SSCardBaseInfoPtr pSSCardInfo = nullptr;
	QButtonGroup* pButtonGrp = nullptr;
	QSqlDatabase SQLiteDB;
	Sys_DialogReadIDCard* pDialogReadIDCard = nullptr;
	bool    InitializeDB(QString& strMessage);
	bool    bInitialized = false;
	void	StartDetect();
	void	StopDetect();
	void    ThreadWork();
	void	ShowGuardianWidget(bool bShow);
	void	ClearGuardianInfo();
	QSpacerItem* horizontalSpacer_Mobile = nullptr;

signals:
	void AddNewIDCard(IDCardInfo* IDCard);
protected:
	//virtual void showEvent(QShowEvent* event) override;
private slots:
	//void on_pushButton_Query_clicked();

	void on_pushButton_OK_clicked();

	void on_AddNewIDCard(IDCardInfo* pIDCard);

	void on_comboBox_Province_currentIndexChanged(int index);

	void on_comboBox_City_currentIndexChanged(int index);

	void on_comboBox_County_currentIndexChanged(int index);

	void on_comboBox_Town_currentIndexChanged(int index);

	void on_checkBox_Agency_stateChanged(int arg1);

	void on_pushButton_TakePhoto_clicked();

	void on_pushButton_SelectPhoto_clicked();

	void on_checkBox_Agency_clicked();

private:
	Ui::uc_InputIDCardInfo* ui;
};

#endif // UC_INPUTIDCARDINFO_H
