﻿#pragma execution_character_set("utf-8")
#include "Sys_DeviceManager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include "Gloabal.h"
#include "DevBase.h"
#include "Payment.h"
#include "Sys_dialogidcardinfo.h"
#include "Sys_dialogcameratest.h"
#include "Payment.h"

DWORD GetModuleVersion(QString strModulePath, WORD& nMajorVer, WORD& nMinorVer, WORD& nBuildNum, WORD& nRevsion)
{
	DWORD dwHnd;
	DWORD dwVerInfoSize;
	wchar_t pszVersion[2048] = { 0 };
	wchar_t szModulePath[4096] = { 0 };
	wcscpy_s(szModulePath, 4096, strModulePath.toStdWString().c_str());
	do
	{
		if (0 >= (dwVerInfoSize = GetFileVersionInfoSizeW(szModulePath, &dwHnd)))
		{
			break;
		}

		// get file version info
		if (!GetFileVersionInfoW(szModulePath, dwHnd, dwVerInfoSize, pszVersion))
		{
			break;
		}

		// Read the list of languages and code pages.
		struct LANGANDCODEPAGE
		{
			WORD    wLanguage;
			WORD    wCodePage;
		}*lpTranslate;
		unsigned int cbTranslate;
		if (!VerQueryValueW(pszVersion, L"\\VarFileInfo\\Translation", (void**)&lpTranslate, &cbTranslate))
		{
			break;
		}

		// get FileVersion string from resource
		VS_FIXEDFILEINFO* p_version;
		unsigned int version_len = 0;
		if (!VerQueryValue(pszVersion, L"\\", (void**)&p_version, &version_len))
		{
			break;
		}

		nMajorVer = (p_version->dwFileVersionMS >> 16) & 0x0000FFFF;
		nMinorVer = p_version->dwFileVersionMS & 0x0000FFFF;
		nBuildNum = (p_version->dwFileVersionLS >> 16) & 0x0000FFFF;
		nRevsion = p_version->dwFileVersionLS & 0x0000FFFF;
		qDebug() << szModulePath << "Version=" << nMajorVer << "." << nMinorVer << "." << nBuildNum << "." << nRevsion;
	} while (0);
	return GetLastError();
}

DeviceManager::DeviceManager(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	wchar_t szSerialPort[4096] = { 0 };
	WORD nPort = 0;
	EnumSerialPortW((WCHAR *)szSerialPort,4096, nPort);
	if (nPort)
	{
		for (int i = 0; i < nPort; i++)
		{
			ui.comboBox_SSCardReaderPort->addItem(QString((QChar*)&szSerialPort[i * 8]));
			ui.comboBox_DesktopSSCardReaderPort->addItem(QString((QChar*)&szSerialPort[i * 8]));
		}
	}
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	int nIndex = -1;
	if (DevConfig.nPrinterType >= PRINTER_MIN && DevConfig.nPrinterType <= PRINTER_MAX)
		nIndex = DevConfig.nPrinterType - 1;

	ui.comboBox_PrinterType->setCurrentIndex(nIndex);

	nIndex = ui.comboBox_DPI->findText(DevConfig.strDPI.c_str());
	ui.comboBox_DPI->setCurrentIndex(nIndex);

	ui.lineEdit_DepenseBox->setText(QString("%1").arg(DevConfig.nDepenseBox));
	ui.lineEdit_PrinterModule->setText(DevConfig.strPrinterModule.c_str());

	ui.lineEdit_SSCardReaderModule->setText(DevConfig.strReaderModule.c_str());

	//QMenu* pBtnMenu = new QMenu(this);

	//QAction* pInsertCard = pBtnMenu->addAction(tr("进卡"), this, SLOT(OnInsertCard()));
	//QAction* pEjectCard = pBtnMenu->addAction(tr("出卡"), this, SLOT(OnEjectCard()));
	//QAction* pRetracCard = pBtnMenu->addAction(tr("回收"), this, SLOT(OnRetractCard()));
	//QAction* pPrintCard = pBtnMenu->addAction(tr("打印"), this, SLOT(OnPrintCard()));

	//connect(pInsertCard, &QPushButton::clicked, this, &DeviceManager::OnInsertCard);
	//connect(pEjectCard, &QPushButton::clicked, this, &DeviceManager::OnEjectCard);
	//connect(pRetracCard, &QPushButton::clicked, this, &DeviceManager::OnRetractCard);
	//connect(pPrinter, &QPushButton::clicked, this, &DeviceManager::OnPrintCard);

	//ui.pushButton_PrinterTest->setMenu(pBtnMenu);
	//ui.pushButton_PrinterTest->setStyleSheet("QPushButton::menu-indicator{"
	//	"subcontrol-position:right center;"
	//	"subcontrol-origin:padding;"
	//	"image:url(./Image/arrow-down.png);}");

	ui.pushButton_PrinterTest->hide();

	// 制卡读卡器信息
	nIndex = ui.comboBox_SSCardReaderPort->findText(DevConfig.strSSCardReaderPort.c_str());
	ui.comboBox_SSCardReaderPort->setCurrentIndex(nIndex);

	if (DevConfig.nSSCardReaderType >= READER_MIN && DevConfig.nSSCardReaderType <= READER_MAX)
		nIndex = DevConfig.nSSCardReaderType - 1;
	else
		nIndex = -1;
	ui.comboBox_SSCardReaderType->setCurrentIndex(nIndex);

	if (DevConfig.nSSCardReaderPowerOnType >= READER_CONTACT && DevConfig.nSSCardReaderPowerOnType <= READER_UNCONTACT)
		nIndex = DevConfig.nSSCardReaderPowerOnType - 1;
	else
		ui.comboBox_PoweronType->setCurrentIndex(-1);
	ui.comboBox_PoweronType->setCurrentIndex(nIndex);

	// 桌面读卡器信息
	ui.lineEdit_DesktopSSCardReaderModule->setText(DevConfig.strDesktopReaderModule.c_str());
	nIndex = ui.comboBox_DesktopSSCardReaderPort->findText(DevConfig.strDesktopSSCardReaderPort.c_str());
	ui.comboBox_DesktopSSCardReaderPort->setCurrentIndex(nIndex);
	if (DevConfig.nDesktopSSCardReaderType >= READER_MIN && DevConfig.nDesktopSSCardReaderType <= READER_MAX)
		nIndex = DevConfig.nDesktopSSCardReaderType - 1;
	else
		nIndex = -1;
	ui.comboBox_DesktopSSCardReaderType->setCurrentIndex(nIndex);

	if (DevConfig.nDesktopSSCardReaderPowerOnType >= READER_CONTACT && DevConfig.nDesktopSSCardReaderPowerOnType <= READER_UNCONTACT)
		nIndex = DevConfig.nDesktopSSCardReaderPowerOnType - 1;
	else
		nIndex = -1;
	ui.comboBox_DesktopPoweronType->setCurrentIndex(nIndex);


	ui.lineEdit_IDCardReaderPort->setText(DevConfig.strIDCardReaderPort.c_str());
	ui.lineEdit_PinBroadPort->setText(DevConfig.strPinBroadPort.c_str());

	QString strAppPath = QCoreApplication::applicationFilePath();
	WORD nMajor, nMinor, nBuild, nRev;
	GetModuleVersion(strAppPath, nMajor, nMinor, nBuild, nRev);
	strAppPath = QCoreApplication::applicationDirPath();
	QString strModule = strAppPath + "/" + "KT_Printerd.dll";
	GetModuleVersion(strModule, nMajor, nMinor, nBuild, nRev);
	strModule = strAppPath + "/" + "KT_Readerd.dll";
	GetModuleVersion(strModule, nMajor, nMinor, nBuild, nRev);
	connect(this, &DeviceManager::ShowIDCardInfo, this, &DeviceManager::on_ShowIDCardInfo, Qt::QueuedConnection);
	connect(this, &DeviceManager::InputPin, this, &DeviceManager::on_InputPin);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	int nRows = ui.tableWidget->rowCount();
	int nCols = ui.tableWidget->columnCount();
	for (int nIndex = 0; nIndex < nCols; nIndex++)
		ui.tableWidget->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);

	/*for (int nIndex = 0; nIndex < nRows; nIndex++)
		ui.tableWidget->verticalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);*/
	QTableWidgetItem* pItem = nullptr;
	for (int nItem = 0; nItem < nRows; nItem++)
	{
		pItem = ui.tableWidget->item(nItem, 0);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		//pItem->setBackground(QBrush(Qt::lightGray));

		pItem = ui.tableWidget->item(nItem, 1);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		pItem->setBackground(QBrush(Qt::lightGray));
	}
	pItem = ui.tableWidget->item(1, 2);
	pItem->setFlags(pItem->flags() & (Qt::ItemIsEditable));
	g_pDataCenter->CloseDevice();
}

DeviceManager::~DeviceManager()
{
	bThreadReadIDCardRunning = false;
	bThreadReadPinRunning = false;
	if (pThreadReadIDCard && pThreadReadIDCard->joinable())
	{
		pThreadReadIDCard->join();
		delete pThreadReadIDCard;
		pThreadReadIDCard = nullptr;
	}


	if (pThreadReadPin && pThreadReadPin->joinable())
	{
		pThreadReadPin->join();
		delete pThreadReadPin;
		pThreadReadPin = nullptr;
	}

}

bool DeviceManager::CheckPrinterModule(QString& strPrinterLib, PrinterType& nPrinterType, int& nDepenseBox, QString& strDPI, QString& strMessage)
{
	strPrinterLib = ui.lineEdit_PrinterModule->text();
	if (strPrinterLib.isEmpty())
	{
		strMessage = tr("打印机驱动模块尚未设置!");
		return false;
	}

	nPrinterType = (PrinterType)(ui.comboBox_PrinterType->currentIndex() + 1);
	if (nPrinterType < PRINTER_MIN || nPrinterType > PRINTER_MAX)
	{
		strMessage = tr("打印机类型尚未设置!");
		return false;
	}
	nDepenseBox = ui.lineEdit_DepenseBox->text().toInt();
	if (nDepenseBox < CardBox_Min || nDepenseBox > CardBox_Max)
	{
		strMessage = QString(tr("进卡箱号值无效，范围应在%1~%2之间!")).arg(CardBox_Min).arg(CardBox_Max);
		return false;
	}
	strDPI = ui.comboBox_DPI->currentText();
	return true;
}

bool DeviceManager::CheckReaderModule(QString& strReaderLib,QString &strPort, ReaderBrand& nSSCardReaderType, CardPowerType& nPowerType, QString& strMessage, ReaderUsage nUsage)
{
	if (nUsage == DeviceManager::Usage_MakeCard)
	{
		strReaderLib = ui.lineEdit_SSCardReaderModule->text();
		if (strReaderLib.isEmpty())
		{
			strMessage = tr("制卡读卡器驱动模块尚未设置!");
			return false;
		}
		nPowerType = (CardPowerType)(ui.comboBox_PoweronType->currentIndex() + 1);
		nSSCardReaderType = (ReaderBrand)(ui.comboBox_SSCardReaderType->currentIndex() + 1);
		strPort = ui.comboBox_SSCardReaderPort->currentText();
	}
	else
	{
		strReaderLib = ui.lineEdit_DesktopSSCardReaderModule->text();
		if (strReaderLib.isEmpty())
		{
			strMessage = tr("桌面读卡器驱动模块尚未设置!");
			return false;
		}
		nPowerType = (CardPowerType)(ui.comboBox_DesktopPoweronType->currentIndex() + 1);
		nSSCardReaderType = (ReaderBrand)(ui.comboBox_DesktopSSCardReaderType->currentIndex() + 1);
		strPort = ui.comboBox_DesktopSSCardReaderPort->currentText();
	}
	return true;
}


bool DetectIDCardReaderPort(QString& strPort, QString& strMessage)
{
	char szPort[128] = { 0 };
	bool bOpened = false;
	do
	{
		if (::OpenReader(nullptr))
		{
			strMessage = "未检测到身份证读卡器,请检查设备是否已经正常安装!";
			break;
		}
		bOpened = true;
		if (GetCurrentPort(szPort, 128))
		{
			strMessage = "获取设备端口信息失败!";
			break;
		}

	} while (0);
	if (bOpened)
		CloseReader();
	if (strlen(szPort))
	{
		strPort = szPort;
		return true;
	}
	else
		return false;
}

void DeviceManager::on_pushButton_DetectIDCardReadPort_clicked()
{
	QString strPort, strMessage;
	if (DetectIDCardReaderPort(strPort, strMessage))
	{
		ui.lineEdit_IDCardReaderPort->setText(strPort);
	}
	else
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
	}
}

void DeviceManager::on_pushButton_BrowsePrinterModule_clicked()
{
	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("请选择打印机驱动模块"),
		QCoreApplication::applicationDirPath(),
		tr("动态库 (*.dll);;所有文件 (*)"),
		&selectedFilter);
	if (!fileName.isEmpty())
	{
		QFileInfo fi(fileName);
		qDebug() << "Priter lib:" << fi.fileName();
		ui.lineEdit_PrinterModule->setText(fi.fileName());
	}
}

void DeviceManager::on_pushButton_BrowseReaderModule_clicked()
{
	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("请选择读卡器驱动模块"),
		QCoreApplication::applicationDirPath(),
		tr("动态库 (*.dll);;所有文件 (*)"),
		&selectedFilter);
	if (!fileName.isEmpty())
	{
		QFileInfo fi(fileName);
		qDebug() << "SSCard Reader lib:" << fi.fileName();
		ui.lineEdit_SSCardReaderModule->setText(fi.fileName());
	}
}

void DeviceManager::on_pushButton_ReaderTest_clicked()
{
	int nTestCount = ui.lineEdit_SSCardReader_TestCount->text().toInt();
	if (nTestCount > 1)
	{
		int nRes = QMessageBox_CN(QMessageBox::Question, tr("提示"), "多次测试过程不会显示中间测试结果,测试结束后会有一个成功和失败的统计,是否继续?", QMessageBox::Yes | QMessageBox::No, this);
		if (nRes == QMessageBox::No)
			return;
		int nFailed = 0;
		int nSucceed = 0;
		for (int i = 0; i < nTestCount; i++)
		{
			gInfo() << __FUNCTION__ << " Test for [" << i << "];";
			if (ReaderTest_clicked(true) == -1)
				nFailed++;
			else
				nSucceed++;
			google::FlushLogFiles(0);
		}
		QString  strMessage = QString("读取成功次数:%1\n失败次数:%2").arg(nSucceed).arg(nFailed);
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
	}
	else
	{
		ReaderTest_clicked(false);
	}

	QString strMessage;
	QString strPrinterLib;
	PrinterType nPrinterType;
	int nDepenseBox;
	QString strDPI;
	char szRCode[128] = { 0 };
#pragma warning(disable:4700)
	if (!CheckPrinterModule(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok);
		return;
	}


	//g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);
}

void DeviceManager::on_pushButton_BrowseDesktopReaderModule_clicked()
{
	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("请选择读卡器驱动模块"),
		QCoreApplication::applicationDirPath(),
		tr("动态库 (*.dll);;所有文件 (*)"),
		&selectedFilter);
	if (!fileName.isEmpty())
	{
		QFileInfo fi(fileName);
		qDebug() << "SSCard Reader lib:" << fi.fileName();
		ui.lineEdit_DesktopSSCardReaderModule->setText(fi.fileName());
	}
}

void DeviceManager::on_pushButton_DesktopReaderTest_clicked()
{
	QString strMessage;
	CardPowerType nPowerType;
	ReaderBrand nSSCardReaderType;
	QString strSSCardReaderType;
	QString strReaderLib;
	QString strPort;

	if (!CheckReaderModule(strReaderLib,strPort, nSSCardReaderType, nPowerType, strMessage, Usage_Desktop))
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok);
		return;
	}

	int nRes = QMessageBox_CN(QMessageBox::Information, tr("提示"), tr("上电测试前请放好一张卡芯片卡到桌面插卡口,准备就绪后请按确定按钮继续!"), QMessageBox::Ok | QMessageBox::Cancel, this);
	if (nRes == QMessageBox::Cancel)
		return;
	bool bSucceed = false;
	do
	{
		QWaitCursor Wait;
		char szCardATR[128] = { 0 };
		char szRCode[128] = { 0 };
		int nCardATRLen = 0;
		if (g_pDataCenter->OpenSSCardReader(strReaderLib,strPort, nSSCardReaderType, strMessage))
			break;
		QString strPowerType = ui.comboBox_DesktopPoweronType->currentText();
		if (QFailed(g_pDataCenter->GetSSCardReader()->Reader_PowerOn(nPowerType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("读卡器上电失败,上电类型:%1,错误代码:%2").arg(strPowerType).arg(szRCode);
			break;
		}
		bSucceed = true;
		strMessage = QString("读卡测试成功,ATR:%1,稍后请取出卡片").arg(szCardATR);
	} while (0);
	if (!bSucceed)
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
	else
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
}

void DeviceManager::fnThreadReadPin()
{
	uchar szTemp[16] = { 0 };
	int nRet = 0;
	while (bThreadReadPinRunning)
	{
		nRet = g_pDataCenter->ScanKeyPress(szTemp);
		if (nRet > 0)
		{
			emit InputPin(szTemp[0]);
		}
		else
		{
			qDebug() << "nRet = " << nRet;
		}

		this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void DeviceManager::on_pushButton_PrinterTest_clicked()
{
	QString strMessage;
	QString strPrinterLib;
	PrinterType nPrinterType;
	int nDepenseBox;
	QString strDPI;
	if (!CheckPrinterModule(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok);
		return;
	}
	QWaitCursor Wait;

	if (g_pDataCenter->OpenPrinter(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
	{
		Wait.RestoreCursor();
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}


	if (g_pDataCenter->Depense(nDepenseBox, (CardPowerType)0, strMessage))
	{
		Wait.RestoreCursor();
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}
	SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
	/*strcpy_s(pSSCardInfo->strName, UTF8_GBK("测试用户").c_str());
	strcpy_s(pSSCardInfo->strCardID, "123456789012345678");
	strcpy_s(pSSCardInfo->strCardNum, "PP123456N");
	strcpy_s(pSSCardInfo->strValidDate, "20220101");*/
	QString strPhoto = QCoreApplication::applicationDirPath() + "/Image/SamplePhoto.bmp";

	if (g_pDataCenter->PrintCard(pSSCardInfo, strPhoto, strMessage))
	{
		Wait.RestoreCursor();
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}
	Wait.RestoreCursor();
	strMessage = tr("卡片打印成功,请取走卡片!");
	QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
}

void DeviceManager::fnThreadReadIDCard(QString strPort)
{
	int nRetry = 30;
	bool bSucceed = false;
	while (bThreadReadIDCardRunning)
	{
		if (ReaderIDCard(strPort.toStdString().c_str(), CardInfo) == IDCard_Status::IDCard_Succeed)
		{
			bSucceed = true;
			break;
		}
		nRetry--;
		if (nRetry <= 0)
		{
			break;
		}
		Sleep(100);
	}
	if (bSucceed)
	{
		emit ShowIDCardInfo(bSucceed, "");
	}
	else
		emit ShowIDCardInfo(bSucceed, "读取身份证信息失败,请稍后重试!");

}
#define    Error_Not_IDCARD         (-1)

int DeviceManager::ReaderIDCard(const char* szPort, IDCardInfo& CardInfo)
{
	int nResult = IDCard_Status::IDCard_Succeed;
	do
	{
		nResult = ::OpenReader(szPort);
		//nResult = OpenReader(nullptr);;
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		nResult = FindIDCard();
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}

		nResult = ::ReadIDCard(CardInfo);
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		CloseReader();
	} while (0);
	return nResult;
}

void DeviceManager::EnableUI(QObject* pUIObj, bool bEnable)
{
	if (!pUIObj)
		return;

	QObjectList list = pUIObj->children();
	if (pUIObj->inherits("QWidget"))
	{
		QWidget* pWidget = qobject_cast<QWidget*>(pUIObj);
		pWidget->setEnabled(bEnable);
		return;
	}
	else if (list.isEmpty())
	{
		return;
	}
	foreach(QObject * pObj, list)
	{
		qDebug() << pObj->metaObject()->className();
		EnableUI(pObj, bEnable);
	}
	this->setEnabled(true);
}
void DeviceManager::on_pushButton_IDCardReaderTest_clicked()
{
	QString strPort = ui.lineEdit_IDCardReaderPort->text();
	if (strPort.isEmpty())
	{
		if (QMessageBox::No == QMessageBox_CN(QMessageBox::Question, tr("提示"), "身份证读卡器端口为空,是否要自动检测读卡器端口?", QMessageBox::Yes | QMessageBox::No, this))
			return;
		QWaitCursor qWaitor;
		QString strPort, strMessage;
		if (!DetectIDCardReaderPort(strPort, strMessage))
		{
			qWaitor.RestoreCursor();
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok);
			return;
		}
		ui.lineEdit_IDCardReaderPort->setText(strPort);
	}
	QMessageBox_CN(QMessageBox::Information, tr("提示"), "请将身份证放置于读卡区,并每隔2秒取走一次,重复放置于读卡区!", QMessageBox::Ok, this);
	//ui.pushButton_IDCardReaderTest->setEnabled(false);
	//ui.pushButton_DetectIDCardReadPort->setEnabled(false);
	EnableUI(this, false);

	bThreadReadIDCardRunning = true;
	pThreadReadIDCard = new ::thread(&DeviceManager::fnThreadReadIDCard, this, strPort);
}

void DeviceManager::on_pushButton_TestCamera_clicked()
{
	DialogCameraTest dlg;
	dlg.exec();
}

void DeviceManager::on_ShowIDCardInfo(bool bSucceed, QString strMessage)
{
	if (bSucceed)
	{
		DialogIDCardInfo W(CardInfo, this);
		W.exec();
	}
	else
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
	EnableUI(this, true);

}

void DeviceManager::on_InputPin(char ch)
{
	switch (ch)
	{
	case 0x1b:		// cancel
	{
		bThreadReadPinRunning = false;
		if (pThreadReadPin)
		{
			pThreadReadPin->join();
			delete pThreadReadPin;
			pThreadReadPin = nullptr;
		}

		EnableUI(this, true);
		g_pDataCenter->CloseCom();
		ui.pushButton_PinBroadTest->setText("开始测试");
		break;
	}
	case 0x08:		// update
	{
		if (nPinSize > 0)
		{
			szPin[nPinSize - 1] = '\0';
			nPinSize--;
		}

		qDebug("Update...");
		break;
	}
	case 0x0d:		// confirm
	{
		qDebug("confirm...");
		bThreadReadPinRunning = false;
		if (pThreadReadPin)
		{
			pThreadReadPin->join();
			delete pThreadReadPin;
			pThreadReadPin = nullptr;
		}
		EnableUI(this, true);
		g_pDataCenter->CloseCom();
		ui.pushButton_PinBroadTest->setText("开始测试");
		break;
	}
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	{
		qDebug("function key...");
		break;
		break;
	}
	case 0xaa:
	{
		qDebug("Input end...");

	}
	default:
	{
		szPin[nPinSize++] = ch;
	}
	}
	strPin = (const char*)szPin;
	ui.lineEdit_StringInput->setText(strPin);

}

bool DeviceManager::TryOpenPinKeyBroadPort(int nPort, int nBaudrate, bool bClose)
{
	bool bSucceed = false;
	do
	{
		
		if (!g_pDataCenter->OpenCom(nPort, nBaudrate))
			break;

		unsigned char szRetInfo[255] = { 0 };
		if (!g_pDataCenter->UseEppPlainTextMode(0x06, 0, szRetInfo))
			break;

		bSucceed = true;
	} while (0);
	if (bClose)
		g_pDataCenter->CloseCom();
	return bSucceed;
}

bool DeviceManager::DetectPinBroadPort(QString& strPort)
{
	// 	int nUsbPort = 0;
	// 	if (TryOpenPinKeyBroadPort(nUsbPort, 9600))
	// 	{
	// 		strPort = "USB";
	// 		return true;
	// 	}
	// 	else
	{
		wchar_t szPortsList[4096] = { 0 };
		WORD nSerialPorts = 0;
		::EnumSerialPortW(szPortsList, 4096, nSerialPorts);
		for (int i = 0; i < nSerialPorts; i++)
		{
			wchar_t szPort[8] = { 0 };
			wcscpy_s(szPort, 8, (wchar_t*)&szPortsList[i * 8]);
			long nPort = wcstol(&szPort[3], nullptr, 10);
			if (nPort < 1)
				continue;
			if (TryOpenPinKeyBroadPort(nPort, 9600))
			{
				strPort = QString::fromStdWString(szPort);
				return true;
			}
		}
	}
	return false;
}

bool DeviceManager::Save(QString& strMessage)
{
	DeviceConfig& devconfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	QString strPrinterLib;
	PrinterType nPrinterType;
	int nDepenseBox;
	QString strDPI;
	if (!CheckPrinterModule(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
		return false;
	devconfig.strPrinter = ui.comboBox_PrinterType->currentText().toStdString();
	devconfig.nDepenseBox = nDepenseBox;
	devconfig.strPrinterModule = strPrinterLib.toStdString();
	devconfig.strDPI = strDPI.toStdString();

	CardPowerType nPowerType;
	ReaderBrand nSSCardReaderType;
	QString strSSCardReaderType;
	QString strReaderLib;
	QString strPort;

	if (!CheckReaderModule(strReaderLib,strPort, nSSCardReaderType, nPowerType, strMessage))
		return false;

	devconfig.strReaderModule = strReaderLib.toStdString();
	devconfig.strSSCardReadType = szReaderTypeList[nSSCardReaderType + 1];
	devconfig.nSSCardReaderType = nSSCardReaderType;
	devconfig.nSSCardReaderPowerOnType = nPowerType;
	devconfig.strSSCardReaderPort = strPort.toStdString();

	if (!CheckReaderModule(strReaderLib, strPort, nSSCardReaderType, nPowerType, strMessage, Usage_Desktop))
		return false;

	devconfig.strDesktopReaderModule = strReaderLib.toStdString();
	devconfig.strDesktopSSCardReadType = szReaderTypeList[nSSCardReaderType + 1];
	devconfig.nDesktopSSCardReaderType = nSSCardReaderType;
	devconfig.nDesktopSSCardReaderPowerOnType = nPowerType;
	devconfig.strDesktopSSCardReaderPort = strPort.toStdString();

	devconfig.strPinBroadPort = ui.lineEdit_PinBroadPort->text().toStdString();
	if (devconfig.strPinBroadPort.size() <= 0)
	{
		strMessage = "身份证器读卡器端口未设置!";
		return false;
	}
	devconfig.strPinBroadBaudrate = "9600";				   // 9600

	devconfig.strIDCardReaderPort = ui.lineEdit_PinBroadPort->text().toStdString();// 身份证读卡器配置:USB, COM1, COM2...
	if (devconfig.strIDCardReaderPort.size() <= 0)
	{
		strMessage = "密码键盘端口未设置!";
		return false;
	}

	devconfig.strTerminalCode = ui.tableWidget->item(2, 2)->text().toStdString();					   // 终端唯一识别码
	if (devconfig.strTerminalCode.size() <= 10)
	{
		strMessage = "终端识别码无效或未设置!";
		return false;
	}

	return true;
}

int DeviceManager::ReaderTest_clicked(bool bContinue)
{
	QString strMessage;
	QString strPrinterLib;
	PrinterType nPrinterType;
	int nDepenseBox;
	QString strDPI;
#pragma warning(disable:4700)
	if (!CheckPrinterModule(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
	{
		if (bContinue)
			return 1;
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
		return 1;
	}

	CardPowerType nPowerType;
	ReaderBrand nSSCardReaderType;
	QString strSSCardReaderType;
	QString strReaderLib;
	QString strPort;

	if (!CheckReaderModule(strReaderLib,strPort, nSSCardReaderType, nPowerType, strMessage))
	{
		if (bContinue)
			return 1;
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
		return 1;
	}

	if (!bContinue)
	{
		int nRes = QMessageBox_CN(QMessageBox::Information, tr("提示"), tr("上电测试前请放好一张卡芯片卡到打印机进卡口,准备就绪后请按确定按钮继续!"), QMessageBox::Ok | QMessageBox::Cancel, this);
		if (nRes == QMessageBox::Cancel)
			return 1;
	}

	char szRCode[128] = { 0 };
	bool bSucceed = false;
	RegionInfo& reginfo = g_pDataCenter->GetSysConfigure()->Region;

	BOXINFO BoxInfo;
	ZeroMemory(&BoxInfo, sizeof(BOXINFO));
	do
	{
		QWaitCursor Wait;
		if (g_pDataCenter->OpenPrinter(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
			break;

		if (g_pDataCenter->OpenSSCardReader(strReaderLib,strPort, nSSCardReaderType, strMessage))
			break;
		int nDepensePos = 4;	// 1-读磁位；2-接触IC位;3-非接IC位;4-打印位， 默认为接触位
		if (nPowerType == CardPowerType::READER_UNCONTACT)
			nDepensePos = 3;
		else if (nPowerType == CardPowerType::READER_CONTACT)
			nDepensePos = 2;

		if (QFailed(g_pDataCenter->GetPrinter()->Printer_Dispense(nDepenseBox - 1, nDepensePos, szRCode)))
		{
			strMessage = QString("Printer_Dispense失败，错误代码:%1!").arg(szRCode);
			break;
		}

		//if (g_pDataCenter->Depense(nDepenseBox, nPowerType, strMessage))
		//	break;

		char szCardATR[1024] = { 0 };
		char szRCode[1024] = { 0 };
		int nCardATRLen = 0;
		string strCardATR;
		QString strPowerType = ui.comboBox_PoweronType->currentText();

		if (DriverInit((HANDLE)g_pDataCenter->GetSSCardReader(), (char*)reginfo.strCityCode.c_str(), (char*)reginfo.strSSCardDefaulutPin.c_str(), (char*)reginfo.strPrimaryKey.c_str(), szRCode))
		{
			strMessage = QString("DriverInit失败,上电类型:%1,错误代码:%2").arg(strPowerType).arg(szRCode);
			break;
		}

		if (QFailed(g_pDataCenter->GetSSCardReader()->Reader_PowerOn(nPowerType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("Reader_PowerOn,上电类型:%1,错误代码:%2").arg(strPowerType).arg(szRCode);
			break;
		}
		SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();

		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
		strcpy(pSSCardInfo->strReleaseDate, "20220416");
		strcpy(pSSCardInfo->strValidDate, "");
		strcpy(pSSCardInfo->strCardNum, "PP1234567");
		strcpy(pSSCardInfo->strCardID, "123456789012345678");
		QString strName = "测试卡";
		strcpy(pSSCardInfo->strName, strName.toLocal8Bit().data());
		strcpy(pSSCardInfo->strSex, "1");
		strcpy(pSSCardInfo->strNation, "01");
		strcpy(pSSCardInfo->strBirthday, "19991231");
		if (QFailed(g_pDataCenter->ReadCard(pSSCardInfo, strMessage)))
		{
			break;
		}
		bSucceed = true;
		strMessage = QString("读卡测试成功\n银行卡号:%2").arg(pSSCardInfo->strBankNum);
	} while (0);
	if (!bContinue)
	{
		if (!bSucceed)
		{
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
			return 0;
		}
		else
		{
			QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
			return -1;
		}
	}
	else
	{
		gInfo() << gQStr(strMessage);
		if (bSucceed)
			return 0;
		else
			return -1;
	}
}

void DeviceManager::on_pushButton_DetectIDPinBroadPort_clicked()
{
	QWaitCursor Waitor;
	QString strPort;
	if (DetectPinBroadPort(strPort))
	{
		ui.lineEdit_PinBroadPort->setText(strPort);
	}
	else
	{
		Waitor.RestoreCursor();
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), "未检测到密码键盘,请检查设备是否正确安装!", QMessageBox::Ok, this);
	}
}

void DeviceManager::on_pushButton_PinBroadTest_clicked()
{
	wstring strPinPort = ui.lineEdit_PinBroadPort->text().toStdWString();
	if (!strPinPort.size())
	{
		if (QMessageBox::No == QMessageBox_CN(QMessageBox::Question, tr("提示"), "密码键盘端口为空,是否要自动检测键盘端口?", QMessageBox::Yes | QMessageBox::No, this))
			return;
		QWaitCursor Waitor;
		QString strPort;
		if (!DetectPinBroadPort(strPort))
		{
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), "未检测到密码键盘,请检查设备是否正确安装!", QMessageBox::Ok, this);
			return;
		}
		else
			ui.lineEdit_PinBroadPort->setText(strPort);
	}
	if (!bThreadReadPinRunning)
	{
		QMessageBox_CN(QMessageBox::Information, tr("提示"), "请在密码键盘上进行输入，按'确认'或'取消'键结束测试!", QMessageBox::Ok, this);

		int nPort = wcstol(&strPinPort.c_str()[3], nullptr, 10);
		if (!TryOpenPinKeyBroadPort(nPort, 9600, false))
		{
			QMessageBox_CN(QMessageBox::Information, tr("提示"), "打开密码键盘失败!", QMessageBox::Ok, this);
			return;
		}
		ZeroMemory(szPin, sizeof(szPin));
		EnableUI(this, false);
		ui.lineEdit_PinBroadPort->setEnabled(true);
		ui.pushButton_PinBroadTest->setEnabled(true);
		ui.pushButton_PinBroadTest->setText("停止测试");
		ui.lineEdit_StringInput->setText("");
		bThreadReadPinRunning = true;
		nPinSize = 0;
		ZeroMemory(szPin, 128);
		pThreadReadPin = new std::thread(&DeviceManager::fnThreadReadPin, this);
	}
	else
	{
		EnableUI(this, true);
		bThreadReadPinRunning = false;
		ui.pushButton_PinBroadTest->setText("输入测试");
		if (pThreadReadPin)
		{
			pThreadReadPin->join();
			delete pThreadReadPin;
			pThreadReadPin = nullptr;
		}
		g_pDataCenter->CloseCom();
	}
}

void DeviceManager::on_comboBox_PrinterTest_currentIndexChanged(int index)
{

}

void DeviceManager::on_comboBox_PrinterTest_activated(int index)
{
	qDebug() << __FUNCTION__ << "Current Item:" << index;
	//if (index == nCurrentPrinterTest)
	//	ui.comboBox_PrinterTest->emit currentIndexChanged(index);

}

void DeviceManager::on_pushButton_MakePhoto_clicked()
{
	QString strFile = QFileDialog::getOpenFileName();
	if (strFile.isEmpty())
	{
		return;
	}
	SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
	if (g_pDataCenter->LoadSSCardData(strFile.toStdString(),pSSCardInfo))
	{
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
		g_pDataCenter->SetIDCardInfo(pIDCard);

		strcpy((char*)g_pDataCenter->GetIDCardInfo()->szIdentity, pSSCardInfo->strCardID);
		QString strMessage;
		SaveSSCardPhoto(strMessage, pSSCardInfo->strPhoto);
	}
}

void DeviceManager::on_pushButton_Excute_clicked()
{
	int index = ui.comboBox_PrinterTest->currentIndex();
	QString strMessage;
	QString strPrinterLib;
	PrinterType nPrinterType;
	int nDepenseBox;
	QString strDPI;
	char szRCode[128] = { 0 };
	if (!CheckPrinterModule(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok);
		return;
	}
	QWaitCursor Wait;
	if (!g_pDataCenter->GetPrinter())
	{
		if (g_pDataCenter->OpenPrinter(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
		{
			Wait.RestoreCursor();
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
			return;
		}
	}
	else
	{
		if (g_pDataCenter->OpenPrinter(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
		{
			Wait.RestoreCursor();
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
			return;
		}
	}
	switch (index)
	{
	case 0:
	default:
	{
		if (g_pDataCenter->Depense(nDepenseBox, (CardPowerType)0, strMessage))
		{
			Wait.RestoreCursor();
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
			return;
		}
		break;
	}

	case 1:
	{
		if (QFailed(g_pDataCenter->GetPrinter()->Printer_Eject(szRCode)))
		{
			strMessage = QString("Printer_Eject失败，错误代码:%1!").arg(szRCode);
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
			return;
		}
		break;
	}

	case 2:
	{
		if (QFailed(g_pDataCenter->GetPrinter()->Printer_Retract(1, szRCode)))
		{
			strMessage = QString("Printer_Eject失败，错误代码:%1!").arg(szRCode);
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
			return;
		}
		break;
	}

	case 3:
	{
		SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
		strcpy_s(pSSCardInfo->strName, UTF8_GBK("测试用户").c_str());
		strcpy_s(pSSCardInfo->strCardID, "123456789012345678");
		strcpy_s(pSSCardInfo->strCardNum, "PP123456N");
		strcpy_s(pSSCardInfo->strValidDate, "20220101");
		QString strPhoto = QCoreApplication::applicationDirPath() + "/Image/SamplePhoto.bmp";

		if (g_pDataCenter->PrintCard(pSSCardInfo, strPhoto, strMessage))
		{
			Wait.RestoreCursor();
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
			return;
		}
		Wait.RestoreCursor();
		strMessage = tr("卡片打印成功,请取走卡片!");
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
		break;
	}
	case 4:		// 出口卡进卡
	{
		QMessageBox_CN(QMessageBox::Information, tr("提示"), "请在确认后5秒内在出卡口放入卡片!", QMessageBox::Ok, this);
		g_pDataCenter->Print_EnableCard(5000, 1, strMessage);
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
		break;
	}
	}
}
