#pragma once

#include <QWidget>
#include "ui_SystemConfigure.h"

class SystemConfigure : public QWidget
{
	Q_OBJECT

public:
	SystemConfigure(QWidget *parent = Q_NULLPTR);
	~SystemConfigure();

private:
	Ui::SystemConfigure ui;
};
