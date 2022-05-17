#pragma once

#include <QDialog>
#include "ui_CheckPassword.h"

class CheckPassword : public QDialog
{
	Q_OBJECT

public:
	CheckPassword(QWidget *parent = Q_NULLPTR);
	~CheckPassword();
    QString strPassword;

private slots:

    void on_pushButton_OK_clicked();

    void on_pushButton_Cancel_clicked();

    void on_pushButton_T_clicked();

private:
	Ui::CheckPassword ui;
};
