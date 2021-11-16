#ifndef OPERATORSUCCEED_H
#define OPERATORSUCCEED_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
namespace Ui {
	class OperatorSucceed;
}

class OperatorSucceed : public QStackPage
{
	Q_OBJECT

public:
	explicit OperatorSucceed(QLabel* pTitle, int nTimeout = 10, QWidget* parent = nullptr);
	~OperatorSucceed();
	virtual void OnTimeout() override;
private:
	Ui::OperatorSucceed* ui;
};

#endif // OPERATORSUCCEED_H
