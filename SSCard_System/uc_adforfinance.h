#ifndef UC_ADFORFINANCE_H
#define UC_ADFORFINANCE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class AdforFinance;
}

class uc_AdforFinance : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_AdforFinance(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~uc_AdforFinance();
	virtual void OnTimeout() override;
	virtual void ShutDown() override
	{
		gInfo() << __FUNCTION__;
	}
private:
	Ui::AdforFinance* ui;
};

#endif // UC_ADFORFINANCE_H
