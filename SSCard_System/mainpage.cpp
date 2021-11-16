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

	ui->quickWidget->setSource(QUrl(QString::fromUtf8("./qml/SliderPlayer.qml")));
	foreach(QWidget * w, qApp->topLevelWidgets())
	{
		if ((w = qobject_cast<QMainWindow*>(w)))
			m_pMainWindow = w;
	}
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
	((MainWindow*)m_pMainWindow)->on_pushButton_Updatecard_clicked();
}

void MainPage::on_pushButton_ChangePWD_clicked()
{
	((MainWindow*)m_pMainWindow)->on_pushButton_ChangePWD_clicked();
}

void MainPage::on_pushButton_RegisterLost_clicked()
{
	((MainWindow*)m_pMainWindow)->on_pushButton_RegisterLost_clicked();
}
