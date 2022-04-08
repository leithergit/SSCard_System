#ifndef QUERYINFO_H
#define QUERYINFO_H

#include <QWidget>
#include "qmainstackpage.h"
#include "qstackpage.h"
#include "QStackedWidget"
namespace Ui {
	class QueryInfo;
}

class QueryInfo : public QMainStackPage
{
	Q_OBJECT

public:
	explicit QueryInfo(QWidget* parent = nullptr);
	~QueryInfo();
	virtual void OnTimerEvent() override;
	virtual void  ShowTimeOut(int nTimeout) override;

private:
	Ui::QueryInfo* ui;
};

#endif // QUERYINFO_H
