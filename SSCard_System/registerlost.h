#ifndef REGISTERLOST_H
#define REGISTERLOST_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qmainstackpage.h"
#include "qstackpage.h"
#include <QStackedWidget>

enum    RegisgerLost_Widget
{
	_RL_ReadIDCard = 0,
	_EnsureRegisterlog = 1
};

namespace Ui {
	class RegisterLost;
}

class RegisterLost : public QMainStackPage
{
	Q_OBJECT

public:
	explicit RegisterLost(QWidget* parent = nullptr);
	~RegisterLost();

	virtual void OnTimerEvent() override;
	virtual void  SetTimeOut(int nTimeout) override;
private slots:


private:
	Ui::RegisterLost* ui;
};

#endif // REGISTERLOST_H
