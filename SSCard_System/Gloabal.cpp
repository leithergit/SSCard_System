#pragma execution_character_set("utf-8")
#include <QSettings>
#include <QDir>
#include <QApplication>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QTextCodec>
#include "Gloabal.h"
#include "Payment.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")
#pragma comment(lib,"Version.lib")
#ifdef _DEBUG
#pragma comment(lib, "../SDK/KT_Printer/KT_Printerd")
#pragma comment(lib, "../SDK/KT_Reader/KT_Readerd")
#pragma comment(lib, "../SDK/SSCardDriver/SSCardDriverd")
#pragma comment(lib, "../SDK/SSCardHSM/SSCardHSMd")
#pragma comment(lib, "../SDK/SSCardInfo/SSCardInfod")
#pragma comment(lib, "../SDK/libcurl/libcurld")
#pragma comment(lib, "../SDK/QREncode/qrencoded")
#pragma comment(lib, "../SDK/IDCard/IDCard_API")
#pragma comment(lib, "../SDK/glog/glogd")
#pragma comment(lib, "../SDK/PinKeybroad/XZ_F31_API")
#pragma comment(lib, "../SDK/7Z/lib/bit7z_d.lib")
#else
#pragma comment(lib, "../SDK/KT_Printer/KT_Printer")
#pragma comment(lib, "../SDK/KT_Reader/KT_Reader")
#pragma comment(lib, "../SDK/SSCardDriver/SSCardDriver")
#pragma comment(lib, "../SDK/SSCardHSM/SSCardHSM")
#pragma comment(lib, "../SDK/SSCardInfo/SSCardInfo")
#pragma comment(lib, "../SDK/libcurl/libcurl")
#pragma comment(lib, "../SDK/QREncode/qrencode")
#pragma comment(lib, "../SDK/IDCard/IDCard_API")
#pragma comment(lib, "../SDK/glog/glog")
#pragma comment(lib, "../SDK/PinKeybroad/XZ_F31_API")
#pragma comment(lib, "../SDK/7Z/lib/bit7z.lib")
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
QScreen* g_pCurScreen = nullptr;
DataCenter::DataCenter()
{
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
			strError = QString("加载卡版打印版式失败:%1!").arg(strConfigPath);
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

int HttpRecv(void* buffer, size_t sz, size_t nmemb, void* ResInfo)
{
	HttpBuffer* pRespond = (HttpBuffer*)ResInfo;
	if (!pRespond->pBuffer)
		return sz * nmemb;
	if (!pRespond || !pRespond->pBuffer || !pRespond->nBufferSize)
		return sz * nmemb;
	if ((pRespond->nDataLength + sz * nmemb) < pRespond->nBufferSize)
	{
		memcpy(&pRespond->pBuffer[pRespond->nDataLength], buffer, sz * nmemb);
		pRespond->nDataLength += sz * nmemb;
	}

	if (pRespond->fp)
	{
		fwrite(buffer, sz * nmemb, 1, pRespond->fp);
	}
	return sz * nmemb;  //返回接受数据的多少
}

bool SendHttpRequest(string szUrl, string& strRespond, string& strMessage)
{
	CURLcode nCurResult;
	long retcode = 0;
	CURL* pCurl = nullptr;
	bool bSucceed = false;
	int nHttpCode = 200;
	HttpBuffer hb;
	do
	{
		pCurl = curl_easy_init();
		if (pCurl == NULL)
			break;
		nCurResult = curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5L);			//请求超时时长
		curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 5L);	//连接超时时长 
		curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);	//允许重定向
		curl_easy_setopt(pCurl, CURLOPT_HEADER, 1L);			//若启用，会将头文件的信息作为数据流输出
		curl_easy_setopt(pCurl, CURLOPT_TCP_NODELAY, 0L);
		curl_easy_setopt(pCurl, CURLOPT_TCP_FASTOPEN, 1L);

		HttpBuffer* pHttpBuffer = (HttpBuffer*)&hb;
		if (pHttpBuffer->pBuffer && pHttpBuffer->nBufferSize)
		{
			curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, HttpRecv);
			curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pHttpBuffer);
		}

		curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L); //关闭中断信号响应
		curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L); //启用时会汇报所有的信息
		nCurResult = curl_easy_setopt(pCurl, CURLOPT_URL, szUrl.c_str());

		curl_slist* pList = NULL;
		pList = curl_slist_append(pList, "Accept-Encoding:gzip, deflate, sdch");
		pList = curl_slist_append(pList, "Accept-Language:zh-CN,zh;q=0.8");
		curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pList);

		nCurResult = curl_easy_perform(pCurl);
		if (nCurResult != CURLE_OK)
			break;

		nCurResult = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &nHttpCode);
		if (nCurResult != CURLE_OK)
			break;

		if (retcode == 401)
		{
			long nAuthorize;
			nCurResult = curl_easy_getinfo(pCurl, CURLINFO_HTTPAUTH_AVAIL, &nAuthorize);

			if (nCurResult != CURLE_OK)
				break;

			if (!nAuthorize)
				break;
			if (nAuthorize & CURLAUTH_DIGEST)
				nCurResult = curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
			else if (nAuthorize & CURLAUTH_BASIC)
				nCurResult = curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
			else if (nAuthorize & CURLAUTH_NEGOTIATE)
				nCurResult = curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_NEGOTIATE);
			else if (nAuthorize & CURLAUTH_NTLM)
				nCurResult = curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_NTLM);
			if (nCurResult != CURLE_OK)
				break;

			nCurResult = curl_easy_perform(pCurl);
			if (nCurResult != CURLE_OK)
				break;
		}

	} while (0);

	if (nCurResult != CURLE_OK)
	{
		strMessage = curl_easy_strerror(nCurResult);
	}
	else
	{
		strRespond = string((const char*)hb.pBuffer, hb.nDataLength);
		nCurResult = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &nHttpCode);
		if (nHttpCode == 200)
			bSucceed = true;
	}

	if (pCurl)
	{
		curl_easy_cleanup(pCurl);
		pCurl = nullptr;
	}
	return bSucceed;
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


int DataCenter::OpenDevice(QString& strMessage)
{
	int nResult = -1;
	if (QFailed(nResult = OpenPrinter(strMessage)))
	{
		gError() << gQStr(strMessage);
		return nResult;
	}

	if (QFailed(nResult = OpenSSCardReader(strMessage)))
	{
		gError() << gQStr(strMessage);
		return nResult;
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
					strMessage = QString("Printer_Init‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
					break;
				}

				if (QFailed(nResult = m_pPrinter->Printer_Open(szRCode)))
				{
					strMessage = QString("Printer_Open‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
					break;
				}
			}
			else
				return 0;

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

int DataCenter::OpenSSCardReader(QString strLib, ReaderBrand nReaderType, QString& strMessage)
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

int DataCenter::TestPrinter(QString& strMessage)
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
			bSucceed = true;
			break;
		}
		case 1:			// 1-卡在门口:
		{
			strMessage = QString("打印机出卡号尚有未取走卡片,取走点击确定以继续!");
			nResult = 1;		//
			break;
		}
		case 2:			// 2-卡在内部；
		case 3:			// 3-卡在上电位
		{
			if (QFailed(m_pPrinter->Printer_Retract(1, szRCode)))
			{
				strMessage = QString("打印机尚有未取走卡片,尝试将其移动到回收箱失败，错误代码:%1!").arg(szRCode);
				break;
			}
			else
				bSucceed = true;
			break;
		}
		case 4:			// 4-卡在闸门外
		{
			strMessage = QString("请先取走出卡口的卡片!");
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
		// 0-FLLL;1-LOW;2-OUT;3-NOTSUPP;4-UNKNOW
		bSucceed = false;
		switch (PrinterStatus.fwToner)
		{
		case 0:
		case 1:
		{
			bSucceed = true;
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
		default:
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

		if (QFailed(nResult = TestPrinter(strMessage)))
		{
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

		if (QFailed(nResult = m_pPrinter->Printer_InitPrint(nullptr, szRCode)))
		{
			strMessage = QString("Printer_InitPrint失败，错误代码:%1!").arg(szRCode);
			break;
		}

		TextPosition* pFieldPos = &pCardForm->posName;
		if (bPrintText)
		{
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strName, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
			pFieldPos = &pCardForm->posIDNumber;
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardID, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
			pFieldPos = &pCardForm->posSSCardID;
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardNum, pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
			pFieldPos = &pCardForm->posIssueDate;
			int nYear, nMonth, nDay;

			sscanf_s(pSSCardInfo->strReleaseDate, "%04d%02d%02d", &nYear, &nMonth, &nDay);
			char szReleaseDate[32] = { 0 };
			sprintf_s(szReleaseDate, 32, "%d年%d月", nYear, nMonth);
			m_pPrinter->Printer_AddText((char*)UTF8_GBK(szReleaseDate).c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		}

		ImagePosition& ImgPos = pCardForm->posImage;
		if (!strPhoto.size())
			m_pPrinter->Printer_AddImage((char*)strSSCardPhotoFile.c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode);
		else
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
		if (!m_pSSCardReader)
		{
			strMessage = "读卡器未就绪!";
			break;
		}

		//if (QFailed(MoveCard(strMessage)))
		//{
		//	gInfo() << gQStr(strMessage);
		//	break;
		//}
		if (QFailed(nResult = g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
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
		//if (QFailed(MultiPowerOn(strCardATR, strMessage)))
		//{
		//	gInfo() << gQStr(strMessage);
		//	break;
		//}

		if (QFailed(nResult = iWriteCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseWrite)))
		{
			strMessage = QString("写卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
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

		if (QFailed(nResult = cardExternalAuth(HsmInfo, szExAuthData)))
		{
			strMessage = QString("卡版外部信息认证失败,Result:%1").arg(nResult);
			break;
		}
		gInfo() << "szExAuthData = " << szExAuthData;
		string strCardInfo = MakeCardInfo(szCardATR, pSSCardInfo);
		gInfo() << "strCardInfo = " << strCardInfo;
		//if (QFailed(MultiPowerOn(strCardATR, strMessage)))
		//{
		//	gInfo() << gQStr(strMessage);
		//	break;
		//}
		if (QFailed(nResult = iWriteCardBas_HSM_Step1((char*)szExAuthData, (char*)strCardInfo.c_str(), szWHSM1)))
		{
			strMessage = QString("iWriteCardBas_HSM_Step1,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szWHSM1);
			break;
		}
		gInfo() << "szWHSM1 = " << szWHSM1;
		//if (QFailed(MultiPowerOn(strCardATR, strMessage)))
		//{
		//	gInfo() << gQStr(strMessage);
		//	break;
		//}
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

		//if (QFailed(MultiPowerOn(strCardATR, strMessage)))
		//{
		//	gInfo() << gQStr(strMessage);
		//	break;
		//}

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
