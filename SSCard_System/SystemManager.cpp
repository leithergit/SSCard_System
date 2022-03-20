#pragma execution_character_set("utf-8")
#include "SystemManager.h"
#include "Sys_DeviceManager.h"
#include "uc_inputmobile.h"
#include "Sys_pageconfigure.h"
#include "Sys_regionconfigure.h"
#include "Sys_otheroptions.h"
#include "Sys_cardmanger.h"
#include "Sys_logmanager.h"
#include "sys_manualmakecard.h"
#include <QMessageBox>
#include "Gloabal.h"
#include <thread>
#include "mainwindow.h"
#include "Sys_sscardservicet.h"

SystemManager::SystemManager(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setStyleSheet(QString::fromUtf8("#SystemManager{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(26, 37, 223, 255), stop:1 rgba(3, 152, 252, 255));}"));
	ui.tabWidget->addTab(new DeviceManager, "设备管理");
	ui.tabWidget->addTab(new Sys_ManualMakeCard, "手动制卡");
	ui.tabWidget->addTab(new RegionConfigure, "区域设置");
	//ui.tabWidget->addTab(new PageConfigure, "页面设置");
	//ui.tabWidget->addTab(new CardManger, "卡片管理");
	ui.tabWidget->addTab(new logManager, "日志管理");
	ui.tabWidget->addTab(new SSCardServiceT, "社保功能测试");
	ui.tabWidget->addTab(new OtherOptions, "其它选项");
	showFullScreen();
}

SystemManager::~SystemManager()
{
}

void SystemManager::on_pushButton_Exit_clicked()
{
	if (QMessageBox_CN(QMessageBox::Warning, "警告", "该操作将会关闭社保卡制卡系统,是否继续?", QMessageBox::Yes | QMessageBox::No, this) == QMessageBox::Yes)
	{
		QString strInfo = "退出制卡系统!";
		gInfo() << gQStr(strInfo);
		std::this_thread::sleep_for(chrono::milliseconds(200));
		((MainWindow*)parent())->emit Shutdown();
		//this->deleteLater();
	}
}

void SystemManager::on_pushButton_Save_clicked()
{

}

void SystemManager::on_pushButton_Close_clicked()
{
	QDialog::accept();
}
