#ifndef UC_READIDCARD_H
#define UC_READIDCARD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <memory>
#include "qstackpage.h"
#include "../SDK/IDCard/idcard_api.h"
//#include "uc_inputidcardinfo.h"

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
	virtual void  ShutDown() override;
	virtual void timerEvent(QTimerEvent* event) override;

	void StartDetect();
	void StopDetect();

	int     m_nDelayCount = 0;
	int     m_bSucceed = false;
	//int     GetIDImageStorePath(string &strFilePath);
	//int     ReaderIDCard();
	void    ThreadWork();
	bool	m_bAgency = false;
	string  m_strDevPort;
	ushort  m_nBaudreate = 9600;
	ReadID_Type  nReadIDType = ReadID_UpdateCard;
	IDCardInfoPtr m_pIDCard = nullptr;
	//uc_InputIDCardInfo* pInputIDCardWidget = nullptr;
	//QNationWidget* pQNationWidget = nullptr;
	//void RemoveUI();
	//void ShowReadCardID();
	//void ShowInputCardID();

signals:
	void ShowNationWidget(bool bShow);
private slots:
	void OnErrorMessage(QString strErrorMsg);
	void On_WithoutIDCard(int arg1);

private:
	Ui::ReadIDCard* ui;
};

#endif // UC_READIDCARD_H
