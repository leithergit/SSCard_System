﻿#ifndef UC_ENSUREINFORMATION_H
#define UC_ENSUREINFORMATION_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class EnsureInformation;
}

class uc_EnsureInformation : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_EnsureInformation(QLabel* pTitle, int nTimeout = 30, QWidget* parent = nullptr);
	~uc_EnsureInformation();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
    // 待完成
    int    ReadSSCardInfo(QString &strMessage);
    int    QueryRegisterLost(QString &strMessage);
    // 待完成
    int    RegisterLost(QString &strMessage);
    bool   m_bTestStatus = false;
private slots:
    void on_pushButton_OK_clicked();

private:
	Ui::EnsureInformation* ui;
};

#endif // UC_ENSUREINFORMATION_H