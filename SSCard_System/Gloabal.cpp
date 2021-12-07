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

#pragma comment(lib, "../SDK/Printer/KT_Printerd")
#pragma comment(lib, "../SDK/Reader/KT_Readerd")
#pragma comment(lib, "../SDK/SSCardDriver/SSCardDriverd")
#pragma comment(lib, "../SDK/SSCardHSM/SSCardHSMd")
#pragma comment(lib, "../SDK/SSCardInfo/SSCardInfod")
#pragma comment(lib, "../SDK/libcurl/libcurl")

extern DataCenterPtr g_pDataCenter;
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
