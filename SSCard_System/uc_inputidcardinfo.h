#ifndef UC_INPUTIDCARDINFO_H
#define UC_INPUTIDCARDINFO_H

#include <QWidget>

namespace Ui {
	class uc_InputIDCardInfo;
}

class uc_InputIDCardInfo : public QWidget
{
	Q_OBJECT

public:
	explicit uc_InputIDCardInfo(QWidget* parent = nullptr);
	~uc_InputIDCardInfo();
	QWidget* pNationWidget = nullptr;
	int		TestInput(QString& strMessage);
signals:
	void ShowNationWidgets(QWidget* pWidget, bool bShow);
private slots:
	void on_pushButton_Query_clicked();

	void on_pushButton_OK_clicked();

	//void on_lineEdit_Nation_clicked();

	void on_ShowWidgets(QWidget* pWidget, bool bShow);

private:
	Ui::uc_InputIDCardInfo* ui;
};

#endif // UC_INPUTIDCARDINFO_H
