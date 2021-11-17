#ifndef MAINWINDOW_H
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
//#include "operatorfailed.h"

#include "Gloabal.h"


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
	UpdateCard* m_pUpdateCard = nullptr;
	UpdatePassword* m_pUpdatePassword = nullptr;
	RegisterLost* m_pRegiserLost = nullptr;
    MaskWidget* m_pMaskWindow = nullptr;

	void on_pushButton_Updatecard_clicked();

	void on_pushButton_ChangePWD_clicked();

	void on_pushButton_RegisterLost_clicked();

	void on_pushButton_MainPage_clicked();
	int m_nDateTimer = 0;
	virtual void timerEvent(QTimerEvent* event);

	int LoadConfigure(QString& strError);
signals:
    void ShowMaskWidget(QString strMessage,MaskStatus nStatus,PageOperation nOperation);

private slots:
    void On_ShowMaskWidget(QString,MaskStatus nStatus,PageOperation nPage);
    void On_MaskWidgetTimeout();

private:
	Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
