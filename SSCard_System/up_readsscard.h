#ifndef UP_READSSCARD_H
#define UP_READSSCARD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class ReadSSCard;
}

class up_ReadSSCard : public QStackPage
{
	Q_OBJECT

public:
	explicit up_ReadSSCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~up_ReadSSCard();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
private:
	Ui::ReadSSCard* ui;
};

#endif // UP_READSSCARD_H
