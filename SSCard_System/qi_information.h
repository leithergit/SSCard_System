#ifndef QI_INFORMATION_H
#define QI_INFORMATION_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class qi_Information;
}

class qi_Information : public QStackPage
{
	Q_OBJECT

public:
	explicit qi_Information(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~qi_Information();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	virtual void ShutDown() override;
    void ResetPage();

private slots:
    void on_pushButton_OK_clicked();

    void on_pushButton_ModifyInfo_clicked();

private:
	Ui::qi_Information* ui;
};

#endif // QI_INFORMATION_H
