#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QWidget>
#include "ConfigurePage.h"

namespace Ui {
	class logManager;
}

class logManager : public QWidget
{
	Q_OBJECT

public:
	explicit logManager(QWidget* parent = nullptr);
	~logManager();
	bool Save(QString& strMessage) { return true; }

private slots:
	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

private:
	Ui::logManager* ui;
};

#endif // LOGMANAGER_H
