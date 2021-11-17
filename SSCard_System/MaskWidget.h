#ifndef MASKWIDGET_H
#define MASKWIDGET_H
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QString>

namespace Ui {
	class MaskWidget;
}
#include "qstackpage.h"

class MaskWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MaskWidget(QWidget* parent = nullptr);
	~MaskWidget();
	virtual void timerEvent(QTimerEvent* event);
    void Popup(QString strTitle,MaskStatus nStatus,PageOperation nPage,int nTimeout);
signals:
    void MaskTimeout();
private:
	QString m_strFailedText = "";
	int m_nTimerID = 0;
	int m_nTimeout = 0;
    Ui::MaskWidget* ui;
};

#endif // MASKWIDGET_H
