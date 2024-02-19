#ifndef UC_INPUTIDCARDINFO_H
#define UC_INPUTIDCARDINFO_H

#include <QWidget>
#include <QButtonGroup>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QToolTip>
#include <QVariantList>
#include "qstackpage.h"
#include "sys_dialogreadidcard.h"
#include "Gloabal.h"
#include <QCompleter>

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
	int		GetCardInfo(IDCardInfoPtr& pIDCard, QString& strMessage);
	int		GetSSCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage);
	void	ResetPage();
	IDCardInfoPtr pIDCard = nullptr;
	SSCardBaseInfoPtr pSSCardInfo = nullptr;
	QButtonGroup* pButtonGrp = nullptr;
	//QSqlDatabase SQLiteDB;
	Sys_DialogReadIDCard* pDialogReadIDCard = nullptr;
	//bool    InitializeDB(QString& strMessage);
	//bool    OpenDB(QSqlDatabase& DB, QString& strMessage);
	bool    bInitialized = false;
	void	StartDetect();
	void	StopDetect();
	void    ThreadWork();
	void	ShowGuardianWidget(bool bShow);
	void	ClearGuardianInfo();
	bool	LoadPersonInfo(QString strJson);
	void	SavePersonInfo();
	bool	eventFilter(QObject* watched, QEvent* event) override;
	void	ClearInfo();
	bool	GetPersonName(QString strPersonFile, QString& strName);
	void	HideItem();

	QSpacerItem* horizontalSpacer_Mobile = nullptr;
	QCompleter* pCompleter = nullptr;
	map<QString, QString> mapPersonFile;

signals:
	void AddNewIDCard(IDCardInfo* IDCard);
	void CheckPersonInfo();
protected:
	//virtual void showEvent(QShowEvent* event) override;
private slots:
	//void on_pushButton_Query_clicked();
	void on_CheckPersonInfo();

	void on_pushButton_OK_clicked();

	void on_pushButton_GetCardID_clicked();

	static int QueryPersonInfo(void*);

	void on_AddNewIDCard(IDCardInfo* pIDCard);

	//void on_comboBox_Province_currentIndexChanged(int index);

	//void on_comboBox_City_currentIndexChanged(int index);

	//void on_comboBox_County_currentIndexChanged(int index);

	//void on_comboBox_Town_currentIndexChanged(int index);

	void on_checkBox_Agency_stateChanged(int arg1);

	void on_pushButton_TakePhoto_clicked();

	void on_pushButton_SelectPhoto_clicked();

	void on_checkBox_Agency_clicked();

	// UI编辑事件和程序编辑事件都会触发
	void on_Name_textChanged(const QString& arg1);
	void on_Name_EditingFinished();

	// 只有UI编辑事件会触发
	void on_lineEdit_Name_textEdited(const QString& arg1);
	// 只响应UI编辑事件
	void on_lineEdit_CardID_textEdited(const QString& arg1);

    void on_comboBox_CardStatues_currentIndexChanged(int index);

    void on_comboBox_DocType_currentIndexChanged(int index);

    void on_comboBox_Guoji_currentIndexChanged(int index);

    void on_comboBox_GuardianDocType_currentIndexChanged(int index);

private:
	Ui::uc_InputIDCardInfo* ui;
};

#endif // UC_INPUTIDCARDINFO_H
