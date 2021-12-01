#pragma execution_character_set("utf-8")
#include "uc_makecard.h"
#include "ui_uc_makecard.h"
#include "Gloabal.h"

uc_MakeCard::uc_MakeCard(QLabel* pTitle, QString strStepImage, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nTimeout, parent),
	ui(new Ui::MakeCard)
{
	ui->setupUi(this);
}

uc_MakeCard::~uc_MakeCard()
{
	delete ui;
}


int uc_MakeCard::ProcessBussiness()
{
	QString strMessage;
	if (QFailed(OpenDevice(strMessage)))
	{
		gError() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return -1;
	}
	m_pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	m_bWorkThreadRunning = true;
	m_pWorkThread = new std::thread(&uc_MakeCard::ThreadWork, this);
	return 0;
}

int uc_MakeCard::OpenPrinter(QString& strMessage)
{
	int nResult = -1;
	try
	{
		do
		{
			Kingaotech::_PRINTERBOX boxesUnit[10];
			Kingaotech::BOXINFO boxesInfo = { 10, boxesUnit };
			Kingaotech::LPBOXINFO lpBoxInfo = &boxesInfo;
			Kingaotech::PRINTERSTATUS PrinterStatus;
			Kingaotech::LPPRINTERSTATUS lpPrinterStatus = &PrinterStatus;
			char szRCode[32] = { 0 };
			DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
			QString strAppPath = QCoreApplication::applicationDirPath();
			QString strPrinterMudule = strAppPath + "/" + DevConfig.strPrinterModule.c_str();

			m_pPrinterlib = make_shared<KTModule<KT_Printer>>(strPrinterMudule);
			if (!m_pPrinterlib)
			{
				strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strPrinterMudule);
				break;
			}
			m_pPrinter = m_pPrinterlib->Instance();
			if (!m_pPrinter)
			{
				strMessage = QString("创建‘%1’实例失败!").arg(strPrinterMudule);
				break;
			}

			if (QFailed(nResult = m_pPrinter->Printer_Init(DevConfig.nPrinterType, szRCode)))
			{
				strMessage = QString("Printer_Init‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
				break;
			}
			if (QFailed(m_pPrinter->Printer_BoxStatus(lpBoxInfo, szRCode)))
			{
				strMessage = QString("Printer_BoxStatus失败:%1").arg(szRCode);
				break;
			}
			if (lpBoxInfo->BoxList[0].BoxStatus == 2)
			{
				strMessage = QString("卡箱:%1无卡!").arg(szRCode);
				break;
			}

			if (QFailed(m_pPrinter->Printer_Status(lpPrinterStatus, szRCode)))
			{
				strMessage = QString("Printer_Status失败，错误代码:%1!").arg(szRCode);
				break;
			}
			if (lpPrinterStatus->fwDevice != 0)
			{
				strMessage = QString("打印机未就绪,状态代码:%1!").arg(lpPrinterStatus->fwDevice);
				break;
			}

			if (lpPrinterStatus->fwMedia != 0)
			{
				strMessage = QString("打印机色带耗尽或未安装色带,状态代码:%1!").arg(lpPrinterStatus->fwToner);
				if (QFailed(m_pPrinter->Printer_Retract(1, szRCode)))
				{
					strMessage = QString("打印机尚有未取走卡片,尝试将其移动到回收箱失败，错误代码:%1!").arg(szRCode);
					break;
				}
			}
			if (lpPrinterStatus->fwToner == 2)
			{
				strMessage = QString("打印机色带耗尽或未安装色带,状态代码:%1!").arg(lpPrinterStatus->fwToner);
				break;
			}
			int nDepensePos = 2;	// 1-读磁位；2-接触IC位;3-非接IC位;4-打印位， 默认为接触位
			if (DevConfig.nSSCardReaderPowerOnType == "2")
				nDepensePos = 3;

			if (QFailed(m_pPrinter->Printer_Dispense(DevConfig.nDepenseBox, nDepensePos, szRCode)))
			{
				strMessage = QString("Printer_Dispense失败，错误代码:%1!").arg(szRCode);
				break;
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

int uc_MakeCard::OpenReader(QString& strMessage)
{
	int nResult = -1;
	try
	{
		do
		{
			char szRCode[32] = { 0 };
			DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
			QString strAppPath = QCoreApplication::applicationDirPath();
			QString strReaderMudule = strAppPath + "/" + DevConfig.strPrinterModule.c_str();

			m_pReaderLib = make_shared<KTModule<KT_Reader>>(strReaderMudule);
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
			if (QFailed(nResult = m_pReader->Reader_Create(DevConfig.nSSCardReaderType, szRCode)))
			{
				strMessage = QString("Reader_Create‘%1’失败,错误代码:%2").arg(DevConfig.strSSCardReadType.c_str()).arg(szRCode);
				break;
			}
			if (QFailed(nResult = m_pReader->Reader_Init(szRCode)))
			{
				strMessage = QString("Reader_Init失败,错误代码:%2").arg(szRCode);
				break;
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

int uc_MakeCard::OpenDevice(QString& strMessage)
{
	int nResult = -1;
	if (QFailed(nResult = OpenPrinter(strMessage)))
	{
		gError() << gQStr(strMessage);
		return nResult;
	}
	if (QFailed(nResult = OpenReader(strMessage)))
	{
		gError() << gQStr(strMessage);
		return nResult;
	}
	return 0;
}

void uc_MakeCard::CloseDevice()
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

void uc_MakeCard::OnTimeout()
{
	ShutDown();
}

void  uc_MakeCard::ShutDown()
{
	CloseDevice();
	m_bWorkThreadRunning = false;
	if (m_pWorkThread && m_pWorkThread->joinable())
	{
		m_pWorkThread->join();
		m_pWorkThread = nullptr;
	}
}

/*
	typedef struct _PRINTERSTATUS
	{
		WORD fwDevice;			//打印机状态, 0-Ready；1-Busy；2-Offline；3-ErrMachine；4-Printing
		WORD fwMedia;			//介质状态，0-无卡；1-卡在门口；2-卡在内部；3-卡在上电位，4-卡在闸门外；5-堵卡；6-卡片未知（根据硬件特性返回,必须支持有无卡检测）
		WORD fwToner;			//平印色带状态,0-FLLL;1-LOW;2-OUT;3-NOTSUPP;4-UNKNOW
	}PRINTERSTATUS, * LPPRINTERSTATUS;
*/
void uc_MakeCard::ThreadWork()
{
	char szRCode[32] = { 0 };
	int nResult = 0;
	QString strMessage;
	Kingaotech::_PRINTERBOX boxesUnit[10];
	Kingaotech::BOXINFO boxesInfo = { 10, boxesUnit };
	Kingaotech::LPBOXINFO lpBoxInfo = &boxesInfo;
	Kingaotech::PRINTERSTATUS PrinterStatus;
	Kingaotech::LPPRINTERSTATUS lpPrinterStatus = &PrinterStatus;

	SSCardInfoPtr& pCardInfo = g_pDataCenter->GetSSCardInfo();
	CardFormPtr& pCardForm = g_pDataCenter->GetCardForm();
	char szBuffer[1024] = { 0 };
	int nBufferSize = sizeof(szBuffer);
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	while (m_bWorkThreadRunning)
	{
		if (!m_pPrinter)
			return;
		bool bSucceed = false;

		do
		{
			nBufferSize = sizeof(szBuffer);
			ZeroMemory(szBuffer, nBufferSize);

			if (QFailed(nResult = m_pPrinter->Printer_InitPrint(nullptr, szRCode)))
			{
				strMessage = QString("Printer_InitPrint失败，错误代码:%1!").arg(szRCode);
				break;
			}

			// int Printer_AddText(char* Text, int Angle, float Xpos, float Ypos, char* FontName, int FontSize, int FontStyle, int TextColor, char* resRcCode) = 0;
			TextPosition* pFieldPos = &pCardForm->posName;
			m_pPrinter->Printer_AddText((char*)pCardInfo->strName.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize, pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
			pFieldPos = &pCardForm->posIDNumber;
			m_pPrinter->Printer_AddText((char*)pCardInfo->strIDNumber.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize, pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
			pFieldPos = &pCardForm->posSSCardID;
			m_pPrinter->Printer_AddText((char*)pCardInfo->strSSCardNumber.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize, pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
			pFieldPos = &pCardForm->posIssueDate;
			m_pPrinter->Printer_AddText((char*)pCardInfo->strIssuedDate.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize, pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
			ImagePosition& ImgPos = pCardForm->posImage;
			m_pPrinter->Printer_AddImage((char*)pCardInfo->strPhotoPath.c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode);
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
			bSucceed = true;

		} while (0);
		if (!bSucceed)
		{
			gError() << strMessage.toLocal8Bit().data();
			emit ShowMaskWidget("操作失败", strMessage, Error, Stay_CurrentPage);
		}
		this_thread::sleep_for(chrono::milliseconds(200));
	}
}
