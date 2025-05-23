﻿#pragma execution_character_set("utf-8")
#include <QSettings>
#include <QDir>
#include <QApplication>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QTextCodec>
#include "Gloabal.h"
#include "Payment.h"
#include "aestools.h"
#include "../update/Update.h"
#include "qstackpage.h"
#include "idcard_api.h"
#include "mainwindow.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")
#pragma comment(lib,"Version.lib")
#pragma comment(lib,"../sdk/FaceCapture/DVTGKLDCamSDK.lib")
#pragma comment(lib,"../sdk/BugTrap/BugTrapU.lib")
#ifdef _DEBUG
#pragma comment(lib, "../SDK/KT_Printer/KT_Printerd")
#pragma comment(lib, "../SDK/KT_Reader/KT_Readerd")
#pragma comment(lib, "../SDK/SSCardDriver/SSCardDriverd")
#pragma comment(lib, "../SDK/SSCardHSM/SSCardHSMd")
#pragma comment(lib, "../SDK/SSCardInfo_Henan/SSCardInfod")
#pragma comment(lib, "../SDK/libcurl/libcurld")
#pragma comment(lib, "../SDK/QREncode/qrencoded")
#pragma comment(lib, "../SDK/IDCard/IDCard_API")
#pragma comment(lib, "../SDK/glog/glogd")
#pragma comment(lib, "../SDK/PinKeybroad/XZ_F31_API")
#pragma comment(lib, "../SDK/7Z/lib/bit7z_d.lib")
#pragma comment(lib, "../SDK/Update/Debug/update")
#else
#pragma comment(lib, "../SDK/KT_Printer/KT_Printer")
#pragma comment(lib, "../SDK/KT_Reader/KT_Reader")
#pragma comment(lib, "../SDK/SSCardDriver/SSCardDriver")
#pragma comment(lib, "../SDK/SSCardHSM/SSCardHSM")
#pragma comment(lib, "../SDK/SSCardInfo_Henan/SSCardInfo")
#pragma comment(lib, "../SDK/libcurl/libcurl")
#pragma comment(lib, "../SDK/QREncode/qrencode")
#pragma comment(lib, "../SDK/IDCard/IDCard_API")
#pragma comment(lib, "../SDK/glog/glog")
#pragma comment(lib, "../SDK/PinKeybroad/XZ_F31_API")
#pragma comment(lib, "../SDK/7Z/lib/bit7z.lib")
#pragma comment(lib, "../SDK/Update/release/update")
#endif

const char* szPrinterTypeList[PRINTER_MAX] =
{
	Str(EVOLIS_KC200),
	Str(EVOLIS_ZENIUS),
	Str(EVOLIS_AVANSIA),
	Str(HITI_CS200),
	Str(HITI_CS220),
	Str(HITI_CS290),
	Str(ENTRUCT_EM1),
	Str(ENTRUCT_EM2),
	Str(ENTRUCT_CD809)
};

const char* szReaderTypeList[READER_MAX + 1] =
{
	Str(IN_VALID),
	Str(DC_READER),//德卡读卡器
	Str(MH_READER),//明华读卡器
	Str(HD_READER),//华大读卡器
};
QWaitCursor::QWaitCursor()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	bStart = true;
}
void QWaitCursor::RestoreCursor()
{
	QApplication::restoreOverrideCursor();
	bStart = false;
}
QWaitCursor::~QWaitCursor()
{
	if (bStart)
		QApplication::restoreOverrideCursor();
}

extern DataCenterPtr g_pDataCenter;
MainWindow* g_pMainWnd = nullptr;
QScreen* g_pCurScreen = nullptr;
const char* szAesKey = "581386C9F1514F6B92170BF457D8B065";
vector<NationaltyCode> g_vecNationCode = {
			{"01", "汉族"},
			{ "02","蒙古族" },
			{ "03","回族" },
			{ "04","藏族" },
			{ "05","维吾尔族" },
			{ "06","苗族" },
			{ "07","彝族" },
			{ "08","壮族" },
			{ "09","布依族" },
			{ "10","朝鲜族" },
			{ "11","满族" },
			{ "12","侗族" },
			{ "13","瑶族" },
			{ "14","白族" },
			{ "15","土家族" },
			{ "16","哈尼族" },
			{ "17","哈萨克族" },
			{ "18","傣族" },
			{ "19","黎族" },
			{ "20","傈傈族" },
			{ "21","佤族" },
			{ "22","畲族" },
			{ "23","高山族" },
			{ "24","拉祜族" },
			{ "25","水族" },
			{ "26","东乡族" },
			{ "27","纳西族" },
			{ "28","景颇族" },
			{ "29","柯尔克孜族" },
			{ "30","土族" },
			{ "31","达翰尔族" },
			{ "32","仫佬族" },
			{ "33","羌族" },
			{ "34","布朗族" },
			{ "35","撒拉族" },
			{ "36","毛南族" },
			{ "37","仡佬族" },
			{ "38","锡伯族" },
			{ "39","阿昌族" },
			{ "40","普米族" },
			{ "41","塔吉克族" },
			{ "42","怒族" },
			{ "43","乌孜别克族" },
			{ "44","俄罗斯族" },
			{ "45","鄂温克族" },
			{ "46","德昂族" },
			{ "47","保安族" },
			{ "48","裕固族" },
			{ "49","京族" },
			{ "50","塔塔尔族" },
			{ "51","独龙族" },
			{ "52","鄂伦春族" },
			{ "53","赫哲族" },
			{ "54","门巴族" },
			{ "55","珞巴族" },
			{ "56","基诺族" },
			{ "57","穿青人" },
			{ "90","外籍人士" },
			{ "99","其他" }
};
DataCenter::DataCenter()
{
	DVTGKLDCam_Init();
}

DataCenter::~DataCenter()
{
}
// 
int DataCenter::LoadSysConfigure(QString& strError)
{
	try
	{
		CloseDevice();
		gInfo() << "Try to load configure.ini";
		QString strConfigPath = QCoreApplication::applicationDirPath();
		strConfigPath += "/Configure.ini";
		QFileInfo fi(strConfigPath);
		if (!fi.isFile())
		{
			strError = QString("加载配置文件失败:%1!").arg(strConfigPath);
			return -1;
		}
		QSettings ConfigIni(strConfigPath, QSettings::IniFormat);
		pSysConfig = make_shared<SysConfig>(&ConfigIni);
		if (!pSysConfig)
		{
			strError = "内存不足，无法初始化DataCenter类实例!";
			return -1;
		}
		bDebug = GetSysConfigure()->bDebug;
		bSkipFaceCapture = GetSysConfigure()->bSkipFaceCapture;
		bEnableUpdate = GetSysConfigure()->bEnableUpdate;
		nSkipPayTime = GetSysConfigure()->nSkipPayTime;
		bTestCard = GetSysConfigure()->bTestCard;
		nNetTimeout = GetSysConfigure()->nNetTimeout;
		strTitle = GetSysConfigure()->strTitle;
		return 0;
	}

	catch (std::exception& e)
	{
		strError = "Catch an exception:";
		strError += e.what();
		gError() << strError.toLatin1().data();
		return -1;
	}
}

int DataCenter::LoadCardForm(QString& strError)
{
	try
	{
		QString strConfigPath = QCoreApplication::applicationDirPath();
		strConfigPath += "/CardForm.ini";
		QFileInfo fi(strConfigPath);
		if (!fi.isFile())
		{
			strError = QString("加载卡片打印版式失败:%1!").arg(strConfigPath);
			return -1;
		}
		QSettings ConfigIni(strConfigPath, QSettings::IniFormat);
		pCardForm = make_shared<CardForm>(&ConfigIni);
		if (!pCardForm)
		{
			strError = "内存不足，无法初始化数据中心!";
			return -1;
		}
		return 0;
	}

	catch (std::exception& e)
	{
		strError = "Catch an exception:";
		strError += e.what();
		gError() << strError.toLatin1().data();
		return -1;
	}
}

int QMessageBox_CN(QMessageBox::Icon nIcon, QString strTitle, QString strText, QMessageBox::StandardButtons stdButtons, QWidget* parent)
{
	QMessageBox Msgbox(nIcon, strTitle, strText, stdButtons, parent);
	Msgbox.setStandardButtons(stdButtons);
	struct ButtonInfo
	{
		QMessageBox::StandardButton nBtn;
		QString     strText;
	};

	static std::map<QMessageBox::StandardButton, QString>  ButtonMap =
	{
		{QMessageBox::NoButton          ,""},
		{QMessageBox::Ok                ,QObject::tr("确定")},
		{QMessageBox::Save              ,QObject::tr("保存")},
		{QMessageBox::SaveAll           ,QObject::tr("全部保存")},
		{QMessageBox::Open              ,QObject::tr("打开")},
		{QMessageBox::Yes               ,QObject::tr("是")},
		{QMessageBox::YesToAll          ,QObject::tr("所有都是")},
		{QMessageBox::No                ,QObject::tr("否")},
		{QMessageBox::NoToAll           ,QObject::tr("所有都不")},
		{QMessageBox::Abort             ,QObject::tr("中止")},
		{QMessageBox::Retry             ,QObject::tr("重试")},
		{QMessageBox::Ignore            ,QObject::tr("忽略")},
		{QMessageBox::Close             ,QObject::tr("关闭")},
		{QMessageBox::Cancel            ,QObject::tr("取消")},
		{QMessageBox::Discard           ,QObject::tr("放弃")},
		{QMessageBox::Help              ,QObject::tr("帮忙")},
		{QMessageBox::Apply             ,QObject::tr("应用")},
		{QMessageBox::Reset             ,QObject::tr("复位")},
		{QMessageBox::RestoreDefaults   ,QObject::tr("恢复默认")},
		{QMessageBox::FirstButton       ,QObject::tr("首个按钮")},
		{QMessageBox::LastButton        ,QObject::tr("末尾按钮")},
		{QMessageBox::YesAll            ,QObject::tr("全是")},
		{QMessageBox::NoAll             ,QObject::tr("全否")},
		{QMessageBox::Default           ,QObject::tr("默认")},
		{QMessageBox::Escape            ,QObject::tr("取消操作")},
		{QMessageBox::FlagMask          ,""},
		{QMessageBox::ButtonMask        ,""}         // obsolete
	};

	for (auto btnType : ButtonMap)
	{
		if ((btnType.first & stdButtons) == btnType.first)
		{
			Msgbox.setButtonText(btnType.first, btnType.second);
		}
	}
	return Msgbox.exec();
}

string UTF8_GBK(const char* strUtf8)
{
	QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
	QTextCodec* GBKCodec = QTextCodec::codecForName("GB18030");
	QString strUnicode = utf8Codec->toUnicode(strUtf8);
	QByteArray ByteGb2312 = GBKCodec->fromUnicode(strUnicode);
	return string(ByteGb2312.data(), ByteGb2312.size());
}

string GBK_UTF8(const char* strGBK)
{
	QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
	QTextCodec* GBKCodec = QTextCodec::codecForName("GB18030");
	QString strUnicode = GBKCodec->toUnicode(strGBK);
	QByteArray ByteUTF8 = utf8Codec->fromUnicode(strUnicode);
	return string(ByteUTF8.data(), ByteUTF8.size());
}

void  SetTableWidgetItemChecked(QTableWidget* pTableWidget, int nRow, int nCol, QButtonGroup* pButtonGrp, int nItemID, bool bChecked)
{
	QWidget* pCellWidget = new QWidget(pTableWidget);
	QHBoxLayout* pHLayout = new QHBoxLayout();
	pHLayout->setMargin(0);
	pHLayout->setSpacing(0);

	QCheckBox* pCheckBox = new QCheckBox();
	pCheckBox->setChecked(false);
	pCheckBox->setFont(pTableWidget->font());
	pCheckBox->setFocusPolicy(Qt::NoFocus);
	pCheckBox->setStyle(QStyleFactory::create("fusion"));
	pCheckBox->setStyleSheet(QString(".QCheckBox {margin:3px;border:0px;}QCheckBox::indicator {width: %1px; height: %1px; }").arg(20));

	pHLayout->addWidget(pCheckBox);
	pHLayout->setAlignment(pCheckBox, Qt::AlignCenter);
	pCellWidget->setLayout(pHLayout);
	//pCheckBox->setEnabled(false);
	pButtonGrp->addButton(pCheckBox, nItemID);
	pTableWidget->setCellWidget(nRow, nCol, pCellWidget);
	if (bChecked)
		pCheckBox->setCheckState(Qt::Checked);
}

QStringList SearchFiles(const QString& dir_path, QDateTime* pStart, QDateTime* pStop)
{
	QStringList FileList;
	QDir dir(dir_path);
	if (!dir.exists())
	{
		qDebug() << "it is not true dir_path";
	}

	/*设置过滤参数，QDir::NoDotAndDotDot表示不会去遍历上层目录*/
	QDirIterator dir_iterator(dir_path, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

	while (dir_iterator.hasNext())
	{
		dir_iterator.next();
		QFileInfo file_info = dir_iterator.fileInfo();
		QString files = file_info.absoluteFilePath();
		if (pStart && file_info.birthTime() < *pStart)
			continue;
		if (pStop && file_info.birthTime() > *pStop)
			continue;
		FileList.append(files);
	}

	return FileList;
}

int  DataCenter::ReaderIDCard(IDCardInfo* pIDCard)
{
	int nResult = IDCard_Status::IDCard_Succeed;
	auto strDevPort = GetSysConfigure()->DevConfig.strIDCardReaderPort;
	bool bDevOpened = false;
	do
	{
		if (strDevPort == "AUTO" || !strDevPort.size())
		{
			gInfo() << "Try to open IDCard Reader auto!" << strDevPort;
			nResult = OpenReader(nullptr);
		}
		else
		{
			gInfo() << "Try to open IDCard Reader " << strDevPort;
			nResult = OpenReader(strDevPort.c_str());
		}

		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		bDevOpened = true;
		nResult = FindIDCard();
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}

		nResult = ReadIDCard(*pIDCard);
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		int nNationalityCode = strtol((char*)pIDCard->szNationaltyCode, nullptr, 10);
		if (nNationalityCode < 10)
			sprintf_s((char*)pIDCard->szNationaltyCode, sizeof(pIDCard->szNationaltyCode), "%02d", nNationalityCode);

	} while (0);
	//if (bDevOpened)
	{
		gInfo() << "Try to close IDCard Reader!";
		CloseReader();
	}
	return nResult;
}

int DataCenter::OpenDevice(QString& strMessage)
{	
	if (QFailed(OpenPrinter(strMessage)))
	{
		gError() << gQStr(strMessage);
		return -1;
	}
	
	if (QFailed(OpenSSCardReader(strMessage)))
	{
		gError() << gQStr(strMessage);
		return -1;
	}
	return 0;
}

int  DataCenter::UpdatePrinterStatus(QString& strMessage)
{
	char szRCode[128] = { 0 };
	if (QFailed(m_pPrinter->Printer_Status(PrinterStatus, szRCode)))
	{
		strMessage = QString("Printer_Status失败，错误代码:%1!").arg(szRCode);
		gError() << gQStr(strMessage);
		return -1;
	}
	return 0;
}
int DataCenter::OpenPrinter(QString& strMessage)
{
	int nResult = -1;
	char szRCode[32] = { 0 };
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	QString strAppPath = QCoreApplication::applicationDirPath();
	QString strPrinterMudule = strAppPath + "/" + DevConfig.strPrinterModule.c_str();
	try
	{
		do
		{
			if (DevConfig.nPrinterType < PrinterType::PRINTER_MIN ||
				DevConfig.nPrinterType > PrinterType::PRINTER_MAX)
			{
				strMessage = QString("不支持打印机型号代码:%1").arg(DevConfig.nPrinterType);
				break;
			}

			if (!m_pPrinterlib)
			{
				m_pPrinterlib = make_shared<KTModule<KT_Printer>>(strPrinterMudule.toStdString());
				if (!m_pPrinterlib)
				{
					strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strPrinterMudule);
					break;
				}
			}
			if (!m_pPrinter)
			{
				m_pPrinter = m_pPrinterlib->Instance();
				if (!m_pPrinter)
				{
					strMessage = QString("创建‘%1’实例失败!").arg(strPrinterMudule);
					break;
				}

				if (QFailed(nResult = m_pPrinter->Printer_Init(DevConfig.nPrinterType, szRCode)))
				{
					strMessage = QString("打印机初始化‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
					break;
				}

				if (QFailed(nResult = m_pPrinter->Printer_Open(szRCode)))
				{
					if (strcmp(szRCode, "0010") == 0)
					{
						strMessage = QString("打开打印机‘%1’失败,未安装色带!").arg(DevConfig.strPrinterType.c_str());
						PrinterStatus.fwToner = Ribbon_Losed;
					}
						
					else if (strcmp(szRCode, "0011") == 0)
					{
						strMessage = QString("打开打印机‘%1’失败,色带与打印不兼容!").arg(DevConfig.strPrinterType.c_str());
						PrinterStatus.fwToner = Ribbon_NOTSUPP;
					}
					else
					{
						strMessage = QString("打开打印机‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
						PrinterStatus.fwToner = Ribbon_UNKNOWN;
					}
						
					break;
				}
			}
			else
				return 0;
		} while (0);
		if (QFailed(nResult))
		{
			m_pPrinterlib = nullptr;
			m_pPrinter = nullptr;
			return -1;
		}

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

// nCheckMode   为0时，则一直开启从出卡口进卡，否再时在lTimeout时间定内，开启从出卡口进卡，超时则恢复从卡箱进卡
int  DataCenter::Print_EnableCard(long lTimeout, int nCheckable, QString& strMessage)
{

	char pszRCode[128];
	memset(pszRCode, 0x00, 128);
	const char* szCommand[] = {
						 "Psmgr;2",     // 防止进卡和出卡阻塞
						 "Pcim;M;S",    // 从卡箱进卡,并移动到接触位
						 "Pcem;M",      // 从出卡口出卡
						 //"Pneab;E"    // 打印结束后不出卡
	};
	// Pcim;B       // 设置为可同时从卡箱和出卡口手动进卡
	/* 关于Pcim;B
	1. 如果卡盒有卡时输入进卡指令，机器还是会从卡盒进卡，从缝隙处手动插卡无效
	2. 如果卡盒无卡时输入进卡指令，可以此时放一张卡在卡盒或缝隙，都可以进卡
	3. 输入Pcim;B后，即使卡盒无卡，打印机在驱动里也会显示就绪，此前在Pcim;F同状态下会显示卡片装载的黄色警告
	*/
	int nResult = -1;

	if (!m_pPrinter)
	{
		strMessage = "打印机未就绪!";
		return -1;
	}

	//if (QFailed(nResult = TestPrinter(strMessage)))
	//{
	//	return nResult;
	//}
	int Res = 0;
	for (auto var : szCommand)
	{
		gInfo() << "Try to Printer_ExtraCommand(" << var << ").";
		if (QFailed(m_pPrinter->Printer_ExtraCommand(var, pszRCode)))
		{
			gInfo() << "Printer_ExtraCommand (" << var << ") Failed:" << pszRCode;
			strMessage = QString("执行命令:%1失败").arg(var);
			return -1;
		}
	}

	if (nCheckable)
	{
		auto tStart = chrono::system_clock::now();
		bool bCardReady = false;
		int nResult = 1;
		PRINTERSTATUS lpStatus;
		while (!bCardReady)
		{
			// 检查卡片是否已经就绪
			this_thread::sleep_for(chrono::milliseconds(500));
			nResult = m_pPrinter->Printer_Status(lpStatus, pszRCode);
			if (nResult == 0)
			{
				if ((lpStatus.fwMedia == 2 || lpStatus.fwDevice == 3))
				{
					gInfo() << "The card is ready,leave.";
					break;
				}
			}
			auto duration = duration_cast<milliseconds>(system_clock::now() - tStart);
			if (duration.count() >= lTimeout)
			{
				gInfo() << "Wait timeout,leave.";
				break;
			}
		}

		Print_DisableCard(lTimeout, pszRCode);
	}
	return  0;
}

int  DataCenter::Print_DisableCard(long lTimeout, char* pszRcCode)
{
	Q_UNUSED(lTimeout);

	char szReply[128] = { 0 };
	const char* szCmd[] = {
						 "Psmgr;2",     // 防止进卡和出卡阻塞
						 "Pcim;F",      // 从卡箱进卡
						 "Pcem;M",      // 从出卡口出卡
						 //"Pneab;E"      // 打印结束后不出卡
	};

	for (auto var : szCmd)
	{
		gInfo() << "Try to evolis_command(" << var << ").";
		if (QFailed(m_pPrinter->Printer_ExtraCommand(var, pszRcCode)))
		{
			gInfo() << "Printer_ExtraCommand (" << var << ") Failed!";
			break;
		}
	}

	strcpy(pszRcCode, "0000");
	return  0;
}


int DataCenter::OpenPrinter(QString strPrinterLib, PrinterType nPrinterType, int& nDepenseBox, QString& strDPI, QString& strMessage)
{
	int nResult = -1;
	char szRCode[32] = { 0 };

	try
	{
		m_pPrinterlib = nullptr;
		do
		{
			if (nPrinterType < PrinterType::PRINTER_MIN ||
				nPrinterType > PrinterType::PRINTER_MAX)
			{
				strMessage = QString("不支持打印机型号代码:%1").arg(nPrinterType);
				break;
			}
			if (!m_pPrinterlib)
			{
				QString strAppPath = QCoreApplication::applicationDirPath();
				QString strFullPrinterModule = strAppPath + "/" + strPrinterLib;
				gInfo() << "Try to load " << strFullPrinterModule.toStdString();
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
					strMessage = QString("Printer_Init‘%1’失败,错误代码:%2").arg(szPrinterTypeList[nPrinterType]).arg(szRCode);
					break;
				}

				if (QFailed(nResult = m_pPrinter->Printer_Open(szRCode)))
				{
					strMessage = QString("Printer_Open‘%1’失败,错误代码:%2").arg(szPrinterTypeList[nPrinterType]).arg(szRCode);
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

void DataCenter::CloseDevice()
{
	char szRCode[32] = { 0 };
	if (m_pPrinter)
	{
		m_pPrinter->Printer_Close(szRCode);
		m_pPrinter = nullptr;
	}

	m_pPrinterlib = nullptr;

	if (m_pSSCardReader)
	{
		m_pSSCardReader->Reader_Exit(szRCode);
		m_pSSCardReader = nullptr;
	}
	m_pSSCardReaderLib = nullptr;
}

int DataCenter::OpenSSCardReader(QString& strMessage)
{
	int nResult = -1;
	try
	{
		do
		{
			char szRCode[32] = { 0 };
			DeviceConfig& DevConfig = pSysConfig->DevConfig;
			RegionInfo& Region = pSysConfig->Region;
			QString strAppPath = QCoreApplication::applicationDirPath();
			QString strReaderMudule = strAppPath + "/" + DevConfig.strReaderModule.c_str();
			if (!m_pSSCardReaderLib)
			{
				m_pSSCardReaderLib = make_shared<KTModule<KT_Reader>>(strReaderMudule.toStdString());
				if (!m_pSSCardReaderLib)
				{
					strMessage = strMessage = QString("内存不足，加载'%1'失败!").arg(strReaderMudule);
					break;
				}
			}
			if (!m_pSSCardReader)
			{
				m_pSSCardReader = m_pSSCardReaderLib->Instance();
				if (!m_pSSCardReader)
				{
					strMessage = QString("创建'%1'实例失败!").arg(strReaderMudule);
					break;
				}
				if (QFailed(nResult = m_pSSCardReader->Reader_Create(DevConfig.nSSCardReaderType, szRCode)))
				{
					strMessage = QString("Reader_Create'%1'失败,错误代码:%2").arg(DevConfig.strSSCardReadType.c_str()).arg(szRCode);
					break;
				}
				string &strSSCardReaderPort = DevConfig.strSSCardReaderPort;
				int nIndex = 0;
				int nPort = 0;
				if (strSSCardReaderPort.find("USB") != string::npos)
				{
					nPort = 100;
				}
				else if (nIndex = strSSCardReaderPort.find("COM") != string::npos)
				{
					string strPortnum = strSSCardReaderPort.substr(nIndex + 3);
					nPort = strtol(strPortnum.c_str(), nullptr, 10);
				}
				if (QFailed(nResult = m_pSSCardReader->Reader_Init(szRCode)))
				{
					strMessage = QString("Reader_Init失败,错误代码:%2").arg(szRCode);
					break;
				}
			}

			char szOutInfo[1024] = { 0 };
			DriverInit(m_pSSCardReader, (char*)Region.strCountry.c_str(), (char*)Region.strSSCardDefaulutPin.c_str(), (char*)Region.strPrimaryKey.c_str(), szOutInfo);

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

int DataCenter::OpenSSCardReader(QString strLib,QString strPort, ReaderBrand nReaderType, QString& strMessage)
{
	int nResult = -1;
	try
	{
		m_pSSCardReaderLib = nullptr;
		do
		{
			if (!m_pSSCardReaderLib)
			{
				char szRCode[32] = { 0 };
				QString strAppPath = QCoreApplication::applicationDirPath();
				QString strReaderMudule = strAppPath + "/" + strLib.toStdString().c_str();

				m_pSSCardReaderLib = make_shared<KTModule<KT_Reader>>(strReaderMudule.toStdString());
				if (!m_pSSCardReaderLib)
				{
					strMessage = strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strReaderMudule);
					break;
				}
				m_pSSCardReader = m_pSSCardReaderLib->Instance();
				if (!m_pSSCardReader)
				{
					strMessage = QString("创建‘%1’实例失败!").arg(strReaderMudule);
					break;
				}
				if (QFailed(nResult = m_pSSCardReader->Reader_Create(nReaderType, szRCode)))
				{
					strMessage = QString("Reader_Create(‘%1’)失败,错误代码:%2").arg(nReaderType).arg(szRCode);
					break;
				}
				string strSSCardReaderPort = strPort.toStdString();
				int nIndex = 0;
				int nPort = 0;
				if (strSSCardReaderPort.find("USB") != string::npos)
					nPort = 100;
				
				else if ((nIndex = strSSCardReaderPort.find("COM")) != string::npos)
				{
					string strPortnum = strSSCardReaderPort.substr(nIndex + 3);
					nPort = strtol(strPortnum.c_str(), nullptr, 10) - 1;
				}

				if (QFailed(nResult = m_pSSCardReader->Reader_Init(szRCode)))
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

int DataCenter::TestCard(QString& strMessage)
{
	PRINTERSTATUS PrinterStatus;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	int& nDepenseBox = DevConfig.nDepenseBox;

	int nResult = -1;
	BOXINFO BoxInfo;
	ZeroMemory(&BoxInfo, sizeof(BOXINFO));
	char szRCode[32] = { 0 };
	do
	{
		if (QFailed(m_pPrinter->Printer_BoxStatus(BoxInfo, szRCode)))
		{
			strMessage = QString("Printer_BoxStatus失败:%1").arg(szRCode);
			break;
		}

		if (BoxInfo.BoxList[nDepenseBox - 1].BoxStatus == 2)
		{
			strMessage = QString("进卡箱无卡,请放入卡片后重试!").arg(nDepenseBox).arg(BoxInfo.BoxList[nDepenseBox].BoxStatus);
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
		// 0-无卡；2-卡在内部；3-卡在上电位，4-卡在闸门外 5-堵卡；6-卡片未知
		bool bSucceed = false;
		switch (PrinterStatus.fwMedia)
		{
		case 0:		// 机内无卡
		{
			break;
		}
		case 1:			// 1-卡在门口:
		{
			strMessage = QString("打印机出卡号尚有未取走卡片,取走点击确定以继续!");
			nResult = 1;
			break;
		}
		case 2:			// 2-卡在内部；
		case 3:			// 3-卡在接触位
		case 4:			// 4-卡在非接位
		{
			if (!m_pSSCardReader)
				break;
			if (!g_pDataCenter->bTestCard)
			{
				bSucceed = true;
				break;
			}
			char szCardATR[128] = { 0 };
			char szRCode[128] = { 0 };
			int nCardATRLen = 0;
			if (QFailed(m_pSSCardReader->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
			{
				strMessage = "卡片上电失败,请更换卡片或重新进卡!";
				break;
			}
			const char* szCommand = "0084000008";
			char szOut[1024] = { 0 };
			int nOutLen = 0;
			if (QFailed(m_pSSCardReader->Reader_APDU((BYTE*)szCommand, strlen(szCommand), szOut, nOutLen, szRCode)))
			{
				strMessage = "卡片数据交互失败,请更换卡片或重新进卡!";
				break;
			}
			bSucceed = true;
			break;
		}
		case 5:
		{
			strMessage = QString("打印机堵卡,请联系相关技术人员处理!");
			break;
		}
		case 6:
		default:
		{
			strMessage = QString("发生未知错误,请联系相关技术人员处理!");
			break;
		}
		}
		if (!bSucceed)
			break;

		nResult = 0;
	} while (0);
	if (QFailed(nResult))
	{
		m_pPrinter->Printer_Retract(1, szRCode);
	}
	return nResult;
}

optional< string> DataCenter::GetProgressFile(IDCardInfoPtr pIDCard)
{
	QString strAppPath = QApplication::applicationDirPath();
	QString strJsonPath;
	if (pIDCard)
		strJsonPath = QString("%1/data/Progress_%2.json").arg(strAppPath).arg((char*)pIDCard->szIdentity);
	else if (this->pIDCard)
		strJsonPath = QString("%1/data/Progress_%2.json").arg(strAppPath).arg((char*)this->pIDCard->szIdentity);
	else
		return std::nullopt;
	return { string(strJsonPath.toLocal8Bit().data()) };
}

optional<CJsonObjectPtr > DataCenter::OpenProgress(string& strProgressFile, bool bCreate)
{
	return OpenProgress((string&&)strProgressFile, bCreate);
}

void TraverseJson( neb::CJsonObject& jsonObject, const std::string& prefix = "") 
{
	std::string key;
	std::string value;
	neb::CJsonObject subObject;

	for (int i = 0; i < jsonObject.ArraySize(); ++i) 
	{
		jsonObject.Get(i, subObject);
		TraverseJson(subObject, prefix + "[" + std::to_string(i) + "]");
	}

	while (jsonObject.GetKey(key)) {
		if (jsonObject.Get(key, value)) 
		{
			qDebug() << prefix.c_str() << key.c_str() << ": " << value.c_str();
		}
		else if (jsonObject.Get(key, subObject)) {
			TraverseJson(subObject, prefix + key + ".");
		}
	}
}

optional<CJsonObjectPtr > DataCenter::OpenProgress(string &&strProgressFile,bool bCreate)
{
	try
	{
		CJsonObjectPtr pJson = make_shared<CJsonObject>();
		
		if (!pJson)
		{
			gError() << "Failed in allocating memory for pJson.";
			return std::nullopt;
		}

		if (!bCreate && fs::exists(strProgressFile))
		{
			
			ifstream ifs(strProgressFile, ios::in | ios::binary);
			stringstream ss;
			ss << ifs.rdbuf();
			//gInfo() << ss.str();
			if (!pJson->Parse(ss.str()))
			{
				gInfo() << strProgressFile << " is invalid.";
				gInfo() << strProgressFile << ":" << ss.str();
				ifs.close();
				fs::remove(strProgressFile);
				pJson->Clear();
			}
			TraverseJson(*pJson);
			return pJson;
		}
		else
		{
			pJson->Parse("{}");
			SaveProgress(pJson, strProgressFile);
			return { pJson };
		}
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return std::nullopt;
	}
}

bool DataCenter::SetProgress(SSCardInfoPtr& pSSCard,CJsonObjectPtr pJson,bool bSave)
{
	if (!pSSCard)
	{
		gError() << "pSSCard can't be nullptr";
		return false;
	}

	if (!pJson)
	{
		if (!pJsonProgress)
		{
			gError() << "pJson and pJsonProgress can't be nullptr and the same time";
			return false;
		}
		else
			pJson = pJsonProgress;
	}
	if (bSave)
	{
		// 创建时已经由身份证信息写入
		//JsGetValueP(pJson, "Name", pSSCardInfo->strName);
		//JsGetValueP(pJson, "CardID", pSSCardInfo->strCard);
		//JsGetValueP(pJson, "Birthday", pSSCardInfo->strBirthday);

		SetJsonPtrValue(pJson, "CardNum", pSSCard->strCardNum);
		SetJsonPtrValue(pJson, "Address", pSSCard->strAddress);
		SetJsonPtrValue(pJson, "NationCode", pSSCardInfo->strNation);
		SetJsonPtrValue(pJson, "Sex", pSSCardInfo->strSex);
		SetJsonPtrValue(pJson, "Birthday", pSSCardInfo->strBirthday);
		SetJsonPtrValue(pJson, "Mobile", pSSCard->strMobile);
		SetJsonPtrValue(pJson, "BankCode", pSSCard->strBankCode);
		SetJsonPtrValue(pJson, "City", pSSCard->strCity);
		SetJsonPtrValue(pJson, "PersonType", pSSCard->strPersonType);
		SetJsonPtrValue(pJson, "CompanyName", pSSCard->strCompanyName);
		SetJsonPtrValue(pJson, "ReleaseDate", pSSCard->strReleaseDate);
		SetJsonPtrValue(pJson, "ValidDate", pSSCard->strValidDate);
		SetJsonPtrValue(pJson, "PCH", pSSCard->strPCH);
		SetJsonPtrValue(pJson, "Photo", pSSCard->strPhoto);

		SetJsonPtrValue(pJson, "Community", pSSCard->strCommunity);		
		SetJsonPtrValue(pJson, "Department", pSSCard->strDepartmentName);
		SetJsonPtrValue(pJson, "LocalNum", pSSCard->strLocalNum);
		SetJsonPtrValue(pJson, "Class", pSSCard->strClassName);
		SetJsonPtrValue(pJson, "OrganID", pSSCard->strOrganID);		
		SetJsonPtrValue(pJson, "PostalCode", pSSCard->strPostalCode);
		SetJsonPtrValue(pJson, "Email", pSSCard->strEmail);
		SetJsonPtrValue(pJson, "GuardianName", pSSCard->strGuardianName);
		SetJsonPtrValue(pJson, "TransType", pSSCard->strTransType);
		SetJsonPtrValue(pJson, "SSQX", pSSCard->strSSQX);
		SetJsonPtrValue(pJson, "CityAccTime", pSSCard->strCityAccTime);
		SetJsonPtrValue(pJson, "CityInfo", pSSCard->strCityInfo);
		SetJsonPtrValue(pJson, "CardStatus", pSSCard->strCardStatus);
		SetJsonPtrValue(pJson, "IdentifyNum", pSSCard->strIdentifyNum);
		SetJsonPtrValue(pJson, "CardATR", pSSCard->strCardATR);
		SetJsonPtrValue(pJson, "BankNum", pSSCard->strBankNum);
		
		return SaveProgress(pJsonProgress, strPersonProgressFile);
	}
	else
		return true;
}

bool DataCenter::SetProgress(tuple<ProgrerssType, string, CJsonObjectPtr>& tpl,bool bSave)
{	
	std::tie(nProgressType, strPersonProgressFile, pJsonProgress) = tpl;
	if (strPersonProgressFile.empty() || !pJsonProgress)
	{
		gError() << "strPersonProgressFile is empty or pJsonProgress is nullptr";
		return false;
	}
	return SetProgress(pIDCard, pJsonProgress, bSave);
	/*{
		gError() << __FUNCTION__ << " Failed in update IDCard Information.";
		return false;
	}*/
	//if (!SetProgress(pSSCardInfo, pJsonProgress, false))
	//{
	//	gError() << __FUNCTION__ << " Failed in update SSCard Information.";
	//	return false;
	//}
}

bool DataCenter::SetProgress(IDCardInfoPtr& pIDCard,CJsonObjectPtr pJson,bool bSave)
{
	if (!pIDCard)
	{
		gError() << "pIDCard can't be nullptr";
		return false;
	}
		
	if (!pJson)
	{
		if (!pJsonProgress)
		{
			gError() << "pJson and pJsonProgress can't be nullptr and the same time";
			return false;
		}
		else
			pJson = pJsonProgress;
	}
	if (bSave)
	{
		SetJsonPtrValue(pJson, "Name", pIDCard->szName);
		SetJsonPtrValue(pJson, "Identity", pIDCard->szIdentity);
		SetJsonPtrValue(pJson, "Gender", pIDCard->szGender);
		SetJsonPtrValue(pJson, "NationCode", pIDCard->szNationaltyCode);
		SetJsonPtrValue(pJson, "Address", pIDCard->szAddress);
		SetJsonPtrValue(pJson, "Nation", pIDCard->szNationalty);
		SetJsonPtrValue(pJson, "PaperIssuedate", pIDCard->szExpirationDate1);
		SetJsonPtrValue(pJson, "PaperExpiredate", pIDCard->szExpirationDate2);
		char szPayCode[64] = { 0 };
		if (g_pDataCenter->GetProgressField("payCode", szPayCode))
			g_pDataCenter->strPayCode = szPayCode;
		char szTranTime[64] = { 0 };
		if (g_pDataCenter->GetProgressField("transTime", szTranTime))
			g_pDataCenter->strTransTime = szTranTime;
		SetJsonPtrValue(pJson, "payCode", (char*)g_pDataCenter->strPayCode.c_str());
		SetJsonPtrValue(pJson, "transTime", (char*)g_pDataCenter->strTransTime.c_str());
		return SaveProgress(pJsonProgress, strPersonProgressFile);
	}
	else
		return true;
}

bool DataCenter::GetProgress(IDCardInfoPtr& pIDCard,CJsonObjectPtr pJson)
{
	if (!pIDCard)
	{
		gError() << "pIDCard can't be nullptr";
		return false;
	}

	if (!pJson)
	{
		if (!pJsonProgress)
		{
			gError() << "pJson and pJsonProgress can't be nullptr and the same time";
			return false;
		}
		else
			pJson = pJsonProgress;
	}

	GetJsonPtrValue(pJson, "Name", pIDCard->szName);
	GetJsonPtrValue(pJson, "Identity", pIDCard->szIdentity);
	GetJsonPtrValue(pJson, "Gender", pIDCard->szGender);
	GetJsonPtrValue(pJson, "Nation", pIDCard->szName);
	GetJsonPtrValue(pJson, "Address", pIDCard->szAddress);
	GetJsonPtrValue(pJson, "Nation", pIDCard->szNationalty);
	GetJsonPtrValue(pJson, "PaperIssuedate", pIDCard->szExpirationDate1);
	GetJsonPtrValue(pJson, "PaperExpiredate", pIDCard->szExpirationDate2);
	return true;
}

bool DataCenter::GetProgress(SSCardInfoPtr& pSSCard,CJsonObjectPtr pJson)
{
	if (!pSSCard)
	{
		gError() << "pSSCard can't be nullptr";
		return false;
	}
	if (!pJson)
	{
		if (!pJsonProgress)
		{
			gError() << "pJson and pJsonProgress can't be nullptr and the same time";
			return false;
		}
		else
			pJson = pJsonProgress;
	}

	GetJsonPtrValue(pJson, "Name", pSSCard->strName);
	GetJsonPtrValue(pJson, "Identity", pSSCard->strCardID);
	GetJsonPtrValue(pJson, "Birthday", pSSCard->strBirthday);
	GetJsonPtrValue(pJson, "CardNum", pSSCard->strCardNum);
	GetJsonPtrValue(pJson, "Address", pSSCard->strAddress);
	GetJsonPtrValue(pJson, "NationCode", pSSCard->strNation);
	GetJsonPtrValue(pJson, "Sex", pSSCard->strSex);
	GetJsonPtrValue(pJson, "Mobile", pSSCard->strMobile);
	GetJsonPtrValue(pJson, "BankCode", pSSCard->strBankCode);
	GetJsonPtrValue(pJson, "City", pSSCard->strCity);
	GetJsonPtrValue(pJson, "PersonType", pSSCard->strPersonType);
	GetJsonPtrValue(pJson, "CompanyName", pSSCard->strCompanyName);
	GetJsonPtrValue(pJson, "ReleaseDate", pSSCard->strReleaseDate);
	GetJsonPtrValue(pJson, "ValidDate", pSSCard->strValidDate);
	GetJsonPtrValue(pJson, "PCH", pSSCard->strPCH);
	GetJsonPtrStrValue(pJson, "payCode", g_pDataCenter->strPayCode);
	strcpy(pSSCard->strPayCode , g_pDataCenter->strPayCode.c_str());
	GetJsonPtrStrValue(pJson, "transTime", g_pDataCenter->strTransTime);
	strcpy(pSSCard->strTransactionTime, g_pDataCenter->strTransTime.c_str());
#ifdef HN2022
	GetJsonPtrValue(pJson, "PaperIssuedate", pSSCard->strIDCardIssuedDate);
#endif
	GetJsonPtrValuePtr(pJson, "Photo", pSSCard->strPhoto);
	GetJsonPtrValue(pJson, "CardVender", pSSCard->strCardVender);
	
	GetJsonPtrValue(pJson, "Community", pSSCard->strCommunity);
	GetJsonPtrValue(pJson, "LocalNum", pSSCard->strLocalNum);
	GetJsonPtrValue(pJson, "Department", pSSCard->strDepartmentName);
	GetJsonPtrValue(pJson, "Class", pSSCard->strClassName);
	GetJsonPtrValue(pJson, "OrganID", pSSCard->strOrganID);
	GetJsonPtrValue(pJson, "PostalCode", pSSCard->strPostalCode);
	GetJsonPtrValue(pJson, "Email", pSSCard->strEmail);
	GetJsonPtrValue(pJson, "GuardianName", pSSCard->strGuardianName);
	GetJsonPtrValue(pJson, "TransType", pSSCard->strTransType);
	GetJsonPtrValue(pJson, "SSQX", pSSCard->strSSQX);
	GetJsonPtrValue(pJson, "CityAccTime", pSSCard->strCityAccTime);
	GetJsonPtrValue(pJson, "CityInfo", pSSCard->strCityInfo);
	GetJsonPtrValue(pJson, "CardStatus", pSSCard->strCardStatus);	
	GetJsonPtrValue(pJson, "IdentifyNum", pSSCard->strIdentifyNum);
	GetJsonPtrValue(pJson, "CardATR", pSSCard->strCardATR);
	GetJsonPtrValue(pJson, "BankNum", pSSCard->strBankNum);
	return true;
}

void DataCenter::CloseProgress()
{
	try
	{
		auto optProgress = GetProgressFile(pIDCard);
		if (!optProgress)
		{
			gError() << "Failed in GetProgressFile";
			return;
		}
		
		if (pJsonProgress)
			pJsonProgress = nullptr;
		
		if (!fs::exists("./Finished"))
		{
			fs::create_directory("./Finished");
		}
		QString strAppPath = QApplication::applicationDirPath();
		QString strFinishPath = QString("%1/Finished/Progress_%2.json").arg(strAppPath).arg((char*)pIDCard->szIdentity);
		string strBaseFile = strFinishPath.toLocal8Bit().data();
		int nIndex = 0;
		nProgressType = ProgrerssType::Progress_UnStart;
		do
		{
			if (fs::exists(strBaseFile))
				strBaseFile = strFinishPath.toStdString() + std::to_string(nIndex++);
			else
				break;
		} while (true);
		fs::copy(strPersonProgressFile, strBaseFile);
		fs::remove(strPersonProgressFile);
		strPersonProgressFile = "";
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return;
	}
}

bool DataCenter::SetProgressField(const char* szKey, char* szVal)
{
	if (!pJsonProgress)
	{
		gError() << "pJsonProgress can't be nullptr";
		return false;
	}
	if (!szKey || !szVal)
	{
		gError() << "szKey or szVal can't be nullptr";
		return false;
	}
	try
	{		
		SetJsonPtrValue(pJsonProgress, szKey, szVal);
		ofstream ifs(strPersonProgressFile, ios::out | ios::binary);
		ifs << pJsonProgress->ToFormattedString();
		return true;
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return false;
	}
}

bool DataCenter::GetProgressField(const char* szKey,char *szVal)
{
	if (!pJsonProgress)
	{
		gError() << "pJsonProgress can't be nullptr";
		return false;
	}
	string strValue;
	if (pJsonProgress->Get(szKey, strValue))
	{
		strcpy(szVal, strValue.c_str());
		return true;
	}
	else
	{
		gError() << "Failed in get field " << szKey << " from pJsonProgress";
		return false;
	}
}

bool DataCenter::GetProgressField(const char* szKey, string& strVal)
{
	if (!pJsonProgress)
	{
		gError() << "pJsonProgress can't be nullptr";
		return false;
	}

	if (pJsonProgress->Get(szKey, strVal))
		return true;
	else
	{
		gError() << "Failed in get field " << szKey << " from pJsonProgress";
		return false;
	}
}

optional<CJsonObjectPtr> DataCenter::LoadSSCardData(string strProgressFile,SSCardInfoPtr& pSSCard)
{
	auto optProgressFile = OpenProgress(strProgressFile,false);
	if (!optProgressFile)
	{
		gError() << "Failed in OpenProgress.";
		return std::nullopt;
	}
 	auto pJson = optProgressFile.value();
	if (GetProgress(pSSCard, pJson))
		return { pJson };
	else
		return nullopt;	
}

bool DataCenter::SaveProgress(CJsonObjectPtr& pJson, string& strProgressFile)
{
	return SaveProgress(pJson, (string&&)strProgressFile);
}

bool DataCenter::SaveProgress(CJsonObjectPtr& pJson, string&& strProgressFile)
{
	try
	{
		gInfo() << "Try to save json file" << strProgressFile;
		ofstream ifs(strProgressFile, ios::out | ios::binary);
		ifs << pJson->ToFormattedString();
		return true;
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return false;
	}
}

/*
bool DataCenter::SaveProgress()
{
	if (!pSSCardInfo || !pJsonProgress)
		return false;

	try
	{
		// 只保存社保卡信息，不保存身份证中的信息		
		JsSetValueP(pJsonProgress, "Name", pSSCardInfo->strName);
		JsSetValueP(pJsonProgress, "CardID", pSSCardInfo->strCard);
		JsSetValueP(pJsonProgress, "Gender", pSSCardInfo->strSex);
		JsSetValueP(pJsonProgress, "Birthday", pSSCardInfo->strBirthday);

		JsSetValueP(pJsonProgress, "Mobile", pSSCardInfo->strMobile);
		JsSetValueP(pJsonProgress, "BankCode", pSSCardInfo->strBankCode);
		JsSetValueP(pJsonProgress, "City", pSSCardInfo->strCity);
		JsSetValueP(pJsonProgress, "PersonType", pSSCardInfo->strPersonType);
		JsSetValueP(pJsonProgress, "Company", pSSCardInfo->strCompanyName);
		JsSetValueP(pJsonProgress, "Community", pSSCardInfo->strCommunity);
		JsSetValueP(pJsonProgress, "LocalNum", pSSCardInfo->strLocalNum);
		JsSetValueP(pJsonProgress, "Department", pSSCardInfo->strDepartmentName);
		JsSetValueP(pJsonProgress, "Class", pSSCardInfo->strClassName);

		JsSetValueP(pJsonProgress, "OrganID", pSSCardInfo->strOrganID);
		JsSetValueP(pJsonProgress, "CardNum", pSSCardInfo->strCardNum);
		JsSetValueP(pJsonProgress, "Address", pSSCardInfo->strAddress);
		JsSetValueP(pJsonProgress, "PostalCode", pSSCardInfo->strPostalCode);
		JsSetValueP(pJsonProgress, "Email", pSSCardInfo->strEmail);
		JsSetValueP(pJsonProgress, "GuardianName", pSSCardInfo->strGuardianName);
		JsSetValueP(pJsonProgress, "TransType", pSSCardInfo->strTransType);
		JsSetValueP(pJsonProgress, "City", pSSCardInfo->strCity);
		JsSetValueP(pJsonProgress, "SSQX", pSSCardInfo->strSSQX);
		JsSetValueP(pJsonProgress, "CityAccTime", pSSCardInfo->strCityAccTime);
		JsSetValueP(pJsonProgress, "CityInfo", pSSCardInfo->strCityInfo);
		JsSetValueP(pJsonProgress, "CardStatus", pSSCardInfo->strCardStatus);
		JsSetValueP(pJsonProgress, "Card", pSSCardInfo->strCard);
		JsSetValueP(pJsonProgress, "ReleaseDate", pSSCardInfo->strReleaseDate);
		JsSetValueP(pJsonProgress, "ValidDate", pSSCardInfo->strValidDate);
		JsSetValueP(pJsonProgress, "IdentifyNum", pSSCardInfo->strIdentifyNum);
		JsSetValueP(pJsonProgress, "CardATR", pSSCardInfo->strCardATR);
		JsSetValueP(pJsonProgress, "BankNum", pSSCardInfo->strBankNum);
		JsSetValueP(pJsonProgress, "PCH", pSSCardInfo->strPCH);
		
		return SaveProgress(pJsonProgress, strPersonProgressFile);
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return false;
	}
}
*/
int DataCenter::GetProgressStatus(string strProcessName, CJsonObjectPtr pInJsonProgress)
{
	int nStatus = 0;
	CJsonObjectPtr pProgress = nullptr;
	try
	{
		if (pInJsonProgress)
			pProgress = pInJsonProgress;
		else
			pProgress = pJsonProgress;
		if (!pProgress)
		{
			gError() <<"pProgress can't be null.";
			return -1;
		}
		if (!pProgress->KeyExist("Progress"))
		{
			gError() << "Can't locate key 'Progress'.";
			return 0;
		}
		CJsonObject pSubJson =	(*pProgress)["Progress"];
		if (pSubJson.IsEmpty())
		{
			gError() << "Can't get a valid json object with key 'Progress' from pJsonProgress.";
			return 0;
		}

		if (pSubJson.KeyExist(strProcessName) &&
			pSubJson.Get(strProcessName, nStatus))
		{
			gInfo() << nStatus << "<===" << strProcessName;
			return nStatus;
		}
		else
		{
			gError() << "Can't locate Key '" << strProcessName << "'";
			return 0;
		}
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return 0;
	}
}

bool DataCenter::SetProgressStatus(string strProcessName, int nStatus)
{
	try
	{
		if (!pJsonProgress)
		{
			gError() << "pJsonProgress can't be null.";
			return false;
		}
		CJsonObject pSubJson;
		if (pJsonProgress->KeyExist("Progress"))
		{
			gInfo() << "Try to update node Progress."<<strProcessName;
			pSubJson = (*pJsonProgress)["Progress"];
			if (pSubJson.KeyExist(strProcessName))
				pSubJson.Replace(strProcessName, nStatus);
			else
				pSubJson.Add(strProcessName, nStatus);
			gInfo() << nStatus << "===>" << strProcessName;
		}
		else
		{
			pSubJson.Add(strProcessName, nStatus);
			gInfo() << nStatus << "===>" << strProcessName;
			pJsonProgress->Add("Progress", pSubJson);
			gInfo() << "Create node 'Progress'.";
		}
		pJsonProgress->Replace("Progress", pSubJson);
		gInfo() << "Progress:" << pSubJson.ToFormattedString();
		ofstream ifs(strPersonProgressFile, ios::out | ios::binary);
		gInfo() << pJsonProgress->ToFormattedString();
		ifs << pJsonProgress->ToFormattedString();
		ifs.flush();
		return true;
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return false;
	}
}

int DataCenter::TestPrinter(QString& strMessage)
{
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	int& nDepenseBox = DevConfig.nDepenseBox;

	int nResult = -1;
	BOXINFO BoxInfo;
	ZeroMemory(&BoxInfo, sizeof(BOXINFO));
	char szRCode[32] = { 0 };
	do
	{
		if (QFailed(m_pPrinter->Printer_BoxStatus(BoxInfo, szRCode)))
		{
			strMessage = QString("Printer_BoxStatus失败:%1").arg(szRCode);
			break;
		}

		if (BoxInfo.BoxList[nDepenseBox - 1].BoxStatus == 2)
		{
			strMessage = QString("进卡箱无卡,请放入卡片后重试!").arg(nDepenseBox).arg(BoxInfo.BoxList[nDepenseBox].BoxStatus);
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
		// 0-无卡；2-卡在内部；3-卡在上电位，4-卡在闸门外 5-堵卡；6-卡片未知
		bool bSucceed = false;
		switch (PrinterStatus.fwMedia)
		{
		case 0:		// 机内无卡
		{
			bSucceed = true;
			break;
		}
		case 1:			// 1-卡在门口:
		{
			if (g_pDataCenter->bDebug)
			{
				gInfo() << "Warning!!!In Remote Debug,Skip the card on Eject postion!";
				nResult = 0;
				bSucceed = true;
				break;
			}
			strMessage = QString("打印机出卡口尚有未取走卡片,请先取走卡片后重新操作!");
			nResult = 1;
			break;
		}
		case 2:			// 2-卡在内部；
		case 3:			// 3-卡在上电位
		case 4:			// 4-卡在闸门外
		{
			if (g_pDataCenter->bDebug)
			{
				bSucceed = true;
				break;;
			}
				
			if (QFailed(m_pPrinter->Printer_Retract(1, szRCode)))
			{
				strMessage = QString("打印机尚有未取走卡片,尝试将其移动到回收箱失败，错误代码:%1!").arg(szRCode);
				break;
			}
			else
				bSucceed = true;
			break;
		}

		case 5:
		{
			strMessage = QString("打印机堵卡,请联系相关技术人员处理!");
			break;
		}
		case 6:
		default:
		{
			strMessage = QString("发生未知错误,请联系相关技术人员处理!");
			break;
		}
		}
		if (!bSucceed)
			break;
		if (g_pDataCenter->bDebug )
		{
			if (PrinterStatus.fwMedia == 0)
				if (QFailed(g_pDataCenter->Depense(strMessage)))
				{
					bSucceed = false;
					break;
				}
		}
		else if (QFailed(g_pDataCenter->Depense(strMessage)))
		{
			bSucceed = false;
			break;
		}
		// 0-FLLL;1-LOW;2-OUT;3-NOTSUPP;4-UNKNOW
		bSucceed = false;
		
		switch (PrinterStatus.fwToner)
		{
		default:
		case 0:
		{
			bSucceed = true;
			
			break;
		}
		case 1:
		{
			//bSucceed = true;
			strMessage = QString("打印机色带余量低,请检查或更换色带!");
			break;
		}
		case 2:
		{
			strMessage = QString("打印机色带耗尽或未安装色带,状态代码:%1!").arg(PrinterStatus.fwToner);
			break;
		}
		case 3:
		{
			strMessage = QString("打印机中安装了不兼容的色带,请更换色带!").arg(PrinterStatus.fwToner);
			break;
		}
		case 4:
		{
			strMessage = QString("发生未知错误!").arg(PrinterStatus.fwToner);
			break;
		}
		}

		if (!bSucceed)
			break;
		nResult = 0;
	} while (0);
	return nResult;
}

int DataCenter::Depense(QString& strMessage)
{
	int nResult = -1;

	//BOXINFO BoxInfo = {0};
	//PRINTERSTATUS PrinterStatus;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	char szRCode[32] = { 0 };
	do
	{
		if (!m_pPrinter)
		{
			strMessage = "打印机未初始化";
			break;
		}

		/* 此功能前置,不再在制卡时才检查是否有卡，而是在进行整个流程开始前就检查，以防止制止中途无卡时发生无卡的情况
		if (QFailed(nResult = TestPrinter(strMessage)))
		{
			break;
		}*/

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

int DataCenter::Depense(int nDepenseBox, CardPowerType nPowerOnType, QString& strMessage)
{
	int nResult = -1;
	BOXINFO BoxInfo;
	ZeroMemory(&BoxInfo, sizeof(BOXINFO));
	char szRCode[32] = { 0 };
	do
	{
		if (!m_pPrinter)
		{
			strMessage = "打印机未初始化";
			break;
		}

		if (QFailed(nResult = TestPrinter(strMessage)))
		{
			break;
		}
		int nDepensePos = 4;	// 1-读磁位；2-接触IC位;3-非接IC位;4-打印位， 默认为接触位
		if (nPowerOnType == CardPowerType::READER_UNCONTACT)
			nDepensePos = 3;
		else if (nPowerOnType == CardPowerType::READER_CONTACT)
			nDepensePos = 2;

		if (QFailed(m_pPrinter->Printer_Dispense(nDepenseBox - 1, nDepensePos, szRCode)))
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


string DataCenter::MakeCardInfo(string strATR, SSCardInfoPtr& pSSCardInfo)
{
	string strCardInfo = "";
	//RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
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

int DataCenter::PrintCard(SSCardInfoPtr& pSSCardInfo, QString strPhoto, QString& strMessage, bool bPrintText)
{
	char szBuffer[1024] = { 0 };
	int nBufferSize = sizeof(szBuffer);
	int nResult = -1;
	char szRCode[32] = { 0 };
	CardFormPtr& pCardForm = GetCardForm();
	do
	{
		nBufferSize = sizeof(szBuffer);
		ZeroMemory(szBuffer, nBufferSize);

		if (QFailed(m_pPrinter->Printer_InitPrint(nullptr, szRCode)))
		{
			strMessage = QString("Printer_InitPrint失败，错误代码:%1!").arg(szRCode);
			break;
		}

		TextPosition* pFieldPos = &pCardForm->posName;
		if (bPrintText)
		{
			if (QFailed(m_pPrinter->Printer_AddText((char*)pSSCardInfo->strName, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode)))
				break;
			pFieldPos = &pCardForm->posIDNumber;
			if (QFailed(m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardID, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode)))
				break;
			pFieldPos = &pCardForm->posSSCardID;
			if (QFailed(m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardNum, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode)))
				break;
			pFieldPos = &pCardForm->posIssueDate;
			int nYear, nMonth, nDay;

			sscanf_s(pSSCardInfo->strReleaseDate, "%04d%02d%02d", &nYear, &nMonth, &nDay);
			char szReleaseDate[32] = { 0 };
			sprintf_s(szReleaseDate, 32, "%d年%d月", nYear, nMonth);
			if (QFailed(m_pPrinter->Printer_AddText((char*)UTF8_GBK(szReleaseDate).c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode)))
				break;
		}

		ImagePosition& ImgPos = pCardForm->posImage;
		if (!strPhoto.size())
		{
			gInfo() << "There is no sepiciialed immage ,now try to print:" << strSSCardPhotoFile;
			QFileInfo fi(strSSCardPhotoFile.c_str());
			if (fi.isFile())
			{
				gInfo() << "Try to print image:" << strSSCardPhotoFile;
				if (QFailed(m_pPrinter->Printer_AddImage((char*)strSSCardPhotoFile.c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode)))
					break;
			}
			else
			{
				gInfo() << "Image " << strSSCardPhotoFile << " is not exist!Try to print " << g_pDataCenter->strIDImageFile;
				QFileInfo fi(g_pDataCenter->strIDImageFile.c_str());
				if (fi.isFile())
				{
					gInfo() << "Try to print image:" << g_pDataCenter->strIDImageFile;
					if (QFailed(m_pPrinter->Printer_AddImage((char*)g_pDataCenter->strIDImageFile.c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode)))
						break;
				}
				else
				{
					gInfo() << "Error,There is no image to print for " << pSSCardInfo->strName;
					break;
				}
			}
		}
		else
		{
			gInfo() << "Try to print image:" << strPhoto.toStdString();
			if (QFailed(m_pPrinter->Printer_AddImage((char*)strPhoto.toStdString().c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode)))
				break;
		}
		if (g_pDataCenter->bDebug)
		{
			gInfo() << "In Debug mode,skip Printing!";
			return 0;
		}
		if (QFailed(m_pPrinter->Printer_StartPrint(szRCode)))
		{
			strMessage = QString("Printer_StartPrint失败，错误代码:%1!").arg(szRCode);
			break;
		}

		nResult = 0;

	} while (0);
	return nResult;
}

int DataCenter::MoveCard(QString& strMessage)
{
	//	int nRetry = 0;
	int nPowerOnPos = 4;	// 1-读磁位；2-接触IC位;3-非接IC位;4-打印位， 默认为接触位
	if (pSysConfig->DevConfig.nSSCardReaderPowerOnType == CardPowerType::READER_UNCONTACT)
		nPowerOnPos = 3;
	else if (pSysConfig->DevConfig.nSSCardReaderPowerOnType == CardPowerType::READER_CONTACT)
		nPowerOnPos = 2;
	char szRCode[1024] = { 0 };

	// 移动到打印位
	if (m_pPrinter->Printer_Dispense(pSysConfig->DevConfig.nDepenseBox - 1, 4, szRCode))
	{
		strMessage = QString("Printer_Dispense(4)失败，错误代码:%1!").arg(szRCode);
		gInfo() << gQStr(strMessage);
		return -1;
	}
	// 移动到上电位
	if (m_pPrinter->Printer_Dispense(pSysConfig->DevConfig.nDepenseBox - 1, nPowerOnPos, szRCode))
	{
		strMessage = QString("Printer_Dispense(%1)失败，错误代码:%2!").arg(nPowerOnPos).arg(szRCode);
		gInfo() << gQStr(strMessage);
		return -1;
	}
	return 0;

}

int DataCenter::ReadCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage)
{
	if (!m_pSSCardReader)
	{
		strMessage = "读卡器未就绪!";
		gInfo() << strMessage.toLocal8Bit().data();
		return -1;
	}
	int nResult = -1;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	char szRCode[32] = { 0 };
	char szCardBaseRead[1024] = { 0 };
	char szCardATR[1024] = { 0 };
	int nCardATRLen = 0;
	QString strInfo;

	do
	{
		//if (!m_pSSCardReader)
		//{
		//	strMessage = "读卡器未就绪!";
		//	break;
		//}

		if (QFailed(nResult = g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			nResult = -4;
			break;
		}
		gInfo() << "CardATR:" << szCardATR;
		strcpy(pSSCardInfo->strCardATR, szCardATR);
		char szBankNum[64] = { 0 };
		gInfo() << "Try to iReadBankNumber.";
		if (QFailed(nResult = iReadBankNumber(DevConfig.nSSCardReaderPowerOnType, szBankNum)))
		{
			strMessage = QString("读银行卡信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			break;
		}
		gInfo() << "BankNum:" << szBankNum;
		if (strlen(szBankNum) < sizeof(pSSCardInfo->strBankNum))
			strcpy(pSSCardInfo->strBankNum, szBankNum);
		else
		{
			strMessage = QString("Invalid bankNumber.");
			break;
		}
		//if (QFailed(MultiPowerOn(strCardATR, strMessage)))
		//{
		//	gInfo() << gQStr(strMessage);
		//	break;
		//}

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

		nResult = 0;

	} while (0);
	return nResult;
}


int	 DataCenter::SafeWriteCard(QString& strMessage)
{
	int nResult = -1;
	string strJsonIn, strJsonOut;
	QString strInfo;
	int nWriteCardCount = 0;

	while (nWriteCardCount < 3)
	{
		strInfo = QString("尝试第%1次写卡!").arg(nWriteCardCount + 1);
		gInfo() << gQStr(strInfo);
		nResult = g_pDataCenter->WriteCard(pSSCardInfo, strMessage);
		if (QSucceed(nResult))
		{
			break;
		}
		else if (nResult == -4)
		{
			nWriteCardCount++;
			strInfo = "写卡上电失败,尝试移动卡片!";
			gInfo() << gQStr(strInfo);
			g_pDataCenter->MoveCard(strMessage);
			continue;
		}
		else if (QFailed(nResult))
		{
			break;
		}
		nResult = 0;
	}
	return nResult;
}

void DataCenter::ResetIDData()
{
	strMobilePhone = "";
	strIDImageFile = "";
	strSSCardOldPassword = "";
	strSSCardNewPassword = "";
	strCardMakeProgress = "";
	strPayCode = "";
	strTransTime = "";
	bWithoutIDCard = false;
	pIDCard.reset();
	//if (pSSCardInfo && pSSCardInfo->strPhoto)
	//{
	//	delete[]pSSCardInfo->strPhoto;
	//	pSSCardInfo->strPhoto = nullptr;
	//}
	strPhotoBase64 = "";
	strSSCardPhotoFile = "";

	pSSCardInfo.reset();
}

int DataCenter::WriteCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage)
{
	if (!m_pSSCardReader)
	{
		strMessage = "读卡器未就绪!";
		gInfo() << strMessage.toLocal8Bit().data();
		return -1;
	}
	int nResult = -1;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	RegionInfo& Region = pSysConfig->Region;
	char szRCode[32] = { 0 };
	char szCardBaseRead[1024] = { 0 };
	char szCardBaseWrite[1024] = { 0 };
	char szExAuthData[1024] = { 0 };
	char szWHSM1[1024] = { 0 };
	/*char szWHSM2[1024] = { 0 };*/
	char szSignatureKey[1024] = { 0 };
	char szWriteCARes[1024] = { 0 };
	char szCardATR[1024] = { 0 };
	int nCardATRLen = 0;
	CAInfo caInfo;
	QString strInfo;
	HSMInfo  HsmInfo;
	int nStatus = 0;

	do
	{
		//if (!m_pSSCardReader)
		//{
		//	strMessage = "读卡器未就绪!";
		//	break;
		//}

		if (QFailed(nResult = g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			nResult = -4;
			break;
		}
		gInfo() << "CardATR:" << szCardATR;
		strcpy(pSSCardInfo->strCardATR, szCardATR);
		char szBankNum[64] = { 0 };
		if (QFailed(nResult = iReadBankNumber(DevConfig.nSSCardReaderPowerOnType, szBankNum)))
		{
			strMessage = QString("读银行卡信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			break;
		}
		gInfo() << "BankNum:" << szBankNum;
		strcpy(pSSCardInfo->strBankNum, szBankNum);

		if (QFailed(nResult = iReadCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseRead)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,Result:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
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
		//if (QFailed(MultiPowerOn(strCardATR, strMessage)))
		//{
		//	gInfo() << gQStr(strMessage);
		//	break;
		//}

		if (QFailed(nResult = iWriteCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseWrite)))
		{
			strMessage = QString("写卡片基本信息失败,PowerOnType:%1,Result:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			break;
		}
		// szCardBaseWrite输出结果为两个随机数
		// random1|random2
		gInfo() << "CardBaseRead:" << szCardBaseWrite;

		QStringList strRomdom = QString(szCardBaseWrite).split("|", Qt::KeepEmptyParts);

		strcpy(HsmInfo.strSystemID, "410700006");
		strcpy(HsmInfo.strRegionCode, Region.strCityCode.c_str());
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
		gInfo() << "HsmInfo.strRandom1 = " << HsmInfo.strRandom1 << " HsmInfo.strRandom2 = " << HsmInfo.strRandom2;

		if (QFailed(nResult = cardExternalAuth(HsmInfo, szExAuthData)))
		{
			strMessage = QString("卡片外部信息认证失败,Result:%1").arg(nResult);
			break;
		}
		gInfo() << "szExAuthData = " << szExAuthData;
		string strCardInfo = MakeCardInfo(szCardATR, pSSCardInfo);
		gInfo() << "strCardInfo = " << strCardInfo;

		if (QFailed(nResult = iWriteCardBas_HSM_Step1((char*)szExAuthData, (char*)strCardInfo.c_str(), szWHSM1)))
		{
			strMessage = QString("iWriteCardBas_HSM_Step1,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szWHSM1);
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
		if (g_pDataCenter->bDebug)
		{
			gInfo() << "Skip iWriteCA in Debug mode.";
			return 0;
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

int g_index = 0;

int WINAPI LDCam_VIS_FrameCallback(PVOID context, PUCHAR pFrameData, int nFrameLen)
{
	g_index++;
	return 0;
}

int WINAPI LDCam_NIR_FrameCallback(PVOID context, PUCHAR pFrameData, int nFrameLen)
{
	if (g_index == 10)
	{
		//WriteBMPFile("d:\\nir.bmp", pFrameData, 640, 480);
	}

	return 0;
}


int WINAPI LDCam_EventCallback(int nEventID, PVOID context, int nFrameStatus)
{
	QStackPage* pWidget = (QStackPage*)context;
	pWidget->emit LiveDetectStatusEvent(nEventID, nFrameStatus);
	return 0;
}

BOOL WriteBMPFile(const char* strFileName, PUCHAR pData, int nWidth, int nHeight)
{
	BITMAPFILEHEADER   bmfHdr;
	BITMAPINFOHEADER   bi;
	int nLen;

	FILE* f = NULL;

	BOOL bRet;

	nLen = nWidth * nHeight * 3;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = nWidth;
	bi.biHeight = nHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = nLen;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	bmfHdr.bfType = 0x4D42;  // "BM"
	bmfHdr.bfSize = sizeof(BITMAPFILEHEADER) + bi.biSize + nLen;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + bi.biSize;

	f = fopen(strFileName, "wb");
	if (NULL == f)
	{
		bRet = FALSE;
		goto done;
	}

	if (1 != fwrite(&bmfHdr, sizeof(BITMAPFILEHEADER), 1, f))
	{
		bRet = FALSE;
		goto done;
	}

	if (1 != fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, f))
	{
		bRet = FALSE;
		goto done;
	}


	if (1 != fwrite(pData, nLen, 1, f))
	{
		bRet = FALSE;
		goto done;
	}

	bRet = TRUE;

done:

	if (f)
		fclose(f);

	return bRet;
}

int DataCenter::OpenCom(int nPort, int nBraud)
{
	if (pBinBoard)
		return true;
	else
	{
		QString strMessage;
		try
		{
			DeviceConfig& devConfig = GetSysConfigure()->DevConfig;
			pBinBoard = new CPinbroad(devConfig.nPinBoardType);
			if (!pBinBoard)
			{
				strMessage = "内存不足，无法生成密码键盘类对象!";
				gError()<< gQStr(strMessage);
				return -1;
			}
			return pBinBoard->OpenCom(nPort, nBraud);
		}
		catch (std::exception &e)
		{
			strMessage = e.what();
			gError() << "Catch a exception:" << e.what();
			return -1;
		}
	}
}

void DataCenter::CloseCom()
{
	if (!pBinBoard)
		return ;
	pBinBoard->CloseCom();
	delete pBinBoard;
	pBinBoard = nullptr;
}

int DataCenter::UseEppPlainTextMode(unsigned char ucTextModeFormat, unsigned char AutoEnd, unsigned char* ReturnInfo)
{
	if (!pBinBoard)
		return -1;
	return pBinBoard->UseEppPlainTextMode(ucTextModeFormat, AutoEnd, ReturnInfo);
}

int  DataCenter::ScanKeyPress(unsigned char* ucKeyValue)
{
	if (!pBinBoard)
		return -1;
	return pBinBoard->ScanKeyPress(ucKeyValue);
}

bool DataCenter::IsVideoStart()
{
	return m_bVideoStarted;
}
bool DataCenter::OpenCamera()
{
	int nRet = -1;
	do
	{
		if (m_hCamera)
		{
			nRet = LD_RET_OK;
			break;
		}

		nBufferSize = m_nWidth * m_nHeight * 4;
		pImageBuffer = new byte[nBufferSize];
		if (!pImageBuffer)
			break;

		char szProductInfo[8192] = { 0 };
		nRet = DVTGKLDCam_GetProduct(szProductInfo, 8192);
		qDebug() << szProductInfo;

		nRet = DVTGKLDCam_Open(&m_hCamera);
		if (nRet != LD_RET_OK)
			break;

		nRet = DVTGKLDCam_SetFrameCallback(m_hCamera, 0, LDCam_VIS_FrameCallback, this);
		if (nRet != LD_RET_OK)
			break;

		nRet = DVTGKLDCam_SetFrameCallback(m_hCamera, 1, LDCam_NIR_FrameCallback, this);
		if (nRet != LD_RET_OK)
			break;

	} while (0);
	if (nRet != LD_RET_OK)
		return false;
	else
		return true;
}

bool DataCenter::StartVideo(HWND hWnd)
{
	int nRet = -1;
	do
	{
		if (!m_hCamera)
			break;

		if (m_bVideoStarted)
			return m_bVideoStarted;

		if (!IsWindow(hWnd))
			break;

		nRet = DVTGKLDCam_SetCamFormat(m_hCamera, 0, LD_FMT_MJPG, m_nWidth, m_nHeight);
		if (nRet != LD_RET_OK)
			break;

		nRet = DVTGKLDCam_SetCamFormat(m_hCamera, 1, LD_FMT_MJPG, m_nWidth, m_nHeight);
		if (nRet != LD_RET_OK)
			break;
		RECT rtWnd;
		GetWindowRect(hWnd, &rtWnd);
		int nWndWidth = rtWnd.right - rtWnd.left;
		int nWndHeight = rtWnd.bottom - rtWnd.top;
		nRet = DVTGKLDCam_StartVideo(m_hCamera, (HWND)hWnd, 0, 0, nWndWidth, nWndHeight);
		if (nRet != LD_RET_OK)
			break;
		QString strText[] = { "人脸检测正在进行" ,"人脸检测失败，超时" ,"人脸检测成功" };
		DVTGKLDCam_SetDetectText(m_hCamera, TEXT_ONGOING, 30, (char*)strText[0].toLocal8Bit().toStdString().c_str());
		DVTGKLDCam_SetDetectText(m_hCamera, TEXT_FAIL_TIMEOUT, 30, (char*)strText[1].toLocal8Bit().toStdString().c_str());
		DVTGKLDCam_SetDetectText(m_hCamera, TEXT_SUCCESS, 30, (char*)strText[2].toLocal8Bit().toStdString().c_str());
		m_bVideoStarted = true;
	} while (0);
	return m_bVideoStarted;
}

void DataCenter::StopVideo()
{
	if (!m_hCamera)
		return;

	if (!m_bVideoStarted)
		return;

	DVTGKLDCam_StopVideo(m_hCamera);
	m_bVideoStarted = false;
}

void DataCenter::CloseCamera()
{
	if (m_hCamera)
	{
		StopFaceDetect();
		DVTGKLDCam_Close(m_hCamera);
		delete[]pImageBuffer;
		pImageBuffer = nullptr;
		m_hCamera = nullptr;
	}
}

bool DataCenter::StartFaceDetect(void* pContext, int nDetectMilliSeconds, int nTimeoutMilliSeconds)
{
	if (!m_hCamera)
		return false;

	if (LD_RET_OK != DVTGKLDCam_SetEventCallback(m_hCamera, LDCam_EventCallback, pContext))
		return false;

	if (LD_RET_OK != DVTGKLDCam_StartDetection(m_hCamera, nDetectMilliSeconds, nTimeoutMilliSeconds))
		return false;
	m_bDetectStarted = true;
	return true;
}

bool DataCenter::SaveFaceImage(string strPhotoFile, bool bFull)
{
	if (!m_hCamera)
		return false;
	int nDataLen = 0;
	bool bSucceed = false;
	ZeroMemory(pImageBuffer, nBufferSize);
	//if (bFull)
	//{
	try
	{
		if (fs::exists(strPhotoFile))
		{
			fs::remove(strPhotoFile);
		}
		int nRes = DVTGKLDCam_Snapshot(m_hCamera, IMG_JPG, pImageBuffer, nBufferSize, &nDataLen);
		if (nRes != LD_RET_OK)
			return false;
		ofstream ofs(strPhotoFile, ios::out | ios::binary);
		//WriteBMPFile(strPhotoFile.c_str(), pImageBuffer, m_nWidth, m_nHeight);
		string strBuffer((char*)pImageBuffer, nDataLen);
		ofs << strBuffer;
		ofs.flush();
		bSucceed = true;
	}
	catch (std::exception& e)
	{
		gInfo() << "保存图片发生异常:" << e.what();
		bSucceed = false;
	}

	/*}
	else
	{
		int nFaceWidth = 0, nFaceHeight = 0;
		if (LD_RET_OK == DVTGKLDCam_GetFaceImage(m_hCamera, IMG_JPG, pImageBuffer, nBufferSize, &nFaceWidth, &nFaceHeight, &nDataLen))
		{
			WriteBMPFile(strPhotoFile.c_str(), pImageBuffer, nFaceWidth, nFaceHeight);
			bSucceed = true;
		}
	}*/
	return bSucceed;
}

bool DataCenter::FaceCompareByImage(string strFacePhoto1, string strFacePhoto2, float& dfSimilarity)
{
	if (!m_hCamera)
		return false;
	QFileInfo fi1(strFacePhoto1.c_str());

	if (!fi1.isFile())
		return false;

	QFileInfo fi2(strFacePhoto2.c_str());
	if (!fi2.isFile())
		return false;

	QImage FaceImage1(strFacePhoto1.c_str());
	if (FaceImage1.isNull())
		return false;

	QImage FaceImage2(strFacePhoto2.c_str());
	if (FaceImage1.isNull())
		return false;
	int nRet = DVTGKLDCam_FaceCompFeature(m_hCamera, strFacePhoto1.c_str(), strFacePhoto2.c_str(), FaceImage1.height(), FaceImage1.width(), FaceImage2.height(), FaceImage2.width(), &dfSimilarity);
	if (LD_RET_OK != nRet)
		return false;

	return true;
}

void DataCenter::StopFaceDetect()
{
	if (m_bDetectStarted)
	{
		DVTGKLDCam_StopDetection(m_hCamera);
		m_bDetectStarted = false;
	}
}

bool DataCenter::SwitchVideoWnd(HWND hWnd)
{
	if (!IsWindow(hWnd))
		return false;

	if (!m_hCamera)
		return false;

	RECT rtWnd;
	GetWindowRect(hWnd, &rtWnd);
	int nWndWidth = rtWnd.right - rtWnd.left;
	int nWndHeight = rtWnd.bottom - rtWnd.top;
	return LD_RET_OK == DVTGKLDCam_UpdateWindow(m_hCamera, hWnd, 0, 0, nWndWidth, nWndHeight);
}

bool DataCenter::Snapshot(string strFilePath)
{
	if (m_hCamera)
	{
		int nDataLen = 0;
		ZeroMemory(pImageBuffer, nBufferSize);
		int nRet = DVTGKLDCam_Snapshot(m_hCamera, IMG_RGB24, pImageBuffer, nBufferSize, &nDataLen);
		if (nRet == LD_RET_OK)
		{
			WriteBMPFile(strFilePath.c_str(), pImageBuffer, m_nWidth, m_nHeight);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
		return false;
}

bool GetModuleVersion(QString strModulePath, WORD& nMajorVer, WORD& nMinorVer, WORD& nBuildNum, WORD& nRevsion)
{
	DWORD dwHnd;
	DWORD dwVerInfoSize;
	wchar_t pszVersion[2048] = { 0 };
	wchar_t szModulePath[4096] = { 0 };
	bool bResult = false;
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
		bResult = true;
	} while (0);
	return bResult;
}

bool DataCenter::LoadAdminConfigure()
{
	QString strDocPath = qApp->applicationDirPath() + "/Data/Adminster.json";
	QFile jsfile(strDocPath);
	if (!jsfile.open(QIODevice::ReadOnly))
		return false;

	QByteArray byData = jsfile.readAll();
	if (byData.isEmpty())
		return false;

	//AesTools AesDecrypt((unsigned char*)szAesKey, 16);
	//unsigned char* pBuffer = new unsigned char[byData.size() + 1];
	//shared_ptr<unsigned char> pBufferFree(pBuffer);
	//int nDataLen = AesDecrypt.Decrypt((unsigned char*)byData.data(), byData.size(), pBuffer);
	//qDebug() << (char*)pBuffer;
	//QByteArray baJson((char*)pBuffer, nDataLen);
	QJsonParseError jsErr;
	QJsonDocument jsdoc = QJsonDocument::fromJson(byData, &jsErr);
	if (jsdoc.isNull())
	{
		qDebug() << jsErr.errorString();
		return false;
	}
	vecAdminister.clear();
	QJsonArray jsArray = jsdoc.array();
	for (auto var : jsArray)
	{
		auto jsObj = var.toObject();
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
		QString strName = jsObj.value("Name").toString();
		QString strID = jsObj.value("ID").toString();
		QString strGender = jsObj.value("Gender").toString();
		strcpy((char*)pIDCard->szName, strName.toLocal8Bit());
		strcpy((char*)pIDCard->szIdentity, strID.toLocal8Bit());
		strcpy((char*)pIDCard->szGender, strGender.toLocal8Bit());
		vecAdminister.push_back(pIDCard);
	}
	return true;
}

int DataCenter::ReadSSCardInfo(SSCardInfoPtr& pSSCard, int& nStatus, QString& strMessage)
{
	int nResult = 0;
	char szStatus[1024] = { 0 };
	if (QFailed(nResult = queryPersonInfo(*pSSCard, szStatus)))
	{
		FailureMessage("查询人员信息", pSSCard, szStatus, strMessage);
		gError() << gQStr(strMessage);
		return -1;
	}
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

int	 DataCenter::QuerySSCardStatus(SSCardInfoPtr& pSSCard, QString& strMessage)
{
	int nResult = 0;
	char* szStatus[1024] = { 0 };
	// 这里需要旧卡号
	if (QFailed(nResult = queryCardStatus(*pSSCard, reinterpret_cast<char*>(szStatus))))
	{
		FailureMessage("查询卡状态", pSSCard, szStatus, strMessage);
		/*if (strcmp((const char*)szStatus, "08") == 0)
		{
			strMessage = QString("查询卡状态失败:人社服务器没有响应,可能网络异常或人社服务器故障\n姓名:%1\t卡号:%2\t").arg(pSSCardInfo->strName).arg(pSSCardInfo->strCardNum);
		}
		else
			strMessage = QString("查询卡状态失败:%1\n姓名:%2\t卡号:%3\t").arg(nResult).arg(pSSCardInfo->strName).arg(pSSCardInfo->strCardNum);*/
		gError() << gQStr(strMessage);
		return -1;
	}

	if (strcmp(_strupr((char*)szStatus), "OK") == 0)
	{
		strcpy(pSSCard->strCardStatus, "正常");
	}
	else
	{

		QString strStatus = QString::fromLocal8Bit((char*)szStatus);
		strcpy(pSSCard->strCardStatus, (char*)strStatus.toStdString().c_str());
	}

	return 0;
}

char VerifyCardID(const char* pszSrc)
{
	int iS = 0;
	int iW[] = { 7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2 };
	static char szVerCode[] = "10X98765432";
	int i;
	for (i = 0; i < 17; i++)
	{
		iS += (int)(pszSrc[i] - '0') * iW[i];
	}
	int iY = iS % 11;
	return szVerCode[iY];
}

// 需提前把要处理的图片放在./PhotoProcess目录下，并命名为1.jpg
int ProcessHeaderImage(QString& strHeaderPhoto, QString& strMessage)
{
	QWaitCursor Wait;
	QString strAppPath = QApplication::applicationDirPath();
	QString strCurrentPath = strAppPath + "/PhotoProcess";
	QString strProcessPath = strAppPath + "/PhotoProcess/MattingTool.bin";

	try
	{
		if (!fs::exists(strProcessPath.toStdString()))
		{
			strMessage = "找不到人像处理组件:MattingTool.bin";
			return -1;
		}

		QString strPhotoPath2 = strAppPath + "/PhotoProcess/2.jpg";

		if (fs::exists(strPhotoPath2.toStdString()))
			fs::remove(strPhotoPath2.toStdString());

		wchar_t wszCurrentPath[1024] = { 0 };
		wcscpy_s(wszCurrentPath, 1024, strCurrentPath.toStdWString().c_str());
		QProcess tProcess;
		tProcess.setCreateProcessArgumentsModifier([&](QProcess::CreateProcessArguments* args)
			{
				//args->flags |= CREATE_NEW_CONSOLE;
				args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
				args->startupInfo->dwFlags |= STARTF_USEFILLATTRIBUTE;
				args->startupInfo->dwFillAttribute = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
				args->currentDirectory = wszCurrentPath;
			});
		tProcess.start(strProcessPath);
		tProcess.waitForFinished();

		QFileInfo fi2(strPhotoPath2);
		if (!fi2.isFile())
		{
			strMessage = "人像处理失败!";
			return -1;
		}
		strHeaderPhoto = strPhotoPath2;
		return 0;

	}
	catch (std::exception& e)
	{
		gInfo() << "发生异常" << e.what();
	}
	return -1;
}

void EnableWidgets(QWidget* pUIObj, bool bEnable)
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
		EnableWidgets((QWidget*)pObj, bEnable);
	}
	pUIObj->setEnabled(true);
}

int GetAge(string strBirthday)
{
	if (strBirthday.empty())
		return -1;

	int nBirthYear, nBirthMonth, nBirthDay;
	int nCount = sscanf_s(strBirthday.c_str(), "%04d%02d%02d", &nBirthYear, &nBirthMonth, &nBirthDay);
	if (nCount != 3)
		return -1;

	time_t tNow = time(nullptr);
	tm* tmNow = localtime(&tNow);
	tmNow->tm_year += 1900;
	tmNow->tm_mon += 1;

	int nAge = tmNow->tm_year - nBirthYear;
	if (tmNow->tm_mon < nBirthMonth)
	{
		nAge--;
	}
	else if (tmNow->tm_mon == nBirthMonth)
	{
		if (tmNow->tm_mday < nBirthDay)
			nAge--;
	}
	return nAge;
}

tuple<ProgrerssType, string,CJsonObjectPtr > FindCardProgress(const char* szIdentify, SSCardInfoPtr& pSSCard)
{
	pSSCard = make_shared<SSCardInfo>();
	string strCardDataMaking = QString("%1/data/Progress_%2.json").arg(qApp->applicationDirPath()).arg((char*)szIdentify).toLocal8Bit().data();
	//string strCardDataFinished = QString("%1/Finished/Progress_%2.json").arg(qApp->applicationDirPath()).arg((char*)szIdentify).toLocal8Bit().data();

	string strCurProgressFile = "";
	ProgrerssType nProgress = ProgrerssType::Progress_UnStart;
	optional<CJsonObjectPtr> optProgress;
	/*auto pTellProgress = [&](string &strProgressFile) 
	{
		optProgress = g_pDataCenter->LoadSSCardData(strProgressFile, pSSCard);
		if (optProgress)
		{
			do
			{
				int nEnableCard = g_pDataCenter->GetProgressStatus("EnableCard", optProgress.value());
				int nMarkcard = g_pDataCenter->GetProgressStatus("MarkCard", optProgress.value());
				if (nMarkcard == -1)
					break;
				else if (nMarkcard == 0)
					nProgress = ProgrerssType::Progress_UnStart;
				else if (nMarkcard == 1)
				{
					if (nEnableCard == 0)
						nProgress = ProgrerssType::Progress_Making;
					else if (nEnableCard == 1)
						nProgress = ProgrerssType::Progress_Finished;
				}
				strCurProgressFile = strProgressFile;
			} while (0);
			return true;
		}
		else
		{
			gError() << "Failed in load file " << strProgressFile;
			return false;
		}
	};*/
	if (fs::exists(strCardDataMaking))
	{//检查制卡中的数据
		//pTellProgress(strCardDataMaking);
		optProgress = g_pDataCenter->LoadSSCardData(strCardDataMaking, pSSCard);
		if (optProgress)
		{
			do
			{
				int nEnableCard = g_pDataCenter->GetProgressStatus("EnableCard", optProgress.value());
				int nMarkcard = g_pDataCenter->GetProgressStatus("MarkCard", optProgress.value());
				if (nMarkcard == -1)
					break;
				else if (nMarkcard == 0)
					nProgress = ProgrerssType::Progress_UnStart;
				else if (nMarkcard == 1)
				{
					if (nEnableCard == 0)
						nProgress = ProgrerssType::Progress_Making;
					else if (nEnableCard == 1)
						nProgress = ProgrerssType::Progress_Finished;
				}
				strCurProgressFile = strCardDataMaking;
			} while (0);
			return { nProgress,strCurProgressFile,optProgress.value() };
		}
		else
		{
			gError() << "Failed in load file " << strCardDataMaking;
			return { nProgress,strCurProgressFile,nullptr };
		}
	}
	else
	{
		return { nProgress,"",nullptr};
	}
			
}

int LoadPersonInfo(QString strJson,IDCardInfoPtr & pIDCard)
{
	try
	{
		string strProgressFile = strJson.toLocal8Bit().data();
		if (!fs::exists(strProgressFile))
			return 1;

		ifstream ifs(strProgressFile, ios::in | ios::binary);
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObject json;
		if (!json.Parse(ss.str()))
			return 1;

		GetJsonValue(json, "Identity", pIDCard->szIdentity);
		GetJsonValue(json, "Name", pIDCard->szName);
		GetJsonValue(json, "Gender", pIDCard->szGender);
		GetJsonValue(json, "Birthday", pIDCard->szBirthday);
		GetJsonValue(json, "Nation", pIDCard->szNationalty);
		GetJsonValue(json, "Address", pIDCard->szAddress);
		GetJsonValue(json, "PaperIssuedate", pIDCard->szExpirationDate1);
		GetJsonValue(json, "PaperExpiredate", pIDCard->szExpirationDate2);
		return 0;
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return 1;
	}
}