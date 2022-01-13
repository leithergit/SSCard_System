#ifndef UC_INPUTMOBILE_H
#define UC_INPUTMOBILE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

#define MaxMobileSize   11

namespace Ui {
	class InputMobile;
}

class uc_InputMobile : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_InputMobile(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~uc_InputMobile();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	QString  m_strMobile = "";
	int      m_nMobilePhoneSize = 11;
	virtual void ShutDown() override
	{
		gInfo() << __FUNCTION__;
	}
private slots:
	void on_pushButton_0_clicked();
	void on_pushButton_1_clicked();
	void on_pushButton_2_clicked();
	void on_pushButton_3_clicked();
	void on_pushButton_4_clicked();
	void on_pushButton_5_clicked();
	void on_pushButton_6_clicked();
	void on_pushButton_7_clicked();
	void on_pushButton_8_clicked();
	void on_pushButton_9_clicked();
	void on_pushButton_Clear_clicked();
	void on_pushButton_Backspace_clicked();
	void on_pushButton_OK_clicked();
	int     QueryPayResult(QString& strMessage, int& nResult);

private:
	Ui::InputMobile* ui;
};

#endif // UC_INPUTMOBILE_H
