#ifndef NC_COMMITPERSONINFO_H
#define NC_COMMITPERSONINFO_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class nc_commitPersonInfo_Sandong;
}

class nc_commitPersonInfo_Sandong : public QStackPage
{
	Q_OBJECT

public:
	explicit nc_commitPersonInfo_Sandong(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~nc_commitPersonInfo_Sandong();

private slots:
	void on_pushButton_OK_clicked();

private:
	Ui::nc_commitPersonInfo_Sandong* ui;
};

#endif // NC_COMMITPERSONINFO_H
