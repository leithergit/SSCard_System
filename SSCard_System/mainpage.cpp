#pragma execution_character_set("utf-8")
#include "mainpage.h"
#include "ui_mainpage.h"
#include "mainwindow.h"

MainPage::MainPage(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::MainPage)
{
	ui->setupUi(this);
	ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
	//this->setStyleSheet(QString::fromUtf8("#MainPage{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(26, 37, 223, 255), stop:1 rgba(3, 152, 252, 255));}"));
	//this->setStyleSheet(QString("#MainPage{background-color: rgb(255, 255, 255);}"));
	//
	ui->quickWidget->setSource(QUrl(QString::fromUtf8("./qml/SliderPlayer.qml")));
	foreach(QWidget * w, qApp->topLevelWidgets())
	{
		if ((w = qobject_cast<QMainWindow*>(w)))
			m_pMainWindow = w;
	}
	QString strButtonStyle = "QPushButton{border-radius: 31px;"
		"background-color: transparent;	"
		"border-image: url(./Image/%1.png);}"
		"QPushButton:hover{border-image: url(./Image/%1_Hover.png);}"
		"QPushButton:pressed{border-image: url(./Image/%1_Pressed.png);}";
	QString strUpdateCard = QString(strButtonStyle).arg("UpdateCard");
	QString strRegisterLost = QString(strButtonStyle).arg("RegisterLost");
	QString strChangePassword = QString(strButtonStyle).arg("ChangePassword");

	ui->pushButton_Updatecard->setStyleSheet(strUpdateCard);
	ui->pushButton_RegisterLost->setStyleSheet(strRegisterLost);
	ui->pushButton_ChangePWD->setStyleSheet(strChangePassword);
	//    QUrl source("D:/Work/Henan_shangqiu/HNBXZM/SliderPlayer.qml");
	//    //ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
	//    ui->quickWidget->setSource(source);
	//    ui->quickWidget->setClearColor(QColor(Qt::transparent));
}

MainPage::~MainPage()
{
	delete ui;
}

void MainPage::on_pushButton_Updatecard_clicked()
{
	g_pDataCenter->ResetIDData();
	((MainWindow*)m_pMainWindow)->on_pushButton_Updatecard_clicked();
}

void MainPage::on_pushButton_ChangePWD_clicked()
{
	g_pDataCenter->ResetIDData();
	((MainWindow*)m_pMainWindow)->on_pushButton_ChangePWD_clicked();
}

void MainPage::on_pushButton_RegisterLost_clicked()
{
	g_pDataCenter->ResetIDData();
	((MainWindow*)m_pMainWindow)->on_pushButton_RegisterLost_clicked();
}
