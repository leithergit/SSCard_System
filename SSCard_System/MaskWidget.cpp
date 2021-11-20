#pragma execution_character_set("utf-8")
#include "MaskWidget.h"
#include "ui_MaskWidget.h"
#include <qdesktopwidget.h>
#include <QScreen>
#include<QDebug>

MaskWidget::MaskWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::MaskWidget)
{
	ui->setupUi(this);

	hide();
}

MaskWidget::~MaskWidget()
{
	delete ui;
}

void MaskWidget::Popup(QString strTitle, int nStatus, int nPage, int nTimeout)
{
	setWindowOpacity(0.8);
    switch (nStatus)    // 设置相应图标
	{
    case Success:
        //ui->label_Image->setPixmap();
        break;
    default:
    case Information:
		break;
	case Error:
		break;
	case Failed:
		break;
	case Fetal:
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
    setWindowFlags((Qt::WindowFlags)(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowActive));
    show();
    setFocus();
    m_nTimeout = nTimeout;
	m_nTimerID = startTimer(1000);

	ui->label_FailedText->setText(strTitle);

}

void MaskWidget::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == m_nTimerID)
	{
		m_nTimeout--;
		if (m_nTimeout <= 0)
		{
			killTimer(m_nTimerID);
			m_nTimerID = 0;
            emit MaskTimeout(m_nOperatorPage);
		}
	}
}
