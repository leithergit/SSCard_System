#ifndef UC_ENSUREINFORMATION_H
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
	explicit uc_EnsureInformation(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~uc_EnsureInformation();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;

	bool   m_bTestStatus = false;       // 测试专用变量
	bool   m_bRegisterLost = false;
	virtual void ShutDown() override
	{
		gInfo() << __FUNCTION__;
	}

private slots:
	void on_pushButton_OK_clicked();

private:
	Ui::EnsureInformation* ui;
};

#endif // UC_ENSUREINFORMATION_H
