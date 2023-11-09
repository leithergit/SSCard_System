#ifndef SYS_CARDMANGER_H
#define SYS_CARDMANGER_H

#include <QWidget>
#include "ConfigurePage.h"

namespace Ui {
	class CardManger;
}

class CardManger : public QWidget
{
	Q_OBJECT

public:
	explicit CardManger(QWidget* parent = nullptr);
	~CardManger();
	bool Save(QString& strMessage);

private slots:
	void on_pushButton_clicked();

private:
	Ui::CardManger* ui;
};

#endif // SYS_CARDMANGER_H
