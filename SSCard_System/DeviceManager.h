#pragma once

#include <QWidget>
#include "ui_DeviceManager.h"

class DeviceManager : public QWidget
{
	Q_OBJECT

public:
	DeviceManager(QWidget *parent = Q_NULLPTR);
	~DeviceManager();

private:
	Ui::DeviceManager ui;
};
