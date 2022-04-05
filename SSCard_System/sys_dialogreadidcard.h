#ifndef SYS_DIALOGREADIDCARD_H
#define SYS_DIALOGREADIDCARD_H

#include <QDialog>

namespace Ui {
	class Sys_DialogReadIDCard;
}

class Sys_DialogReadIDCard : public QDialog
{
	Q_OBJECT

public:
	explicit Sys_DialogReadIDCard(QString strMessage, bool bBatchMode = false, QWidget* parent = nullptr);
	~Sys_DialogReadIDCard();

private slots:
	void on_pushButton_Stop_clicked();

private:
	Ui::Sys_DialogReadIDCard* ui;
};

#endif // SYS_DIALOGREADIDCARD_H
