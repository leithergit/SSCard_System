#ifndef SYS_SSCARDAPITEST_H
#define SYS_SSCARDAPITEST_H

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
	class Sys_SSCardAPITest;
}

class Sys_SSCardAPITest : public QWidget
{
	Q_OBJECT

public:
	explicit Sys_SSCardAPITest(QWidget* parent = nullptr);
	~Sys_SSCardAPITest();
	bool LoadPersonInfo(QString strJson);

	void SavePersonInfo();

	void ResetPage();

	bool GetPersonName(QString strPersonFile, QString& strName);

	bool eventFilter(QObject* watched, QEvent* event) override;

	void ClearInfo();
	int	GetSSCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage);

	int GetCardID(string& strCardID, QString strMessage);

	int GetCardInfo(/*IDCardInfoPtr& pIDCard,*/ QString& strMessage);

	QButtonGroup* pButtonGrp = nullptr;
	Sys_DialogReadIDCard* pDialogReadIDCard = nullptr;
	IDCardInfoPtr pIDCard = nullptr;
	SSCardInfoPtr pSSCardInfo = nullptr;
	QByteArray baPhoto;
	QCompleter* pCompleter = nullptr;
	map<QString, QString> mapPersonFile;

signals:
	void AddNewIDCard(IDCardInfo* IDCard);
private slots:
	void on_AddNewIDCard(IDCardInfo* pIDCard);

	void on_Name_textChanged(const QString& arg1);

	void on_comboBox_PersonType_currentIndexChanged(int index);

	void on_pushButton_TakePhoto_clicked();

	void on_pushButton_SelectPhoto_clicked();

	void on_pushButton_OK_clicked();

	void on_pushButton_ResetPage_clicked();

	void on_pushButton_Excecute_clicked();

private:
	Ui::Sys_SSCardAPITest* ui;
};

#endif // SYS_SSCARDAPITEST_H
