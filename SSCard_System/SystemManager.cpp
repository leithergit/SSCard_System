#include "SystemManager.h"
#include "SystemConfigure.h"
#include "DeviceManager.h"

SystemManager::SystemManager(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setStyleSheet(QString::fromUtf8("#SystemManager{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(26, 37, 223, 255), stop:1 rgba(3, 152, 252, 255));}"));

}

SystemManager::~SystemManager()
{
}

void SystemManager::on_pushButton_Exit_clicked()
{
	exit(0);
}


void SystemManager::on_pushButton_Save_clicked()
{

}
