﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma execution_character_set("utf-8")
#include <QMainWindow>
#include <QDebug>
#include <QDateTime>
#include "mainpage.h"
#include "updatecard.h"
#include "updatepassword.h"
#include "registerlost.h"
#include "MaskWidget.h"
#include "newcard.h"
#include <chrono>
//#include "operatorfailed.h"

#include "Gloabal.h"
using namespace std;
using namespace chrono;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
	MainPage* m_pMainpage = nullptr;
	NewCard* m_pNewCard = nullptr;
	UpdateCard* m_pUpdateCard = nullptr;
	UpdatePassword* m_pUpdatePassword = nullptr;
	RegisterLost* m_pRegiserLost = nullptr;
	MaskWidget* m_pMaskWindow = nullptr;
	QMainStackPage* pLastStackPage = nullptr;
	high_resolution_clock::time_point	m_tLastPress = high_resolution_clock::now();
	int		m_nContinuePressCount = 0;

	void mousePressEvent(QMouseEvent* e);

	void on_pushButton_NewCard_clicked();

	void on_pushButton_Updatecard_clicked();

	void on_pushButton_ChangePWD_clicked();

	void on_pushButton_RegisterLost_clicked();

	void on_pushButton_MainPage_clicked();

	void On_LoadSystemManager();

	void UpdateRibbonStatus(int nRibbonStatus);

	int m_nDateTimer = 0;
	int m_nTimerTestHost = 0;
	int m_nTimerRibbonStatus = 0;
	uint32_t nRibbonStatusWarningCount = 0;
	int m_nNetworkFailedCount = 0;
	int m_nTimerNetWarning = 0;
	bool bDisconnect = false;
	RibbonStatus nRibbonStatus = Ribbon_Full;
	std::thread* pThreadAsync = nullptr;
	std::chrono::steady_clock::time_point tLastTick;
	virtual void timerEvent(QTimerEvent* event) override;
	virtual void closeEvent(QCloseEvent* event) override;
	int LoadConfigure(QString& strError);
	//virtual void paintEvent(QPaintEvent* event) override;

// 	void fnThreadUploadlog();
// 	std::thread ThreadUploadlog;
// 	bool  bThreadUploadlogRunning = false;

	void ThreadUpdateLauncher();
	std::thread* pThreadUpdateLauncher = nullptr;
	bool bThreadUpdateLauncherRunning = false;

	void SwitchPage(int nOperation);

signals:
	void ShowMaskWidget(QString strTitle, QString strDesc, int nStatus, int nOperation, int nPage = 0);
	void LoadSystemManager();
	void Shutdown();

public slots:
	void On_ShowMaskWidget(QString, QString, int nStatus, int nOperation, int nPage);
	void On_MaskWidgetTimeout(int nOperation, int nStatus);
	void On_MaskWidgetEnsure(int nOperation, int nStatus);
	void on_Shutdown();

private:
	Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
