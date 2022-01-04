#pragma execution_character_set("utf-8")
#include <QSettings>
#include <QDir>
#include <QApplication>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QTextCodec>
#include "Gloabal.h"

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
#pragma comment(lib, "../SDK/glog/glog")
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
		pConfig = make_shared<SysConfig>(&ConfigIni);
		if (!pConfig)
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
		{QMessageBox::YesToAll          ,QObject::tr("全部是")},
		{QMessageBox::No                ,QObject::tr("否")},
		{QMessageBox::NoToAll           ,QObject::tr("全都不")},
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
		{QMessageBox::YesAll            ,QObject::tr("全部是")},
		{QMessageBox::NoAll             ,QObject::tr("全部否")},
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
