#pragma once

#include <QWidget>
#include "ui_DeviceManager.h"
#include "Gloabal.h"
#include "DevBase.h"
#include "ConfigurePage.h"

class DeviceManager : public QWidget
{
	Q_OBJECT

public:
	DeviceManager(QWidget* parent = Q_NULLPTR);
	~DeviceManager();
	int PrintCard(SSCardInfoPtr& pSSCardInfo, QString& strPhoto, QString& strMessage);
	int OpenPrinter(QString strPrinterLib, PrinterType nPrinterType, int& nDepenseBox, QString& strDPI, QString& strMesssage);
	int OpenReader(QString strLib, ReaderBrand nReaderType, QString& strMesssage);
	int Depense(int nDepenseBox, CardPowerType nPowerOnType, QString& strMessage);
	void CloseDevice();
	int ReaderIDCard(const char* szPort, IDCardInfo& CardInfo);
	void fnThreadReadIDCard(QString strPort);
	void fnThreadReadPin();
	IDCardInfo CardInfo;
	void EnableUI(QObject* pUIObj, bool bEnable = true);
	bool DetectPinBroadPort(QString& strPort);

	bool Save(QString& strMessage);

	bool TryOpenPinKeyBroadPort(int nPort, int nBaudrate);

	bool CheckPrinter(QString strPrinterLib, PrinterType nType, int& nDepenseBox, QString& strDPI, QString& strMessage);

	enum ReaderUsage
	{
		Usage_MakeCard,
		Usage_Desktop
	};
	bool CheckReader(QString strReaderLib, ReaderBrand nType, CardPowerType nPowerType, QString strMessage, ReaderUsage nUsage = Usage_MakeCard);
signals:
	void ShowIDCardInfo(bool bSuccceed, QString strMessage);
	void InputPin(char ch);

private slots:
	void on_pushButton_DetectIDCardReadPort_clicked();

	void on_pushButton_BrowsePrinterModule_clicked();

	void on_pushButton_BrowseReaderModule_clicked();

	void on_pushButton_ReaderTest_clicked();

	void on_pushButton_PrinterTest_clicked();

	void on_pushButton_IDCardReaderTest_clicked();

	void on_pushButton_TestCamera_clicked();

	void on_ShowIDCardInfo(bool bSucceed, QString strMessage);

	void on_InputPin(char ch);

	void on_pushButton_DetectIDPinBroadPort_clicked();

	void on_pushButton_PinBroadTest_clicked();

	void on_pushButton_BrowseDesktopReaderModule_clicked();

	void on_pushButton_DesktopReaderTest_clicked();

private:
	Ui::DeviceManager ui;
	KT_PrinterLibPtr	m_pPrinterlib = nullptr;
	KT_ReaderLibPtr		m_pReaderLib = nullptr;
	KT_Printer* m_pPrinter = nullptr;
	KT_Reader* m_pReader = nullptr;
	bool bThreadReadIDCardRunning = false;
	bool bThreadReadPinRunning = false;
	std::thread ThreadReadIDCard;
	std::thread ThreadReadPin;
	QString     strPin;
	uchar   szPin[128];
	int     nPinSize = 0;
};
