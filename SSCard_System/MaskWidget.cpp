#pragma execution_character_set("utf-8")
#include "MaskWidget.h"
#include "ui_MaskWidget.h"
#include <qdesktopwidget.h>
#include <QScreen>
#include<QDebug>
#include "Gloabal.h"
MaskWidget::MaskWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::MaskWidget)
{
	ui->setupUi(this);
	//ui->pushButton_OK->hide();
	hide();
}

MaskWidget::~MaskWidget()
{
	delete ui;
}

void MaskWidget::Popup(QString strTitle, QString strDesc, int nStatus, int nPageOpteration, int nTimeout)
{
	gInfo() << gQStr(strTitle) << gQStr(strDesc) << gVal(nStatus) << gVal(nPageOpteration) << gVal(nTimeout);
	QString strImage;
	setWindowOpacity(0.8);
	m_nPageOpteration = nPageOpteration;
	QString strQSS;
	switch (nStatus)    // 设置相应图标
	{
	case Success:
		strQSS = QString("color: #20b759;font: 57 42px \"思源黑体 CN Medium\";font-weight: normal;line-height: 49px;letter-spacing: 1px;");
		strImage = "Success.png";
		break;
	default:
	case Information:
		strQSS = QString("color: #20b759;font: 57 42px \"思源黑体 CN Medium\";font-weight: normal;line-height: 49px;letter-spacing: 1px;");
		strImage = "Success.png";
		break;
	case Error:
		strQSS = QString("color: #c12a46;font: 57 42px \"思源黑体 CN Medium\";font-weight: normal;line-height: 49px;letter-spacing: 1px;");
		ui->label_Title->setStyleSheet(strQSS);
		strImage = "failed.png";
	case Failed:
		strQSS = QString("color: #c12a46;font: 57 42px \"思源黑体 CN Medium\";font-weight: normal;line-height: 49px;letter-spacing: 1px;");
		strImage = "failed.png";
		break;
	case Fetal:
		strQSS = QString("color: #c12a46;font: 57 42px \"思源黑体 CN Medium\";font-weight: normal;line-height: 49px;letter-spacing: 1px;");
		strImage = "exclamation.png";
		break;
	case Nop:
		break;
	}

	//   以下代码为下设置全屏遮罩，暂弃用
	//    QPoint pos = QWidget::mapToGlobal(this->pos()); //获取窗口在桌面上的绝对位置
	//    qDebug("Pos = (%d,%d)",pos.x(),pos.y());
	//    QScreen *pScreen  = QGuiApplication::screenAt(pos);
	//    if (!pScreen)
	//    {
	//        qDebug()<<"Get Screen Failed.";
	//    }
	//    else
	//    {
	//        qDebug()<<"Get Screen Succeed.";
	//    }
	//    this->setGeometry(pScreen->geometry());
	QString strAppPath = QCoreApplication::applicationDirPath();
	QString strFullImagePath = QString("%1/image/%2").arg(strAppPath).arg(strImage);
	qDebug() << "strImagePath = " << strFullImagePath;
	QFileInfo fi(strFullImagePath);
	if (!fi.isFile())
	{
		qDebug() << strFullImagePath << "not exist!";
	}
	QString strMaskImage = QString("border-image: url(%1);").arg(strFullImagePath);
	gInfo() << gQStr(strMaskImage);
	ui->label_Image->setStyleSheet(strMaskImage);
	setWindowFlags((Qt::WindowFlags)(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowActive));
	show();
	setFocus();

	if (nPageOpteration != Retry_CurrentPage)
	{
		m_nTimeout = nTimeout;
		m_nTimerID = startTimer(m_nTimerInterval);

	}
	if (m_nTimeout <= 5000)
	{
		ui->pushButton_OK->hide();
		ui->pushButton_OK->setText("确定");
	}
	else
	{
		QString strText = QString("确定(%1)").arg(m_nTimeout / 1000);
		ui->pushButton_OK->setText(strText);
		ui->pushButton_OK->show();
	}

	ui->label_Title->setText(strTitle);
	ui->label_Title->setStyleSheet(strQSS);
	ui->label_Desc->setText(strDesc);

}

void MaskWidget::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == m_nTimerID)
	{
		m_nTimeout -= m_nTimerInterval;
		QString strText = QString("确定(%1)").arg(m_nTimeout / 1000);
		ui->pushButton_OK->setText(strText);
		if (m_nTimeout <= 0)
		{
			hide();
			killTimer(m_nTimerID);
			m_nTimerID = 0;
			emit MaskTimeout(m_nPageOpteration);
		}
	}
}

void MaskWidget::on_pushButton_OK_clicked()
{
	hide();
	killTimer(m_nTimerID);
	m_nTimerID = 0;
	emit MaskEnsure(m_nPageOpteration, 0);
}
