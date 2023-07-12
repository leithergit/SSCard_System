#ifndef UC_INPUTIDCARDINFO_H
#define UC_INPUTIDCARDINFO_H

#include <QWidget>
#include <QButtonGroup>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>
#include <QButtonGroup>
#include "qstackpage.h"
#include "sys_dialogreadidcard.h"
#include "Gloabal.h"
#include <QCompleter>

namespace Ui {
	class uc_InputIDCardInfo;
}

class uc_InputIDCardInfo :public QStackPage
{
	Q_OBJECT

public:
	explicit uc_InputIDCardInfo(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~uc_InputIDCardInfo();
	virtual int ProcessBussiness() override;

	virtual void OnTimeout() override;

	bool  InitializeDB(QString& strMessage);

	bool LoadPersonInfo(QString strJson);

	void SavePersonInfo();

	void  ShutDown() override;

	void timerEvent(QTimerEvent* event) override;

	void ResetPage();

	int	GetSSCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage);

	int GetCardInfo(/*IDCardInfoPtr& pIDCard,*/ QString& strMessage);

	void StartDetect();

	void ThreadWork();

	void StopDetect();

	bool GetPersonName(QString strPersonFile, QString& strName);

	bool eventFilter(QObject* watched, QEvent* event) override;

	void ClearInfo();

	QButtonGroup* pButtonGrp = nullptr;
	Sys_DialogReadIDCard* pDialogReadIDCard = nullptr;
	IDCardInfoPtr pIDCard = nullptr;
	SSCardInfoPtr pSSCardInfo = nullptr;
	QByteArray baPhoto;
	QCompleter* pCompleter = nullptr;
	map<QString, QString> mapPersonFile;

signals:
	void AddNewIDCard(IDCardInfo* IDCard);
public slots:
	void on_AddNewIDCard(IDCardInfo* pIDCard);

	void on_pushButton_TakePhoto_clicked();

	void on_pushButton_SelectPhoto_clicked();

	void on_pushButton_OK_clicked();

	void on_comboBox_PersonType_currentIndexChanged(int index);

	// UI编辑事件和程序编辑事件都会触发
	void on_Name_textChanged(const QString& arg1);
	
	void on_lineEdit_CardID_textChanged(const QString &arg1);

	// 只有UI编辑事件会触发
	// void on_lineEdit_Name_textEdited(const QString& arg1);
	

private:
	Ui::uc_InputIDCardInfo* ui;
};

#endif // UC_INPUTIDCARDINFO_H
