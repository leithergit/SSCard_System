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
	// 待完成
	int    ReadSSCardInfo(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);
	//int  QueryRegisterLost(QString& strMessage);
	int	   QuerySSCardStatus(QString& strMessage, SSCardInfoPtr& pSSCardInfo);
	// 待完成
	int    RegisterLost(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);
	int	   UnRegisterLost(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);
	bool   m_bTestStatus = false;       // 测试专用变量
	bool   m_bRegisterLost = false;

private slots:
	void on_pushButton_OK_clicked();

private:
	Ui::EnsureInformation* ui;
};

#endif // UC_ENSUREINFORMATION_H
