#ifndef UC_READIDCARD_H
#define UC_READIDCARD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <memory>
#include "qstackpage.h"
#include "../SDK/IDCard/idcard_api.h"

namespace Ui {
	class ReadIDCard;
}

enum ReadID_Type
{
	ReadID_UpdateCard = 0,
	ReadID_RegisterLost = 1
};

class uc_ReadIDCard : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_ReadIDCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, ReadID_Type nFunction = ReadID_UpdateCard, QWidget* parent = nullptr);
	~uc_ReadIDCard();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	//int     m_nTimerReaderIDCard = 0;
	virtual void timerEvent(QTimerEvent* event) override;
	int     m_nDelayCount = 0;
	int     m_bSucceed = false;
	/*    int     GetIDImageStorePath(string &strFilePath);*/
	int     ReaderIDCard();
	void    ThreadWork();
	virtual void  ShutDown() override;
	string  m_strDevPort;
	ushort  m_nBaudreate = 9600;
	ReadID_Type  nReadIDType = ReadID_UpdateCard;

	IDCardInfoPtr m_pIDCard = nullptr;
private slots:
	void OnErrorMessage(QString strErrorMsg);
private:
	Ui::ReadIDCard* ui;
};

#endif // UC_READIDCARD_H
