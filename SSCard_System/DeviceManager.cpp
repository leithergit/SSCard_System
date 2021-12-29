#pragma execution_character_set("utf-8")
#include "DeviceManager.h"
#include <QFileDialog>
#include <QMessageBox>
#include "Gloabal.h"
#include "DevBase.h"
#include "Payment.h"
#include "dialogidcardinfo.h"
#include "dialogcameratest.h"

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

}

DeviceManager::~DeviceManager()
{

}

bool DeviceManager::CheckPrinter(QString strPrinterLib, PrinterType nPrinterType, int& nDepenseBox, QString& strDPI, QString& strMessage)
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

bool DeviceManager::CheckReader(QString strReaderLib, ReaderBrand nSSCardReaderType, CardPowerType nPowerType, QString strMessage, ReaderUsage nUsage)
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
	}
	return true;
}

int DeviceManager::OpenPrinter(QString strPrinterLib, PrinterType nPrinterType, int& nDepenseBox, QString& strDPI, QString& strMessage)
{
	int nResult = -1;
	char szRCode[32] = { 0 };

	QString strPtinerType = ui.comboBox_PrinterType->currentText();
	QString strPritnerModule = ui.lineEdit_PrinterModule->text();
	try
	{
		m_pPrinterlib = nullptr;
		do
		{
			if (!m_pPrinterlib)
			{
				QString strAppPath = QCoreApplication::applicationDirPath();
				QString strFullPrinterModule = strAppPath + "/" + strPritnerModule;

				m_pPrinterlib = make_shared<KTModule<KT_Printer>>(strFullPrinterModule.toStdString());
				if (!m_pPrinterlib)
				{
					strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strFullPrinterModule);
					break;
				}
				m_pPrinter = m_pPrinterlib->Instance();
				if (!m_pPrinter)
				{
					strMessage = QString("创建‘%1’实例失败!").arg(strFullPrinterModule);
					break;
				}

				if (QFailed(nResult = m_pPrinter->Printer_Init(nPrinterType, szRCode)))
				{
					strMessage = QString("Printer_Init‘%1’失败,错误代码:%2").arg(strPtinerType.toStdString().c_str()).arg(szRCode);
					break;
				}

				if (QFailed(nResult = m_pPrinter->Printer_Open(szRCode)))
				{
					strMessage = QString("Printer_Open‘%1’失败,错误代码:%2").arg(strPtinerType.toStdString().c_str()).arg(szRCode);
					break;
				}
			}

		} while (0);
		if (QFailed(nResult))
			return -1;
		else
			return 0;
	}
	catch (std::exception& e)
	{
		strMessage = e.what();
		gError() << gQStr(strMessage);
		return -1;
	}
}

int DeviceManager::PrintCard(SSCardInfoPtr& pSSCardInfo, QString& strPhoto, QString& strMessage)
{
	char szBuffer[1024] = { 0 };
	int nBufferSize = sizeof(szBuffer);
	int nResult = -1;
	char szRCode[32] = { 0 };
	CardFormPtr& pCardForm = g_pDataCenter->GetCardForm();
	do
	{
		nBufferSize = sizeof(szBuffer);
		ZeroMemory(szBuffer, nBufferSize);

		if (QFailed(nResult = m_pPrinter->Printer_InitPrint(nullptr, szRCode)))
		{
			strMessage = QString("Printer_InitPrint失败，错误代码:%1!").arg(szRCode);
			break;
		}

		TextPosition* pFieldPos = &pCardForm->posName;
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strName, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		pFieldPos = &pCardForm->posIDNumber;
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardID, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		pFieldPos = &pCardForm->posSSCardID;
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardNum, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		pFieldPos = &pCardForm->posIssueDate;

		int nYear, nMonth, nDay;

		sscanf_s(pSSCardInfo->strValidDate, "%04d%02d%02d", &nYear, &nMonth, &nDay);
		char szValidate[32] = { 0 };
		sprintf_s(szValidate, 16, "%d年%d月%d日", nYear, nMonth, nDay);

		m_pPrinter->Printer_AddText((char*)UTF8_GBK(szValidate).c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		ImagePosition& ImgPos = pCardForm->posImage;
		m_pPrinter->Printer_AddImage((char*)strPhoto.toStdString().c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode);
		if (QFailed(m_pPrinter->Printer_StartPrint(szRCode)))
		{
			strMessage = QString("Printer_StartPrint失败，错误代码:%1!").arg(szRCode);
			break;
		}
		if (QFailed(m_pPrinter->Printer_Eject(szRCode)))
		{
			strMessage = QString("Printer_Eject失败，错误代码:%1!").arg(szRCode);
			break;
		}
		nResult = 0;

	} while (0);
	return nResult;

}

int DeviceManager::Depense(int nDepenseBox, CardPowerType nPowerOnType, QString& strMessage)
{
	int nResult = -1;
	BOXINFO BoxInfo;
	ZeroMemory(&BoxInfo, sizeof(BOXINFO));
	PRINTERSTATUS PrinterStatus;
	char szRCode[32] = { 0 };
	do
	{
		if (!m_pPrinter)
		{
			strMessage = "打印机未初始化";
			break;
		}
		if (QFailed(m_pPrinter->Printer_BoxStatus(BoxInfo, szRCode)))
		{
			strMessage = QString("Printer_BoxStatus失败:%1").arg(szRCode);
			break;
		}

		if (BoxInfo.BoxList[nDepenseBox].BoxStatus == 2)
		{
			strMessage = QString("卡箱[%1]:BoxStatus = %2,无卡!").arg(nDepenseBox).arg(BoxInfo.BoxList[nDepenseBox].BoxStatus);
			break;
		}

		if (QFailed(m_pPrinter->Printer_Status(PrinterStatus, szRCode)))
		{
			strMessage = QString("Printer_Status失败，错误代码:%1!").arg(szRCode);
			break;
		}
		if (PrinterStatus.fwDevice != 0)
		{
			strMessage = QString("打印机未就绪,状态代码:%1!").arg(PrinterStatus.fwDevice);
			break;
		}

		if (PrinterStatus.fwMedia != 0)
		{
			strMessage = QString("打印机色带耗尽或未安装色带,状态代码:%1!").arg(PrinterStatus.fwToner);
			if (QFailed(m_pPrinter->Printer_Retract(1, szRCode)))
			{
				strMessage = QString("打印机尚有未取走卡片,尝试将其移动到回收箱失败，错误代码:%1!").arg(szRCode);
				break;
			}
		}
		if (PrinterStatus.fwToner == 2)
		{
			strMessage = QString("打印机色带耗尽或未安装色带,状态代码:%1!").arg(PrinterStatus.fwToner);
			break;
		}
		int nDepensePos = 4;	// 1-读磁位；2-接触IC位;3-非接IC位;4-打印位， 默认为接触位
		if (nPowerOnType == CardPowerType::READER_UNCONTACT)
			nDepensePos = 3;
		else if (nPowerOnType == CardPowerType::READER_CONTACT)
			nDepensePos = 2;

		if (QFailed(m_pPrinter->Printer_Dispense(nDepenseBox, nDepensePos, szRCode)))
		{
			strMessage = QString("Printer_Dispense失败，错误代码:%1!").arg(szRCode);
			break;
		}
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		gInfo() << gQStr(strMessage);
	}
	return nResult;
}

int DeviceManager::OpenReader(QString strLib, ReaderBrand nReaderType, QString& strMessage)
{
	int nResult = -1;
	try
	{
		m_pReaderLib = nullptr;
		do
		{
			if (!m_pReaderLib)
			{
				char szRCode[32] = { 0 };
				QString strAppPath = QCoreApplication::applicationDirPath();
				QString strReaderMudule = strAppPath + "/" + strLib.toStdString().c_str();

				m_pReaderLib = make_shared<KTModule<KT_Reader>>(strReaderMudule.toStdString());
				if (!m_pReaderLib)
				{
					strMessage = strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strReaderMudule);
					break;
				}
				m_pReader = m_pReaderLib->Instance();
				if (!m_pReader)
				{
					strMessage = QString("创建‘%1’实例失败!").arg(strReaderMudule);
					break;
				}
				if (QFailed(nResult = m_pReader->Reader_Create(nReaderType, szRCode)))
				{
					strMessage = QString("Reader_Create(‘%1’)失败,错误代码:%2").arg(nReaderType).arg(szRCode);
					break;
				}
				if (QFailed(nResult = m_pReader->Reader_Init(szRCode)))
				{
					strMessage = QString("Reader_Init失败,错误代码:%2").arg(szRCode);
					break;
				}
			}

		} while (0);
		if (QFailed(nResult))
			return -1;
		else
			return 0;
	}
	catch (std::exception& e)
	{
		strMessage = e.what();
		gError() << gQStr(strMessage);
		return -1;
	}
	return 0;
}

void DeviceManager::CloseDevice()
{
	char szRCode[32] = { 0 };

	if (m_pPrinter)
	{
		m_pPrinter->Printer_Close(szRCode);
		m_pPrinter = nullptr;
	}

	m_pPrinterlib = nullptr;

	if (m_pReader)
	{
		m_pReader->Reader_Exit(szRCode);
		m_pReader = nullptr;
	}
	m_pReaderLib = nullptr;

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
	QString strMessage;
	QString strPrinterLib;
	PrinterType nPrinterType;
	int nDepenseBox;
	QString strDPI;
#pragma warning(disable:4700)
	if (!CheckPrinter(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok);
		return;
	}

	CardPowerType nPowerType;
	ReaderBrand nSSCardReaderType;
	QString strSSCardReaderType;
	QString strReaderLib;

	if (!CheckReader(strReaderLib, nSSCardReaderType, nPowerType, strMessage))
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok);
		return;
	}

	int nRes = QMessageBox_CN(QMessageBox::Information, tr("提示"), tr("上电测试前请放好一张卡芯片卡到打印机进卡口,准备就绪后请按确定按钮继续!"), QMessageBox::Ok | QMessageBox::Cancel, this);
	if (nRes == QMessageBox::Cancel)
		return;
	char szRCode[128] = { 0 };
	do
	{
		QWaitCursor Wait;
		if (OpenPrinter(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
			break;

		if (OpenReader(strReaderLib, nSSCardReaderType, strMessage))
			break;

		if (Depense(nDepenseBox, nPowerType, strMessage))
			break;

		char szCardATR[128] = { 0 };
		char szRCode[128] = { 0 };
		int nCardATRLen = 0;

		QString strPowerType = ui.comboBox_PoweronType->currentText();
		if (QFailed(m_pReader->Reader_PowerOn(nPowerType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("读卡器上电失败,上电类型:%1,错误代码:%2").arg(strPowerType).arg(szRCode);
			break;
		}
		strMessage = QString("上电测试成功,ATR:%1,稍后请取出卡片").arg(szCardATR);
	} while (0);
	QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
	m_pPrinter->Printer_Eject(szRCode);
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

	if (!CheckReader(strReaderLib, nSSCardReaderType, nPowerType, strMessage, Usage_Desktop))
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok);
		return;
	}

	int nRes = QMessageBox_CN(QMessageBox::Information, tr("提示"), tr("上电测试前请放好一张卡芯片卡到桌面插卡口,准备就绪后请按确定按钮继续!"), QMessageBox::Ok | QMessageBox::Cancel, this);
	if (nRes == QMessageBox::Cancel)
		return;
	do
	{
		QWaitCursor Wait;
		char szCardATR[128] = { 0 };
		char szRCode[128] = { 0 };
		int nCardATRLen = 0;
		QString strPowerType = ui.comboBox_DesktopPoweronType->currentText();
		if (QFailed(m_pReader->Reader_PowerOn(nPowerType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("读卡器上电失败,上电类型:%1,错误代码:%2").arg(strPowerType).arg(szRCode);
			break;
		}
		strMessage = QString("上电测试成功,ATR:%1,稍后请取出卡片").arg(szCardATR);
	} while (0);
	QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
}

void DeviceManager::fnThreadReadPin()
{
	uchar szTemp[16] = { 0 };
	int nRet = 0;
	while (bThreadReadPinRunning)
	{
		nRet = SUNSON_ScanKeyPress(szTemp);
		if (nRet == 0)
			emit InputPin(szTemp[0]);
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
	if (!CheckPrinter(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok);
		return;
	}
	QWaitCursor Wait;

	if (OpenPrinter(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
	{
		Wait.RestoreCursor();
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}
	if (Depense(nDepenseBox, (CardPowerType)0, strMessage))
	{
		Wait.RestoreCursor();
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}
	SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
	strcpy_s(pSSCardInfo->strName, UTF8_GBK("测试用户").c_str());
	strcpy_s(pSSCardInfo->strCardID, "123456789012345678");
	strcpy_s(pSSCardInfo->strCardNum, "PP123456N");
	strcpy_s(pSSCardInfo->strValidDate, "20220101");
	QString strPhoto = QCoreApplication::applicationDirPath() + "/Image/SamplePhoto.bmp";

	if (PrintCard(pSSCardInfo, strPhoto, strMessage))
	{
		Wait.RestoreCursor();
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}
	Wait.RestoreCursor();
	strMessage = tr("卡片打印成功,请最走卡片!");
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
	ThreadReadIDCard = std::thread(&DeviceManager::fnThreadReadIDCard, this, strPort);
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
		ThreadReadPin.join();
		EnableUI(this, true);
		ui.pushButton_PinBroadTest->setText("停止测试");
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
		ThreadReadPin.join();
		EnableUI(this, true);
		ui.pushButton_PinBroadTest->setText("停止测试");
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

bool DeviceManager::TryOpenPinKeyBroadPort(int nPort, int nBaudrate)
{
	bool bSucceed = false;
	do
	{
		if (SUNSON_OpenCom(nPort, nBaudrate))
			break;

		unsigned char szRetInfo[255] = { 0 };
		if (SUNSON_UseEppPlainTextMode(0x03, szRetInfo))
			break;

		bSucceed = true;
	} while (0);
	SUNSON_CloseCom();
	return bSucceed;
}

bool DeviceManager::DetectPinBroadPort(QString& strPort)
{
	int nUsbPort = 0;
	if (TryOpenPinKeyBroadPort(nUsbPort, 9600))
	{
		strPort = "USB";
		return true;
	}
	else
	{
		wchar_t szPortsList[4096] = { 0 };
		WORD nSerialPorts = 0;
		::EnumSerialPortW(szPortsList, 4096, nSerialPorts);
		for (int i = 0; i < nSerialPorts; i++)
		{
			wchar_t szPort[8] = { 0 };
			wcscpy_s(szPort, 8, (wchar_t*)&szPortsList[i * 8]);
			long nPort = wcstol(&szPort[3], nullptr, 10);

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
	if (!CheckPrinter(strPrinterLib, nPrinterType, nDepenseBox, strDPI, strMessage))
		return false;
	devconfig.strPrinter = ui.comboBox_PrinterType->currentText().toStdString();
	devconfig.nDepenseBox = nDepenseBox;
	devconfig.strPrinterModule = strPrinterLib.toStdString();
	devconfig.strDPI = strDPI.toStdString();

	CardPowerType nPowerType;
	ReaderBrand nSSCardReaderType;
	QString strSSCardReaderType;
	QString strReaderLib;

	if (!CheckReader(strReaderLib, nSSCardReaderType, nPowerType, strMessage))
		return false;

	devconfig.strReaderModule = strReaderLib.toStdString();
	devconfig.strSSCardReadType = szReaderTypeList[nSSCardReaderType + 1];
	devconfig.nSSCardReaderType = nSSCardReaderType;
	devconfig.nSSCardReaderPowerOnType = nPowerType;
	devconfig.strSSCardReaderPort = ui.comboBox_SSCardReaderPort->currentText().toStdString();

	if (!CheckReader(strReaderLib, nSSCardReaderType, nPowerType, strMessage, Usage_Desktop))
		return false;

	devconfig.strDesktopReaderModule = strReaderLib.toStdString();
	devconfig.strDesktopSSCardReadType = szReaderTypeList[nSSCardReaderType + 1];
	devconfig.nDesktopSSCardReaderType = nSSCardReaderType;
	devconfig.nDesktopSSCardReaderPowerOnType = nPowerType;
	devconfig.strDesktopSSCardReaderPort = ui.comboBox_SSCardReaderPort->currentText().toStdString();

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
	string strPinPort = ui.lineEdit_PinBroadPort->text().toStdString();
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
		QMessageBox_CN(QMessageBox::Information, tr("提示"), "请在密码键上进行输入，按'确认'或'取消'键结束测试!", QMessageBox::Ok, this);
		EnableUI(this, false);
		ui.lineEdit_PinBroadPort->setEnabled(true);
		ui.pushButton_PinBroadTest->setEnabled(true);
		ui.pushButton_PinBroadTest->setText("停止测试");
		bThreadReadPinRunning = true;
		ThreadReadPin = std::thread(&DeviceManager::fnThreadReadPin, this);
	}
	else
	{
		EnableUI(this, true);
		bThreadReadPinRunning = false;
		ui.pushButton_PinBroadTest->setText("输入测试");
		ThreadReadPin.join();
	}
}
