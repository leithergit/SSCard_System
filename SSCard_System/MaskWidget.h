﻿#ifndef MASKWIDGET_H
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
	void Popup(QString strTitle, QString strDesc, int nStatus, int nOpteration, int nPage, int nTimeout);
signals:
	void MaskTimeout(int nOperation, int nPage);
	void MaskEnsure(int nOperation, int nPage);
private slots:
	void on_pushButton_OK_clicked();

private:
	QString m_strFailedText = "";
	int m_nTimerID = 0;
	int m_nTimeout = 0;
	Ui::MaskWidget* ui;
	int nOperation = 0;
	int nPage = 0;
	int m_nTimerInterval = 1000;	//ms
};

#endif // MASKWIDGET_H
