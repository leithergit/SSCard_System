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
#include <chrono>
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

	void mousePressEvent(QMouseEvent* e);

	void on_pushButton_Updatecard_clicked();

	void on_pushButton_ChangePWD_clicked();

	void on_pushButton_RegisterLost_clicked();

	void on_pushButton_MainPage_clicked();

	int m_nDateTimer = 0;
	virtual void timerEvent(QTimerEvent* event) override;
	int LoadConfigure(QString& strError);
	//virtual void paintEvent(QPaintEvent* event) override;

signals:
	void ShowMaskWidget(QString strTitle, QString strDesc, int nStatus, int nOperation);

private slots:
	void On_ShowMaskWidget(QString, QString, int nStatus, int nPageOperation);
	void On_MaskWidgetTimeout(int nOperation);

private:
	Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
