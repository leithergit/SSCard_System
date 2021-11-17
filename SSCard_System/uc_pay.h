#ifndef UC_PAY_H
#define UC_PAY_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class Pay;
}

class uc_Pay : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_Pay(QLabel* pTitle, int nTimeout = 300, QWidget* parent = nullptr);
	~uc_Pay();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
    void    ShutDownDevice();
    int     Pay(QString &strError);
    void    ThreadWork();

private:
	Ui::Pay* ui;
};

#endif // UC_PAY_H
