#ifndef UC_INPUTIDCARDINFO_H
#define UC_INPUTIDCARDINFO_H

#include <QWidget>
#include <QButtonGroup>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>

namespace Ui {
	class uc_InputIDCardInfo;
}

class uc_InputIDCardInfo : public QWidget
{
	Q_OBJECT

public:
	explicit uc_InputIDCardInfo(QWidget* parent = nullptr);
	~uc_InputIDCardInfo();
	int		GetCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage);
	int		GetSSCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage);
	void	ResetPage();
	QButtonGroup* pButtonGrp = nullptr;
	QSqlDatabase SQLiteDB;
	bool    InitializeDB(QString& strMessage);
	bool    bInitialized = false;
signals:
	void ShowNationWidgets(QWidget* pWidget, bool bShow);
protected:
	//virtual void showEvent(QShowEvent* event) override;
private slots:
	//void on_pushButton_Query_clicked();

	void on_pushButton_OK_clicked();

	//void on_lineEdit_Nation_clicked();

	void on_ShowWidgets(QWidget* pWidget, bool bShow);

	void on_comboBox_Province_currentIndexChanged(int index);

	void on_comboBox_City_currentIndexChanged(int index);

	void on_comboBox_County_currentIndexChanged(int index);

	void on_comboBox_Town_currentIndexChanged(int index);

private:
	Ui::uc_InputIDCardInfo* ui;
};

#endif // UC_INPUTIDCARDINFO_H
