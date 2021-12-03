#pragma execution_character_set("utf-8")
#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include "Gloabal.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")

#pragma comment(lib, "../SDK/Printer/KT_Printer")
#pragma comment(lib, "../SDK/Reader/KT_Reader")
#pragma comment(lib, "../SDK/SSCardDriver/SSCardDriver")
#pragma comment(lib, "../SDK/SSCardHSM/SSCardHSM")
#pragma comment(lib, "../SDK/SSCardInfo/SSCardInfo")
#pragma comment(lib, "../SDK/libcurl/libcurl")

extern DataCenterPtr g_pDataCenter;
DataCenter::DataCenter()
{
	BaseInfo Bi;
	RegionInfo& region = g_pDataCenter->GetSysConfigure()->Region;
	strcpy(Bi.strEMUrl, region.strEMURL.c_str());
	strcpy(Bi.strAccount, region.strEMAccount.c_str());
	strcpy(Bi.strPassword, region.strEMPassword.c_str());
	if (!InitEnv(Bi))
	{
		gInfo() << "Failed in InitEnv";
	}
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

struct SafeBuffer
{
	unsigned int nDataLength;
	unsigned int nBufferSize;
	FILE* fp;
	byte* pBuffer;
};

int WriteDate(void* buffer, size_t sz, size_t nmemb, void* ResInfo)
{
	SafeBuffer* pRespond = (SafeBuffer*)ResInfo;
	if (!pRespond || !pRespond->pBuffer || !pRespond->nBufferSize)
		return sz * nmemb;
	if ((pRespond->nDataLength + sz * nmemb) < pRespond->nBufferSize)
		memcpy(&pRespond->pBuffer[pRespond->nDataLength], buffer, sz * nmemb);
	// 	if (pRespond->fp)
	// 	{
	// 		fwrite(buffer, sz * nmemb, 1, pRespond->fp);
	// 	}
	return sz * nmemb;  //返回接受数据的多少
}


#define __countof(array) (sizeof(array)/sizeof(array[0]))
#pragma warning (disable:4996)

void TraceMsgLog(LPCSTR pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);
	int nBuff;
	CHAR szBuffer[8192];
	nBuff = _vsnprintf_s(szBuffer, __countof(szBuffer), pFormat, args);
	//::wvsprintf(szBuffer, pFormat, args);
	//assert(nBuff >=0);
	if (nBuff)
	{
		gInfo() << szBuffer;
	}
	else
	{
		gInfo() << "====================TraceMsgLog Error====================\n";
	}
	va_end(args);
}

int SendHttpRequest(char* szUrl, char* pRespond)
{
	CURLcode nCurResult;
	long retcode = 0;
	CURL* pCurl = nullptr;
	CHAR szError[1024] = { 0 };
	__try
	{
		nCurResult = curl_easy_setopt(pCurl, CURLOPT_URL, szUrl);
		curl_easy_reset(pCurl);
		nCurResult = curl_easy_perform(pCurl);
		if (nCurResult != CURLE_OK)
		{
			strcpy(szError, curl_easy_strerror(nCurResult));
			TraceMsgLog("curl_easy_perform Failed:%s.\n", szError);
			__leave;
		}

		nCurResult = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &retcode);
		if (nCurResult != CURLE_OK)
		{
			strcpy(szError, curl_easy_strerror(nCurResult));
			TraceMsgLog("curl_easy_perform Failed:%s.\n", szError);
			__leave;
		}

		if (retcode == 401)
		{
			long nAuthorize;
			nCurResult = curl_easy_getinfo(pCurl, CURLINFO_HTTPAUTH_AVAIL, &nAuthorize);

			if (nCurResult != CURLE_OK)
			{
				strcpy(szError, curl_easy_strerror(nCurResult));
				TraceMsgLog("curl_easy_perform Failed:%s.\n", szError);
				__leave;
			}

			if (!nAuthorize)
			{
				TraceMsgLog("%s No auth available, perhaps no 401?\n", __FUNCTION__);
				__leave;
			}
			TraceMsgLog("%s Change authorize.\n", __FUNCTION__);
			if (nAuthorize & CURLAUTH_DIGEST)
			{
				nCurResult = curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
				TraceMsgLog("%s authorize CURLAUTH_DIGEST.\n", __FUNCTION__);
			}
			else if (nAuthorize & CURLAUTH_BASIC)
			{
				nCurResult = curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
				TraceMsgLog("%s authorize CURLAUTH_BASIC.\n", __FUNCTION__);
			}
			else if (nAuthorize & CURLAUTH_NEGOTIATE)
			{
				nCurResult = curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_NEGOTIATE);
				TraceMsgLog("%s authorize CURLAUTH_NEGOTIATE.\n", __FUNCTION__);
			}
			else if (nAuthorize & CURLAUTH_NTLM)
			{
				nCurResult = curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_NTLM);
				TraceMsgLog("%s authorize CURLAUTH_NTLM.\n", __FUNCTION__);
			}
			if (nCurResult != CURLE_OK)
			{
				strcpy(szError, curl_easy_strerror(nCurResult));
				TraceMsgLog("curl_easy_perform Failed:%s.\n", szError);
				__leave;
			}

			nCurResult = curl_easy_perform(pCurl);
			if (nCurResult != CURLE_OK)
			{
				strcpy(szError, curl_easy_strerror(nCurResult));
				TraceMsgLog("curl_easy_perform Failed:%s.\n", szError);
				__leave;
			}

			nCurResult = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &retcode);
		}
		if (pRespond)
		{
			SafeBuffer* pSafeBuffer = (SafeBuffer*)pRespond;
			if (pSafeBuffer->pBuffer && pSafeBuffer->nBufferSize)
			{
				curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteDate);
				curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pRespond);
			}
		}
	}
	__finally
	{
		if (nCurResult != CURLE_OK)
		{
			CHAR szError[1024] = { 0 };
			strcpy(szError, curl_easy_strerror(nCurResult));
			TraceMsgLog("%s Error=%s.\n", __FUNCTION__, szError);
		}
		if (pCurl)
		{
			curl_easy_cleanup(pCurl);
			pCurl = nullptr;
		}
	}

	return (UINT)retcode;
}
