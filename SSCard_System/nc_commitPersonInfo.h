#ifndef NC_COMMITPERSONINFO_H
#define NC_COMMITPERSONINFO_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class nc_commitPersonInfo;
}

class nc_commitPersonInfo : public QStackPage
{
	Q_OBJECT

public:
	explicit nc_commitPersonInfo(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~nc_commitPersonInfo();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	virtual void ShutDown() override
	{
		gInfo() << __FUNCTION__;
	}

private slots:
	void on_pushButton_OK_clicked();

private:
	Ui::nc_commitPersonInfo* ui;
};

#endif // NC_COMMITPERSONINFO_H
