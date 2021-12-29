#pragma once

#include <QDialog>
#include "Gloabal.h"
#include "ui_SystemManager.h"

class SystemManager : public QDialog
{
	Q_OBJECT

public:
	SystemManager(QWidget* parent = Q_NULLPTR);
	~SystemManager();
	Manager_Level m_nManagerType = Manager_Level::Manager;

private slots:
	void on_pushButton_Exit_clicked();

	void on_pushButton_Save_clicked();

	void on_pushButton_Close_clicked();

private:
	Ui::SystemManager ui;
};
