#pragma once

#include <QDialog>
#include "ui_SystemManager.h"

class SystemManager : public QDialog
{
	Q_OBJECT

public:
	SystemManager(QWidget *parent = Q_NULLPTR);
	~SystemManager();

private slots:
    void on_pushButton_Exit_clicked();

    void on_pushButton_Save_clicked();

private:
	Ui::SystemManager ui;
};
