#ifndef ADMINISTER_H
#define ADMINISTER_H

#include <QWidget>
#include "Gloabal.h"

namespace Ui {
	class Administer;
}

class Administer : public QWidget
{
	Q_OBJECT

public:
	explicit Administer(QWidget* parent = nullptr);
	~Administer();
	bool	LoadConfigure();
	void	SaveConfigure();
	void    ThreadReadIDCard();
	volatile bool    m_bWorkThreadRunning = false;
	std::thread* m_pWorkThread = nullptr;
private slots:

	void on_pushButton_Add_clicked();

	void on_pushButton_Del_clicked();

	void on_AddNewIDCard(IDCardInfo* pIDCard);

signals:
	void AddNewIDCard(IDCardInfo* IDCard);

private:
	Ui::Administer* ui;
};

#endif // ADMINISTER_H
