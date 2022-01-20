#pragma execution_character_set("utf-8")
#include "uc_makecard.h"
#include "ui_uc_makecard.h"
#include "MaskWidget.h"
extern MaskWidget* g_pMaskWindow;
#include "Gloabal.h"
#include "Payment.h"
#pragma warning(disable:4189)


uc_MakeCard::uc_MakeCard(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::MakeCard)
{
	ui->setupUi(this);
	m_nTimeout = 300;
}

uc_MakeCard::~uc_MakeCard()
{
	ShutDown();
	delete ui;
}

int uc_MakeCard::ProcessBussiness()
{
	/*if (g_pMaskWindow)
		g_pMaskWindow->hide();*/
	m_nSocketRetryInterval = g_pDataCenter->GetSysConfigure()->PaymentConfig.nQueryPayResultInterval;            // 支付结构查询时间间隔单 毫秒
	m_nSocketRetryCount = g_pDataCenter->GetSysConfigure()->PaymentConfig.nSocketRetryCount;
	int nResult = -1;
	QString strMessage;
	//if (QFailed(nResult = OpenDevice(strMessage)))
	//{
	//	gError() << strMessage.toLocal8Bit().data();
	//	emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
	//	return -1;
	//}
	//// 测试打印机，卡片是否已就绪
	//if (QFailed(nResult = TestPrinter(strMessage)))
	//{
	//	gError() << gQStr(strMessage);
	//	emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
	//	return nResult;
	//}
	if (!g_pDataCenter->GetPrinter())
	{
		if (QFailed(g_pDataCenter->OpenPrinter(strMessage)))
		{
			gError() << gQStr(strMessage);
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return nResult;
		}
	}
	if (!g_pDataCenter->GetSSCardReader())
	{
		if (QFailed(g_pDataCenter->OpenSSCardReader(strMessage)))
		{
			gError() << gQStr(strMessage);
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return nResult;
		}
	}
	m_pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!m_pWorkThread)
	{
		m_bWorkThreadRunning = true;
		m_pWorkThread = new std::thread(&uc_MakeCard::ThreadWork, this);
		if (!m_pWorkThread)
		{
			QString strError = QString("内存不足,创建制卡线程失败!");
			gError() << strError.toLocal8Bit().data();
			emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
			return -1;
		}
	}
	return 0;
}

//int uc_MakeCard::OpenPrinter(QString& strMessage)
//{
//	int nResult = -1;
//	char szRCode[32] = { 0 };
//	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
//	QString strAppPath = QCoreApplication::applicationDirPath();
//	QString strPrinterMudule = strAppPath + "/" + DevConfig.strPrinterModule.c_str();
//	try
//	{
//		do
//		{
//			if (!m_pPrinterlib)
//			{
//				m_pPrinterlib = make_shared<KTModule<KT_Printer>>(strPrinterMudule.toStdString());
//				if (!m_pPrinterlib)
//				{
//					strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strPrinterMudule);
//					break;
//				}
//			}
//			if (!m_pPrinter)
//			{
//				m_pPrinter = m_pPrinterlib->Instance();
//				if (!m_pPrinter)
//				{
//					strMessage = QString("创建‘%1’实例失败!").arg(strPrinterMudule);
//					break;
//				}
//				if (QFailed(nResult = m_pPrinter->Printer_Init(DevConfig.nPrinterType, szRCode)))
//				{
//					strMessage = QString("Printer_Init‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
//					break;
//				}
//
//				if (QFailed(nResult = m_pPrinter->Printer_Open(szRCode)))
//				{
//					strMessage = QString("Printer_Open‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
//					break;
//				}
//			}
//			else
//				return 0;
//
//		} while (0);
//		if (QFailed(nResult))
//			return -1;
//		else
//			return 0;
//	}
//	catch (std::exception& e)
//	{
//		strMessage = e.what();
//		gError() << gQStr(strMessage);
//		return -1;
//	}
//}

//string uc_MakeCard::MakeCardInfo(string strATR, SSCardInfoPtr& pSSCardInfo)
//{
//	string strCardInfo = "";
//	//RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
//	//卡识别码|初始化机构编码|发卡日期|卡有效期|卡号|社会保障号码|姓名|姓名拓展|性别|民族|出生日期|
//	strCardInfo = "||";	// 卡识别码|初始化机构编码 两者为空
//	//strCardInfo += pSSCardInfo->strOrganID;					strCardInfo += "|";
//	strCardInfo += pSSCardInfo->strReleaseDate;				strCardInfo += "|";
//	strCardInfo += pSSCardInfo->strValidDate;				strCardInfo += "|";
//	strCardInfo += pSSCardInfo->strCardNum;					strCardInfo += "|";
//	strCardInfo += pSSCardInfo->strCardID;					strCardInfo += "|";
//	strCardInfo += pSSCardInfo->strName;					strCardInfo += "||";//跳过姓名拓展
//	strCardInfo += pSSCardInfo->strSex;						strCardInfo += "|";
//	strCardInfo += pSSCardInfo->strNation;					strCardInfo += "|";
//	strCardInfo += pSSCardInfo->strBirthday;				strCardInfo += "|";
//	return strCardInfo;
//}

//int uc_MakeCard::TestPrinter(QString& strMessage)
//{
//	PRINTERSTATUS PrinterStatus;
//	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
//	int& nDepenseBox = DevConfig.nDepenseBox;
//
//	int nResult = -1;
//	BOXINFO BoxInfo;
//	ZeroMemory(&BoxInfo, sizeof(BOXINFO));
//	char szRCode[32] = { 0 };
//	do
//	{
//		if (QFailed(m_pPrinter->Printer_BoxStatus(BoxInfo, szRCode)))
//		{
//			strMessage = QString("Printer_BoxStatus失败:%1").arg(szRCode);
//			break;
//		}
//
//		if (BoxInfo.BoxList[nDepenseBox - 1].BoxStatus == 2)
//		{
//			strMessage = QString("卡箱无卡,请放入卡片!").arg(nDepenseBox).arg(BoxInfo.BoxList[nDepenseBox].BoxStatus);
//			break;
//		}
//
//		if (QFailed(m_pPrinter->Printer_Status(PrinterStatus, szRCode)))
//		{
//			strMessage = QString("Printer_Status失败，错误代码:%1!").arg(szRCode);
//			break;
//		}
//		if (PrinterStatus.fwDevice != 0)
//		{
//			strMessage = QString("打印机未就绪,状态代码:%1!").arg(PrinterStatus.fwDevice);
//			break;
//		}
//		// 0-无卡；2-卡在内部；3-卡在上电位，4-卡在闸门外 5-堵卡；6-卡片未知
//		bool bSucceed = false;
//		switch (PrinterStatus.fwMedia)
//		{
//		case 0:		// 机内无卡
//		{
//			bSucceed = true;
//			break;
//		}
//		case 1:			// 1-卡在门口:
//		{
//			strMessage = QString("打印机出卡号尚有未取走卡片,取走点击确定以继续!");
//			nResult = 1;		//
//			break;
//		}
//		case 2:			// 2-卡在内部；
//		case 3:			// 3-卡在上电位
//		{
//			if (QFailed(m_pPrinter->Printer_Retract(1, szRCode)))
//			{
//				strMessage = QString("打印机尚有未取走卡片,尝试将其移动到回收箱失败，错误代码:%1!").arg(szRCode);
//				break;
//			}
//			else
//				bSucceed = true;
//			break;
//		}
//		case 4:			// 4-卡在闸门外
//		{
//			strMessage = QString("请先取走出卡口的卡片!");
//			break;
//		}
//		case 5:
//		{
//			strMessage = QString("打印机堵卡,请联系技术人员处理!");
//			break;
//		}
//		case 6:
//		default:
//		{
//			strMessage = QString("发生未知错误,请联系技术人员处理!");
//			break;
//		}
//		}
//		if (!bSucceed)
//			break;
//		// 0-FLLL;1-LOW;2-OUT;3-NOTSUPP;4-UNKNOW
//		bSucceed = false;
//		switch (PrinterStatus.fwToner)
//		{
//		case 0:
//		case 1:
//		{
//			bSucceed = true;
//			break;
//		}
//		case 2:
//		{
//			strMessage = QString("打印机色带耗尽或未安装色带,状态代码:%1!").arg(PrinterStatus.fwToner);
//			break;
//		}
//		case 3:
//		{
//			strMessage = QString("打印机中安装了不兼容的色带,请更换色带!").arg(PrinterStatus.fwToner);
//			break;
//		}
//		case 4:
//		default:
//		{
//			strMessage = QString("发生未知错误!").arg(PrinterStatus.fwToner);
//			break;
//		}
//		}
//		if (!bSucceed)
//			break;
//	} while (0);
//	return nResult;
//
//}

//int uc_MakeCard::PrintCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage)
//{
//	char szBuffer[1024] = { 0 };
//	int nBufferSize = sizeof(szBuffer);
//	int nResult = -1;
//	char szRCode[32] = { 0 };
//	CardFormPtr& pCardForm = g_pDataCenter->GetCardForm();
//	do
//	{
//
//		nBufferSize = sizeof(szBuffer);
//		ZeroMemory(szBuffer, nBufferSize);
//
//		if (QFailed(nResult = m_pPrinter->Printer_InitPrint(nullptr, szRCode)))
//		{
//			strMessage = QString("Printer_InitPrint失败，错误代码:%1!").arg(szRCode);
//			break;
//		}
//
//		TextPosition* pFieldPos = &pCardForm->posName;
//		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strName, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
//		pFieldPos = &pCardForm->posIDNumber;
//		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardID, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
//		pFieldPos = &pCardForm->posSSCardID;
//		m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardNum, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
//		pFieldPos = &pCardForm->posIssueDate;
//		int nYear, nMonth, nDay;
//
//		sscanf_s(pSSCardInfo->strReleaseDate, "%04d%02d%02d", &nYear, &nMonth, &nDay);
//		char szReleaseDate[32] = { 0 };
//		sprintf_s(szReleaseDate, 32, "%d年%d月", nYear, nMonth);
//		m_pPrinter->Printer_AddText((char*)UTF8_GBK(szReleaseDate).c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
//		ImagePosition& ImgPos = pCardForm->posImage;
//		m_pPrinter->Printer_AddImage((char*)g_pDataCenter->strSSCardPhotoFile.c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode);
//		if (QFailed(m_pPrinter->Printer_StartPrint(szRCode)))
//		{
//			strMessage = QString("Printer_StartPrint失败，错误代码:%1!").arg(szRCode);
//			break;
//		}
//		if (QFailed(m_pPrinter->Printer_Eject(szRCode)))
//		{
//			strMessage = QString("Printer_Eject失败，错误代码:%1!").arg(szRCode);
//			break;
//		}
//		nResult = 0;
//
//	} while (0);
//	return nResult;
//}
//
//int uc_MakeCard::Depense(QString& strMessage)
//{
//
//}

//int uc_MakeCard::OpenSSCardReader(QString& strMessage)
//{
//	int nResult = -1;
//	try
//	{
//		do
//		{
//			char szRCode[32] = { 0 };
//			DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
//			QString strAppPath = QCoreApplication::applicationDirPath();
//			QString strReaderMudule = strAppPath + "/" + DevConfig.strReaderModule.c_str();
//			if (!m_pReaderLib)
//			{
//				m_pReaderLib = make_shared<KTModule<KT_Reader>>(strReaderMudule.toStdString());
//				if (!m_pReaderLib)
//				{
//					strMessage = strMessage = QString("内存不足，加载'%1'失败!").arg(strReaderMudule);
//					break;
//				}
//			}
//			if (!m_pReader)
//			{
//				m_pReader = m_pReaderLib->Instance();
//				if (!m_pReader)
//				{
//					strMessage = QString("创建'%1'实例失败!").arg(strReaderMudule);
//					break;
//				}
//				if (QFailed(nResult = m_pReader->Reader_Create(DevConfig.nSSCardReaderType, szRCode)))
//				{
//					strMessage = QString("Reader_Create'%1'失败,错误代码:%2").arg(DevConfig.strSSCardReadType.c_str()).arg(szRCode);
//					break;
//				}
//				if (QFailed(nResult = m_pReader->Reader_Init(szRCode)))
//				{
//					strMessage = QString("Reader_Init失败,错误代码:%2").arg(szRCode);
//					break;
//				}
//			}
//			RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
//			char szOutInfo[1024] = { 0 };
//			DriverInit(m_pReader, (char*)Region.strCountry.c_str(), (char*)Region.strSSCardDefaulutPin.c_str(), (char*)Region.strPrimaryKey.c_str(), szOutInfo);
//
//		} while (0);
//		if (QFailed(nResult))
//			return -1;
//		else
//			return 0;
//	}
//	catch (std::exception& e)
//	{
//		strMessage = e.what();
//		gError() << gQStr(strMessage);
//		return -1;
//	}
//	return 0;
//}

//int uc_MakeCard::OpenDevice(QString& strMessage)
//{
//	int nResult = -1;
//	if (QFailed(nResult = OpenPrinter(strMessage)))
//	{
//		gError() << gQStr(strMessage);
//		return nResult;
//	}
//
//	if (QFailed(nResult = OpenSSCardReader(strMessage)))
//	{
//		gError() << gQStr(strMessage);
//		return nResult;
//	}
//	return 0;
//}

//void uc_MakeCard::CloseDevice()
//{
//	char szRCode[32] = { 0 };
//
//	if (m_pPrinter)
//	{
//		m_pPrinter->Printer_Close(szRCode);
//		m_pPrinter = nullptr;
//	}
//
//	m_pPrinterlib = nullptr;
//
//	if (m_pReader)
//	{
//		m_pReader->Reader_Exit(szRCode);
//		m_pReader = nullptr;
//	}
//	m_pReaderLib = nullptr;
//}

void uc_MakeCard::OnTimeout()
{
	ShutDown();
}

void  uc_MakeCard::ShutDown()
{
	gInfo() << __FUNCTION__;
	m_bWorkThreadRunning = false;
	if (m_pWorkThread && m_pWorkThread->joinable())
	{
		m_pWorkThread->join();
		m_pWorkThread = nullptr;
	}
	g_pDataCenter->CloseDevice();
}


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
		if (QFailed(nResult = MarkCard(strMessage, nStatus, pSSCardInfo)))
		{
			if (strMessage == "已经开始制卡")
			{
				if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
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

		if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
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
		//#ifndef _DEBUG
		//		if (QFailed(nResult = queryPayResult(strMessage, nPayStatus)))
		//			break;
		//		if (nPayStatus == Pay_Not)
		//		{
		//			strMessage = "补卡费用尚未支付!";
		//			nResult = -1;
		//			break;
		//}
		//#endif

		nResult = ApplyCardReplacement(strMessage, nStatus, pSSCardInfo);     //  申请补换卡
		if (QFailed(nResult))
			break;
		if (nStatus != 0 && nStatus != 1)
		{
			nResult = -1;
			break;
		}

		nResult = ResgisterPayment(strMessage, nStatus, pSSCardInfo);          // 缴费登记
		if (QFailed(nResult))
			break;

		if (nStatus != 0 && nStatus != 1)
		{
			strMessage = QString("缴费登记失败:%1!").arg(nStatus);
			nResult = -1;
			break;
		}

		nResult = MarkCard(strMessage, nStatus, pSSCardInfo);
		if (QFailed(nResult))
		{
			strMessage = QString("即制卡标注失败,Result:%1!").arg(nResult);
			/*if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus, pSSCardInfo)))
				break;
			if (nStatus != 0 && nStatus != 1)
			{
				strMessage = QString("取消补换卡失败:%1!").arg(nStatus);
				nResult = -1;
				break;
			}*/
		}
		if (nStatus != 0 && nStatus != 1)
		{
			strMessage = QString("即制卡标注失败:%1!").arg(nStatus);
			nResult = -1;
			break;
		}

		if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
		{
			if (QFailed(nResult = CancelMarkCard(strMessage, nStatus, pSSCardInfo)))
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

			/*if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus, pSSCardInfo)))
			{
				strMessage = "因获取制卡数据失败,尝试取消补卡申请时再次失败!";
				break;
			}

			if (nStatus != 0 && nStatus != 1)
			{
				strMessage = QString("因获取制卡数据失败,尝试取消补换卡失败:%1!").arg(nStatus);
				nResult = -1;
				break;
			}*/
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

	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
	strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
	strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
	strcpy((char*)pSSCardInfo->strTransType, "5");
	strcpy((char*)pSSCardInfo->strCity, Reginfo.strCityCode.c_str());
	strcpy((char*)pSSCardInfo->strSSQX, Reginfo.strCountry.c_str());
	strcpy((char*)pSSCardInfo->strCard, Reginfo.strCardVendor.c_str());
	strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());

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

		if (QFailed(g_pDataCenter->Depense(strMessage)))
			break;
		strInfo = "进卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(g_pDataCenter->WriteCard(pSSCardInfo, strMessage)))
			break;
		strInfo = "写卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
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
			if (QFailed(nResult = CancelMarkCard(strMessage, nStatus, pSSCardInfo)))
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
		if (QFailed(nResult = ReturnCardData(strMessage, nStatus, pSSCardInfo)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
#pragma Warning("回盘失败如何处理？")
		if (nStatus != 0 && nStatus != 1)
			break;

		// 启用
		if (QFailed(nResult = EnalbeCard(strMessage, nStatus, pSSCardInfo)))
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
