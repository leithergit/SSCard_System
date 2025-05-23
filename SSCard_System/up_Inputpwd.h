﻿#ifndef UP_INPUTPWD_H
#define UP_INPUTPWD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
#include "qpinkeybroad.h"
#include "DevBase.h"

namespace Ui {
	class InputPWD;
}

class up_InputPWD : public QStackPage
{
	Q_OBJECT

public:
	explicit up_InputPWD(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~up_InputPWD();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;

	void    ThreadWork();
	virtual void  ShutDown() override;
	void    ClearPassword();

	uchar   m_szPin[32];
	int     m_nPinSize = 0;
	int     m_nSSCardPWDSize = 6;
	QString  m_strDevPort;
	ushort  m_nBaudreate = 9600;
	QPinKeybroadPtr m_pPinKeybroad = nullptr;
	int     CheckPassword(QString& strError);
	//int		OpenSSCardReader(QString strLib, ReaderBrand nReaderType, QString& strMesssage);
	//void	CloseSSCardReader();
	//KT_ReaderLibPtr	m_pReaderLib = nullptr;
	//KT_Reader* m_pSSCardReader = nullptr;

private slots:
	void OnInputPin(unsigned char ch);
	void on_pushButton_OK_clicked();
private:
	Ui::InputPWD* ui;
};

#endif // UP_INPUTPWD_H
