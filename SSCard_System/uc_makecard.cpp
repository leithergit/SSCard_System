#pragma execution_character_set("utf-8")
#include "uc_makecard.h"
#include "ui_uc_makecard.h"
#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;
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
	/*if (g_pMaskWindow)
		g_pMaskWindow->hide();*/
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
			if (!m_pPrinterlib)
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

				if (QFailed(nResult = m_pPrinter->Printer_Open(szRCode)))
				{
					strMessage = QString("Printer_Open‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
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

string uc_MakeCard::MakeCardInfo(string strATR, SSCardInfoPtr& pSSCardInfo)
{
	string strCardInfo = "";
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	//卡识别码|初始化机构编码|发卡日期|卡有效期|卡号|社会保障号码|姓名|姓名拓展|性别|民族|出生日期|
	strCardInfo = "||";	// 卡识别码|初始化机构编码 两者为空
	//strCardInfo += pSSCardInfo->strOrganID;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strReleaseDate;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strValidDate;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strCardNum;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strCardID;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strName;					strCardInfo += "||";//跳过姓名拓展
	strCardInfo += pSSCardInfo->strSex;						strCardInfo += "|";
	strCardInfo += pSSCardInfo->strNation;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strBirthday;				strCardInfo += "|";
	return strCardInfo;
}

//int uc_MakeCard::GetCA(string& strPublicKey, SSCardInfoPtr& pSSCardInfo, CAInfo& caInfo, QString& strMessage)
//{
//	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
//	// *@param[in]  user		接口用户
//	// * @param[in]  pwd		接口密码
//	// * @param[in]  city		城市代码
//	// * @param[in]  cardID		身份证号(社会保障号码)
//	// * @param[in]  cardNum	社保卡号
//	// * @param[in]  QMGY		签名公钥
//	// * @param[in]  name		姓名
//	// * @param[in]  SF			算法
//}

int uc_MakeCard::WriteCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage)
{
	int nResult = -1;
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;

	char szCardATR[128] = { 0 };
	int  nCardATRLen = 0;
	char szRCode[32] = { 0 };
	char szCardBaseRead[1024] = { 0 };
	char szCardBaseWrite[1024] = { 0 };
	char szExAuthData[1024] = { 0 };
	char szWHSM1[1024] = { 0 };
	char szWHSM2[1024] = { 0 };
	char szSignatureKey[1024] = { 0 };
	char szWriteCARes[1024] = { 0 };
	CAInfo caInfo;
	QString strInfo;
	HSMInfo  HsmInfo;
	int nStatus = 0;

	do
	{
#ifdef _DEBUG
#pragma  Warning("直接读取社保卡数据，用于测试回盘，事后须删除！")
		GetCardData(strMessage, nStatus);
#endif
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
		strcpy(pSSCardInfo->strCardATR, szCardATR);
		char szBankNum[64] = { 0 };
		if (QFailed(nResult = iReadBankNumber(DevConfig.nSSCardReaderPowerOnType, szBankNum)))
		{
			strMessage = QString("读很卡信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			break;
		}
		gInfo() << "BankNum:" << szBankNum;
		strcpy(pSSCardInfo->strBankNum, szBankNum);

		gInfo() << "CardATR:" << szCardATR;
		if (QFailed(nResult = iReadCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseRead)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}

		/*
		发卡地区行政区划代码（卡识别码前6位）、社会保障号码、 卡号、 卡识别码、 姓名、 卡复位信息（仅取历史字节）、 规范版本、 发卡日期、 卡有效期、终端机编号、终端设备号。各数据项之间以 “ | ” 分割，且最后一个数据项以 “ I,, 结尾。
		639900|l111111198101011110|X00000019|639900D15600000500BF7C7A48FB4966|张三|00814E43238697159900BF7C7A|1.00|20101001
		区号|社会保障号码(空)|卡号(空)|卡识别码|姓名(空)|卡复位信息(ATR)|Ver|发卡日期|有效期限|
		411600|||411600D156000005C38275EEFC9B9648||008C544CB386844116018593CA|3.00|20211203|20311202|
		*/

		QStringList strFieldList = QString(szCardBaseRead).split("|", Qt::KeepEmptyParts);
		int nIndex = 0;
		for (auto var : strFieldList)
		{
			qDebug() << "Field[" << nIndex << "]" << var;
			nIndex++;
		}
		gInfo() << "CardBaseRead:" << szCardBaseRead;

		string strCardIdentify = strFieldList[3].toStdString();
		strcpy(pSSCardInfo->strIdentifyNum, strCardIdentify.c_str());

		if (QFailed(nResult = iWriteCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseWrite)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		// szCardBaseWrite输出结果为两个随机数
		// random1|random2
		gInfo() << "CardBaseRead:" << szCardBaseWrite;

		QStringList strRomdom = QString(szCardBaseWrite).split("|", Qt::KeepEmptyParts);

		strcpy(HsmInfo.strSystemID, "410700006");
		strcpy(HsmInfo.strRegionCode, Region.strArea.c_str());
		strcpy(HsmInfo.strCardNum, pSSCardInfo->strCardNum); //用最新的卡号	   
		strcpy(HsmInfo.strTerminalCode, DevConfig.strTerminalCode.c_str());	// 
		strcpy(HsmInfo.strCardID, pSSCardInfo->strCardID);
		strcpy(HsmInfo.strBusinessType, "026");	// 写卡为026，读卡为028
		strcpy(HsmInfo.strIdentifyNum, strCardIdentify.c_str());
		strcpy(HsmInfo.strName, pSSCardInfo->strName);
		strcpy(HsmInfo.strCardATR, szCardATR);
		strcpy(HsmInfo.stralgoCode, "03");
		strcpy(HsmInfo.strKeyAddr, "0094");
		strcpy(HsmInfo.strReleaseDate, pSSCardInfo->strReleaseDate);
		strcpy(HsmInfo.strValidDate, pSSCardInfo->strValidDate);
		strcpy(HsmInfo.strSex, pSSCardInfo->strSex);
		strcpy(HsmInfo.strNation, pSSCardInfo->strNation);
		strcpy(HsmInfo.strBirthday, pSSCardInfo->strBirthday);
		strcpy(HsmInfo.strRandom1, strRomdom[1].toStdString().c_str());
		strcpy(HsmInfo.strRandom2, strRomdom[2].toStdString().c_str());

		if (QFailed(nResult = cardExternalAuth(HsmInfo, szExAuthData)))
		{
			strMessage = QString("卡版外部信息认证失败,Result:%1").arg(nResult);
			break;
		}
		gInfo() << "szExAuthData = " << szExAuthData;
		string strCardInfo = MakeCardInfo(string(szCardATR), pSSCardInfo);
		gInfo() << "strCardInfo = " << strCardInfo;
		if (QFailed(nResult = iWriteCardBas_HSM_Step1((char*)szExAuthData, (char*)strCardInfo.c_str(), szWHSM1)))
		{
			strMessage = QString("iWriteCardBas_HSM_Step1,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		gInfo() << "szWHSM1 = " << szWHSM1;
		if (QFailed(nResult = iReadSignatureKey(DevConfig.nSSCardReaderPowerOnType, szSignatureKey)))
		{
			strMessage = QString("iReadSignatureKey,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		gInfo() << "szSignatureKey = " << szSignatureKey;
		string strPublicKey = szSignatureKey;
		string strAlgorithm = "03";

		if (QFailed(nResult = GetCA(strMessage, nStatus, pSSCardInfo, strPublicKey.c_str(), strAlgorithm.c_str(), caInfo)))
		{
			strMessage = QString("GetCA,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		if (nStatus != 0 && nStatus != 1)
		{
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
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strName, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		pFieldPos = &pCardForm->posIDNumber;
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardID, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		pFieldPos = &pCardForm->posSSCardID;
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardNum, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		pFieldPos = &pCardForm->posIssueDate;
		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strValidDate, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
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

	Kingaotech::LPBOXINFO lpBoxInfo = new Kingaotech::LBOXINFO;
	Kingaotech::LPPRINTERSTATUS lpPrinterStatus = new Kingaotech::PRINTERSTATUS;
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

	if (lpBoxInfo)
		delete lpBoxInfo;

	if (lpPrinterStatus)
		delete lpPrinterStatus;

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

			if (!m_pReaderLib)
			{
				char szRCode[32] = { 0 };
				DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
				QString strAppPath = QCoreApplication::applicationDirPath();
				QString strReaderMudule = strAppPath + "/" + DevConfig.strReaderModule.c_str();

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
				RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
				char szOutInfo[1024] = { 0 };
				DriverInit(m_pReader, (char*)Region.strArea.c_str(), (char*)Region.strSSCardDefaulutPin.c_str(), (char*)Region.strPrimaryKey.c_str(), szOutInfo);
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

int uc_MakeCard::PrecessCardInMaking(QString& strMessage)
{
	gInfo() << __FUNCTION__;
	int nStatus = 0;
	int nResult = 0;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	QString strCardProgress = QString::fromLocal8Bit(pSSCardInfo->strCardStatus);

	//#ifdef _DEBUG
	//#pragma Warning("测试阶段使用测试人员身份信息")
	//	QString strName, strID, strMobile;
	//	if (QSucceed(LoadTestData(strName, strID, strMobile)))
	//	{
	//		gInfo() << "Succeed in load Test Card Data:" << gQStr(strName) << gQStr(strID) << gQStr(strMobile);
	//		strcpy((char*)pIDCard->szName, strName.toLocal8Bit().data());
	//		strcpy((char*)pSSCardInfo->strName, strName.toLocal8Bit().data());
	//		strcpy((char*)pIDCard->szIdentify, strID.toStdString().c_str());
	//		strcpy((char*)pSSCardInfo->strCardID, strID.toStdString().c_str());
	//		strcpy((char*)pSSCardInfo->strMobile, strMobile.toStdString().c_str());
	//	}
	//#endif

	do
	{
		if (QFailed(nResult = MarkCard(strMessage, nStatus)))
		{
			if (strMessage == "已经开始制卡")
			{
				if (QFailed(nResult = GetCardData(strMessage, nStatus)))
				{
					break;
					/*if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus)))
						break;*/
				}

				if (nStatus != 0 && nStatus != 1)
				{
					nResult = -1;
					break;
				}
				break;
			}
			else
				break;
			//if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus)))
			//	break;
		}
		if (nStatus != 0 && nStatus != 1)
		{
			nResult = -1;
			break;
		}
		//if (nStatus == 1)
		//{// 尝试取消标注
		//	if (QFailed(nResult = CancelMarkCard(strMessage, nStatus)))
		//		break;

		//	// 重新标注
		//	if (QFailed(nResult = MarkCard(strMessage, nStatus)))
		//		break;
		//	if (nStatus != 0 && nStatus != 1)
		//	{
		//		nResult = -1;
		//		break;
		//	}
		//}

		if (QFailed(nResult = GetCardData(strMessage, nStatus)))
		{
			break;
			/*if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus)))
				break;*/
		}

		if (nStatus != 0 && nStatus != 1)
		{
			nResult = -1;
			break;
		}
	} while (0);
	return nResult;
}


int uc_MakeCard::PrepareMakeCard(QString& strMessage)
{
	int nStatus = 0;
	int nResult = -1;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	int nPayStatus = Pay_Not;
	do
	{
#ifndef _DEBUG
		if (QFailed(nResult = QueryPayment(strMessage, nPayStatus)))
			break;
		if (nPayStatus == Pay_Not)
		{
			strMessage = "补卡费用尚未支付!";
			nResult = -1;
			break;
		}
#endif

		nResult = ApplyCardReplacement(strMessage, nStatus);     //  申请补换卡
		if (QFailed(nResult))
			break;
		if (nStatus != 0 && nStatus != 1)
		{
			nResult = -1;
			break;
		}

		nResult = ResgisterPayment(strMessage, nStatus);          // 缴费登记
		if (QFailed(nResult))
			break;

		if (nStatus != 0 && nStatus != 1)
		{
			strMessage = QString("缴费登记失败:%1!").arg(nStatus);
			nResult = -1;
			break;
		}

		nResult = MarkCard(strMessage, nStatus);
		if (QFailed(nResult))
		{
			if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus)))
				break;
			if (nStatus != 0 && nStatus != 1)
			{
				strMessage = QString("取消补换卡失败:%1!").arg(nStatus);
				nResult = -1;
				break;
			}
		}
		if (nStatus != 0 && nStatus != 1)
		{
			strMessage = QString("即制卡标注失败:%1!").arg(nStatus);
			nResult = -1;
			break;
		}

		if (QFailed(nResult = GetCardData(strMessage, nStatus)))
		{
			if (QFailed(nResult = CancelMarkCard(strMessage, nStatus)))
			{
				strMessage = "因获取制卡数据失败,尝试取消即制卡标注时再次失败!";
				break;
			}
			if (nStatus != 0 && nStatus != 1)
			{
				strMessage = QString("因获取制卡数据失败,尝试取消即制卡标失败:%1!").arg(strMessage);
				nResult = -1;
				break;
			}

			if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus)))
			{
				strMessage = "因获取制卡数据失败,尝试取消补卡申请时再次失败!";
				break;
			}

			if (nStatus != 0 && nStatus != 1)
			{
				strMessage = QString("因获取制卡数据失败,尝试取消补换卡失败:%1!").arg(nStatus);
				nResult = -1;
				break;
			}
		}
		if (nStatus != 0 && nStatus != 1)
		{
			nResult = -1;
			break;
		}
	} while (0);
	return nResult;

}
void uc_MakeCard::ThreadWork()
{
	char szRCode[32] = { 0 };
	int nResult = -1;
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
	QString strInfo;

	do
	{
		if (g_pDataCenter->strCardMakeProgress == "制卡中")
		{
			if (QFailed(PrecessCardInMaking(strMessage)))
				break;
		}
		else
		{
			if (QFailed(PrepareMakeCard(strMessage)))
			{
				break;
			}
		}

		if (QFailed(Depense(strMessage)))
			break;
		strInfo = "进卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(WriteCard(pCardInfo, strMessage)))
			break;
		strInfo = "写卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(PrintCard(pCardInfo, strMessage)))
			break;
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		gInfo() << gQStr(strMessage);
		do
		{
			gInfo() << "Try to CancelMarkCard";
			if (QFailed(nResult = CancelMarkCard(strMessage, nStatus)))
			{
				strMessage = QString("取消标注失败:%1").arg(strMessage);
				gInfo() << gQStr(strMessage);
				break;
			}
			//			目前在河南无权取消
			// 			if (QFailed(CancelCardReplacement(strMessage, nStatus)))
			// 			{
			// 				gInfo() << gQStr(strMessage);
			// 				break;
			// 			}
			//#pragma Warning("需要处理取消补换卡的状态")
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
		if (QFailed(nResult = ReturnCardData(strMessage, nStatus, pCardInfo)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
#pragma Warning("回盘失败如何处理？")
		if (nStatus != 0 && nStatus != 1)
			break;

		// 启用
		if (QFailed(nResult = EnalbeCard(strMessage, nStatus)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
		if (nStatus != 0 && nStatus != 1)
			break;
#pragma Warning("启用卡片失败如何处理？")
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return;
	}
	else
	{
		emit ShowMaskWidget("提示", "制卡成功,请及时取走卡片", Success, Return_MainPage);
	}
}
