#ifndef UC_MAKECARD_H
#define UC_MAKECARD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class MakeCard;
}

class uc_MakeCard : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_MakeCard(QLabel* pTitle, int nTimeout = 300, QWidget* parent = nullptr);
	~uc_MakeCard();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
private:
	Ui::MakeCard* ui;
};

#endif // UC_MAKECARD_H
