#pragma execution_character_set("utf-8")
#include "uc_makecard.h"
#include "ui_uc_makecard.h"
#include "Gloabal.h"
#include "Payment.h"
#pragma warning(disable:4189)

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
	m_nSocketRetryInterval = g_pDataCenter->GetSysConfigure()->PaymentConfig.nQueryPayResultInterval;            // 支付结构查询时间间隔单 毫秒
	m_nSocketRetryCount = g_pDataCenter->GetSysConfigure()->PaymentConfig.nSocketRetryCount;

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
	Kingaotech::_PRINTERBOX boxesUnit[10];
	Kingaotech::BOXINFO boxesInfo = { 10, boxesUnit };
	Kingaotech::LPBOXINFO lpBoxInfo = &boxesInfo;
	Kingaotech::PRINTERSTATUS PrinterStatus;
	Kingaotech::LPPRINTERSTATUS lpPrinterStatus = &PrinterStatus;
	char szRCode[32] = { 0 };
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	try
	{
		do
		{

			QString strAppPath = QCoreApplication::applicationDirPath();
			QString strPrinterMudule = strAppPath + "/" + DevConfig.strPrinterModule.c_str();

			m_pPrinterlib = make_shared<KTModule<KT_Printer>>(strPrinterMudule.toStdString());
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

string uc_MakeCard::MakeCardInfo(string strATR, SSCardInfoPtr& pSSCardInfo)
{
	string strCardInfo = "";
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	//卡识别码|初始化机构编码|发卡日期|卡有效期|卡号|社会保障号码|姓名|姓名拓展|性别|民族|出生日期|
	strCardInfo = strATR;									strCardInfo += "|";
	strCardInfo += pSSCardInfo->strOrganID;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strReleaseDate;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strValidDate;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strCardID;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strCardNum;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strName;					strCardInfo += "|";
#pragma Warning("姓名拓展是什么鬼？")
	strCardInfo += pSSCardInfo->strName;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strSex;						strCardInfo += "|";
	strCardInfo += pSSCardInfo->strNation;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strBirthday;				strCardInfo += "|";
	return strCardInfo;
}

int uc_MakeCard::GetCA(string& strPublicKey, SSCardInfoPtr& pSSCardInfo, CAInfo& caInfo, QString& strMessage)
{
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	// *@param[in]  user		接口用户
	// * @param[in]  pwd		接口密码
	// * @param[in]  city		城市代码
	// * @param[in]  cardID		身份证号(社会保障号码)
	// * @param[in]  cardNum	社保卡号
	// * @param[in]  QMGY		签名公钥
	// * @param[in]  name		姓名
	// * @param[in]  SF			算法

#pragma Warning("算法是什么？来自哪里？")

	string strAlgorithm = "";

	int nResult = -1;
	while (true)
	{
		if (m_nSocketFailedCount >= m_nSocketRetryCount)
		{
			strMessage = QString("%1网络操作连续失败次超过限定次数,GetCA失败!").arg(__FUNCTION__);
			return -1;
		}

		if (QFailed(getCA(Region.strCMAccount.c_str(),
			Region.strCMPassword.c_str(),
			Region.strArea.c_str(),
			pSSCardInfo->strCardID,
			pSSCardInfo->strCardNum,
			strPublicKey.c_str(),
			pSSCardInfo->strName,
			strAlgorithm.c_str(),
			caInfo)))
		{
			m_nSocketFailedCount++;
			int nWaitTimems = 0;
			while (nWaitTimems < m_nSocketRetryInterval)
			{
				this_thread::sleep_for(chrono::milliseconds(100));
				m_nSocketRetryInterval += 100;
			}
		}
		else
		{
			m_nSocketFailedCount = 0;
			nResult = 0;
			break;
		}
	}
	return nResult;
}

int uc_MakeCard::WriteCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage)
{
	int nResult = -1;
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	char szCardATR[128] = { 0 };
	int  nCardATRLen = 0;
	char szRCode[32] = { 0 };
	char szCardBaseRead[256] = { 0 };
	char szCardBaseWrite[256] = { 0 };
	char szExAuthData[256] = { 0 };
	char szWHSM1[256] = { 0 };
	char szWHSM2[256] = { 0 };
	char szSignatureKey[256] = { 0 };
	char szWriteCARes[256] = { 0 };
	CAInfo caInfo;
	QString strInfo;
	HSMInfo  HsmInfo;

	do
	{
		if (!m_pReader)
		{
			strMessage = "读卡器未就绪!";
			break;
		}
		if (QFailed(m_pReader->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("读卡器上电失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		gInfo() << "CardATR:" << szCardATR;
		if (QFailed(nResult = iReadCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseRead)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		gInfo() << "CardBaseRead:" << szCardBaseRead;
		if (QFailed(nResult = iWriteCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseWrite)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		gInfo() << "CardBaseRead:" << szCardBaseWrite;
#pragma Warning("HsmInfo信息未填充")
		if (QFailed(nResult = cardExternalAuth(HsmInfo, szExAuthData)))
		{
			strMessage = QString("卡版外部信息认证失败,Result:%1").arg(nResult);
			break;
		}
		string strCardInfo = MakeCardInfo(string(szCardATR), pSSCardInfo);
		gInfo() << "strCardInfo = " << strCardInfo;
		if (QFailed(nResult = iWriteCardBas_HSM_Step1((char*)szExAuthData, (char*)strCardInfo.c_str(), szWHSM1)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		gInfo() << "szWHSM1 = " << szWHSM1;
		if (QFailed(nResult = iReadSignatureKey(DevConfig.nSSCardReaderPowerOnType, szSignatureKey)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		gInfo() << "szSignatureKey = " << szSignatureKey;

		string strPublicKey = szSignatureKey;
		if (QFailed(nResult = GetCA(strPublicKey, pSSCardInfo, caInfo, strMessage)))
		{
			strMessage = QString("获取私钥失败,%1").arg(strMessage);
			break;
		}
		// 		QMZS：签名证书 ,JMZS：加密证书 ,JMMY：加密密钥 ,GLYPIN：管理员pin ,ZKMY：主控密钥 ,pOutInfo 传出信息
		if (QFailed(nResult = iWriteCA(DevConfig.nSSCardReaderPowerOnType, caInfo.QMZS, caInfo.JMZS, caInfo.JMMY, caInfo.GLYPIN, caInfo.ZKMY, szWriteCARes)))
		{
			strMessage = QString("写CA信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			break;
		}
		gInfo() << "szWriteCA Out = " << szWriteCARes;
		nResult = 0;

	} while (0);
	return nResult;
}

int uc_MakeCard::PrintCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage)
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
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strName, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize, pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
		pFieldPos = &pCardForm->posIDNumber;
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardID, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize, pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
		pFieldPos = &pCardForm->posSSCardID;
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardNum, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize, pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
		pFieldPos = &pCardForm->posIssueDate;
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strValidDate, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize, pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
		ImagePosition& ImgPos = pCardForm->posImage;
		m_pPrinter->Printer_AddImage((char*)g_pDataCenter->strSSCardPhotoFile.c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode);
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

int uc_MakeCard::Depense(QString& strMessage)
{
	int nResult = -1;
	Kingaotech::_PRINTERBOX boxesUnit[10];
	Kingaotech::BOXINFO boxesInfo = { 10, boxesUnit };
	Kingaotech::LPBOXINFO lpBoxInfo = &boxesInfo;
	Kingaotech::PRINTERSTATUS PrinterStatus;
	Kingaotech::LPPRINTERSTATUS lpPrinterStatus = &PrinterStatus;
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	char szRCode[32] = { 0 };
	do
	{
		if (!m_pPrinter)
		{
			strMessage = "打印机未初始化";
			break;
		}
		if (QFailed(m_pPrinter->Printer_BoxStatus(lpBoxInfo, szRCode)))
		{
			strMessage = QString("Printer_BoxStatus失败:%1").arg(szRCode);
			break;
		}

		if (lpBoxInfo->BoxList[DevConfig.nDepenseBox].BoxStatus == 2)
		{
			strMessage = QString("卡箱[%1]:BoxStatus = %2,无卡!").arg(DevConfig.nDepenseBox).arg(lpBoxInfo->BoxList[DevConfig.nDepenseBox].BoxStatus);
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
		if (DevConfig.nSSCardReaderPowerOnType == 2)
			nDepensePos = 3;

		if (QFailed(m_pPrinter->Printer_Dispense(DevConfig.nDepenseBox, nDepensePos, szRCode)))
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
#pragma Warning("需要处理超时未取走卡片,把卡片放回收箱吗？")
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

	char szBuffer[1024] = { 0 };
	int nBufferSize = sizeof(szBuffer);
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	int nStatus = 0;

	do
	{
		if (QFailed(Depense(strMessage)))
			break;

		if (QFailed(WriteCard(pCardInfo, strMessage)))
			break;

		if (QFailed(PrintCard(pCardInfo, strMessage)))
			break;
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		gInfo() << gQStr(strMessage);
		do
		{
			gInfo() << "Try to CancelMarkCard";
			if (QFailed(CancelMarkCard(strMessage, nStatus)))
			{
				gInfo() << gQStr(strMessage);
				break;
			}
#pragma Warning("需要处理取消标注的状态")
			if (QFailed(CancelCardReplacement(strMessage, nStatus)))
			{
				gInfo() << gQStr(strMessage);
				break;
			}
#pragma Warning("需要处理取消补换卡的状态")
		} while (0);

		gError() << strMessage.toLocal8Bit().data();
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}
	gInfo() << gQStr(QString("写卡，打印成功"));
	nResult = -1;
	do
	{
		// 数据回盘
		if (QFailed(ReturnCardData(strMessage, pCardInfo)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
		// 启用
		if (QFailed(EnalbeCard(strMessage, nStatus)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}
	else
	{
		emit ShowMaskWidget("提示", "制止成功,请及时取走卡片", Fetal, Return_MainPage);
	}
}
