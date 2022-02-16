#pragma execution_character_set("utf-8")
#include "mainpage.h"
#include "ui_mainpage.h"
#include "mainwindow.h"

MainPage::MainPage(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::MainPage)
{
	ui->setupUi(this);
	//ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
	//this->setStyleSheet(QString::fromUtf8("#MainPage{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(26, 37, 223, 255), stop:1 rgba(3, 152, 252, 255));}"));
	//this->setStyleSheet(QString("#MainPage{background-color: rgb(255, 255, 255);}"));
	//ui->quickWidget->setSource(QUrl(QString::fromUtf8("./qml/SliderPlayer.qml")));

	const QList<std::pair<QString, QString>>&& imagepairlst = { std::make_pair("./image/Card1.png", QString::fromLocal8Bit("")),
																std::make_pair("./image/Card2.png", QString::fromLocal8Bit("")),
																std::make_pair("./image/Card3.png", QString::fromLocal8Bit("")) };

	ui->label_Image->addImage(imagepairlst);

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
	QString strNewCard = QString(strButtonStyle).arg("NewCard");
	QString strUpdateCard = QString(strButtonStyle).arg("UpdateCard_big");
	QString strRegisterLost = QString(strButtonStyle).arg("RegisterLost");
	QString strChangePassword = QString(strButtonStyle).arg("ChangePassword");

	//ui->pushButton_NewCard->setStyleSheet(strNewCard);
	//ui->pushButton_NewCard->hide();
	ui->pushButton_Updatecard->setStyleSheet(strUpdateCard);
	ui->pushButton_RegisterLost->setStyleSheet(strRegisterLost);
	ui->pushButton_ChangePWD->setStyleSheet(strChangePassword);

	if (LoadServiceDescription())
	{
		QString strStyle = QString("border-image: url(./Image/%1);").arg(m_ServiceDescription[0].strQR_Photo);
		ui->label_QR1->setStyleSheet(strStyle);
		ui->label_QR1_Service->setText(m_ServiceDescription[0].strService[0]);
		ui->label_QR1_Service_Text1->setText(m_ServiceDescription[0].strService[1]);
		ui->label_QR1_Service_Text2->setText(m_ServiceDescription[0].strService[2]);

		strStyle = QString("border-image: url(./Image/%1);").arg(m_ServiceDescription[1].strQR_Photo);
		ui->label_QR2->setStyleSheet(strStyle);
		ui->label_QR2_Service->setText(m_ServiceDescription[1].strService[0]);
		ui->label_QR2_Service_Text1->setText(m_ServiceDescription[1].strService[1]);
		ui->label_QR2_Service_Text2->setText(m_ServiceDescription[1].strService[2]);
	}
}

MainPage::~MainPage()
{
	delete ui;
}

void MainPage::on_pushButton_NewCard_clicked()
{
	g_pDataCenter->ResetIDData();
	((MainWindow*)m_pMainWindow)->on_pushButton_Updatecard_clicked();
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

bool MainPage::LoadServiceDescription()
{
	QString strAppPath = QCoreApplication::applicationDirPath();
	strAppPath += "/ServicesDescription.json";
	QFileInfo fi(strAppPath);
	QString strInfo;
	if (!fi.isFile())
	{
		strInfo = QString("找不到服务描述文件:%1").arg(strAppPath);
		gInfo() << gQStr(strInfo);
		return false;
	}
	QFile fileService(strAppPath);
	if (!fileService.open(QIODevice::ReadOnly))
	{
		strInfo = QString("打开到服务描述文件失败:%1").arg(strAppPath);
		gInfo() << gQStr(strInfo);
		return false;
	}

	QByteArray FileContent = fileService.readAll();

	QJsonParseError jsonError;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(FileContent, &jsonError));

	if (jsonError.error != QJsonParseError::NoError)
	{
		qDebug() << jsonError.errorString();
		return false;
	}

	QJsonObject rootObject = jsonDoc.object();
	/*
	{
		"Wechat1": [{
			"QR_Photo":"qr_wechat1.png",
			"FolowWechat": "关注周口农商银行",
			"SocialService": "提供查询社保服务",
			"PayService": "提供缴费通知服务"
		}],
		"Wechat2": [{
			"QR_Photo":"qr_wechat2.png",
			"FolowWechat": "关注周口农商银行",
			"SocialService": "提供查询社保服务",
			"PayService": "提供缴费通知服务"
		}]
	}
	*/
	if (rootObject.contains("Wechat1"))
	{
		QJsonObject ServiceObj = rootObject.value("Wechat1").toObject();
		m_ServiceDescription[0].strQR_Photo = ServiceObj["QR_Photo"].toString();
		m_ServiceDescription[0].strService[0] = ServiceObj["FolowWechat"].toString();
		m_ServiceDescription[0].strService[1] = ServiceObj["SocialService"].toString();
		m_ServiceDescription[0].strService[2] = ServiceObj["PayService"].toString();

	}
	if (rootObject.contains("Wechat2"))
	{
		QJsonObject ServiceObj = rootObject.value("Wechat2").toObject();
		m_ServiceDescription[1].strQR_Photo = ServiceObj["QR_Photo"].toString();
		m_ServiceDescription[1].strService[0] = ServiceObj["FolowWechat"].toString();
		m_ServiceDescription[1].strService[1] = ServiceObj["SocialService"].toString();
		m_ServiceDescription[1].strService[2] = ServiceObj["PayService"].toString();
	}

	return true;
}
