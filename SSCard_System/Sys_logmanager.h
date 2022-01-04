#ifndef SYS_LOGMANAGER_H
#define SYS_LOGMANAGER_H

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
	void Showlog(QStringList& strLogList);
	~logManager();
	bool Save(QString& strMessage) { return true; }

private slots:
	void on_pushButton_Search_clicked();

	void on_pushButton_Export_clicked();

	void on_pushButton_Del_clicked();

private:
	Ui::logManager* ui;
};

#endif // SYS_LOGMANAGER_H
