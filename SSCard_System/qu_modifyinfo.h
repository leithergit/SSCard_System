#ifndef QU_MODIFYINFO_H
#define QU_MODIFYINFO_H

#include <QWidget>
#include <QButtonGroup>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>
#include "qstackpage.h"
#include "sys_dialogreadidcard.h"
#include "Gloabal.h"
#include <QCompleter>

namespace Ui {
	class qu_ModifyInfo;
}

class qu_ModifyInfo : public QStackPage
{
	Q_OBJECT

public:
	explicit qu_ModifyInfo(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~qu_ModifyInfo();
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
	QButtonGroup* pButtonGrp2 = nullptr;
	//QSqlDatabase SQLiteDB;
	Sys_DialogReadIDCard* pDialogReadIDCard = nullptr;
	//bool    InitializeDB(QString& strMessage);
	//bool    OpenDB(QSqlDatabase& DB, QString& strMessage);
	bool    bInitialized = false;
	//void	StartDetect();
	//void	StopDetect();
	void    ThreadWork();
	void	ShowGuardianWidget(bool bShow);
	void	ClearGuardianInfo();
	bool	LoadPersonInfo(QString strJson);
	void	SavePersonInfo();
	bool	eventFilter(QObject* watched, QEvent* event) override;
	void	ClearInfo();
	bool	GetPersonName(QString strPersonFile, QString& strName);
	//void	HideItem();

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

	//void on_pushButton_GetCardID_clicked();

	static int QueryPersonInfo(void*);

	void on_AddNewIDCard(IDCardInfo* pIDCard);

	//void on_comboBox_Province_currentIndexChanged(int index);

	//void on_comboBox_City_currentIndexChanged(int index);

	//void on_comboBox_County_currentIndexChanged(int index);

	//void on_comboBox_Town_currentIndexChanged(int index);

	void on_checkBox_Agency_stateChanged(int arg1);

	//void on_pushButton_TakePhoto_clicked();

	//void on_pushButton_SelectPhoto_clicked();

	void on_checkBox_Agency_clicked();

	// UI�༭�¼��ͳ���༭�¼����ᴥ��
	void on_Name_textChanged(const QString& arg1);
	void on_Name_EditingFinished();

	// ֻ��UI�༭�¼��ᴥ��
	void on_lineEdit_Name_textEdited(const QString& arg1);
	// ֻ��ӦUI�༭�¼�
	void on_lineEdit_CardID_textEdited(const QString& arg1);

private:
	Ui::qu_ModifyInfo* ui;
};

#endif // qu_ModifyInfo_H
