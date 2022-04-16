#include "Update.h"
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QString>
#include <QFileInfo>
#include <QProcess>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDirIterator>
#include <Windows.h>
#include <process.h>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include "../SDK/libcurl/curl.h"
#include "../SDK/glog/logging.h"
#include "../SDK/7Z/include/bitextractor.hpp"
#include "../SDK/7Z/include/bitarchiveinfo.hpp"
#include "../SDK/7Z/include/bitexception.hpp"
#include "../SDK/7Z/include/bitcompressor.hpp"

#define gInfo()      LOG(INFO)
#define gError()     LOG(ERROR)
#define gWarning()   LOG(WARNING)
#define Str(x)       #x
#define gVal(p)      #p<<" = "<<p <<"\t"
#define gQStr(p)	 #p<<" = "<<p.toLocal8Bit().data()<<"\t"

#define  QFailed(x)		(x != 0)
#define  QSucceed(x)		(x == 0)

using namespace std;
using namespace filesystem;
using namespace bit7z;

namespace fs = std::filesystem;
string strUpdateCode;
string strUpdateServer;

#ifdef _DEBUG
#pragma comment(lib, "../SDK/glog/glogd")
#pragma comment(lib, "../SDK/7Z/lib/bit7z_d.lib")
#pragma comment(lib, "../SDK/libcurl/libcurld")
#else
#pragma comment(lib, "../SDK/glog/glog")
#pragma comment(lib, "../SDK/7Z/lib/bit7z.lib")
#pragma comment(lib, "../SDK/libcurl/libcurl")
#endif
#pragma comment(lib, "version")


CurlInitializer::CurlInitializer()
{
	curl_global_init(CURL_GLOBAL_ALL);
}
CurlInitializer::~CurlInitializer()
{
	curl_global_cleanup();
}

bool GetModuleVersion(string strModulePath, short& nMajorVer, short& nMinorVer, short& nBuildNum, short& nRevsion)
{
	DWORD dwHnd;
	DWORD dwVerInfoSize;
	char pszVersion[2048] = { 0 };
	bool bResult = false;
	do
	{
		if (0 >= (dwVerInfoSize = GetFileVersionInfoSizeA(strModulePath.c_str(), &dwHnd)))
		{
			break;
		}

		// get file version info
		if (!GetFileVersionInfoA(strModulePath.c_str(), dwHnd, dwVerInfoSize, pszVersion))
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
		if (!VerQueryValueA(pszVersion, "\\VarFileInfo\\Translation", (void**)&lpTranslate, &cbTranslate))
		{
			break;
		}

		// get FileVersion string from resource
		VS_FIXEDFILEINFO* p_version;
		unsigned int version_len = 0;
		if (!VerQueryValueA(pszVersion, "\\", (void**)&p_version, &version_len))
		{
			break;
		}

		nMajorVer = (p_version->dwFileVersionMS >> 16) & 0x0000FFFF;
		nMinorVer = p_version->dwFileVersionMS & 0x0000FFFF;
		nBuildNum = (p_version->dwFileVersionLS >> 16) & 0x0000FFFF;
		nRevsion = p_version->dwFileVersionLS & 0x0000FFFF;
		qDebug() << strModulePath.c_str() << "Version=" << nMajorVer << "." << nMinorVer << "." << nBuildNum << "." << nRevsion;
		bResult = true;
	} while (0);
	return bResult;
}

int RunProcess(string strPath, string strWorkDir)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	// Start the child process. 
	if (!CreateProcessA(NULL,   // No module name (use command line)
		(LPSTR)strPath.c_str(),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		strWorkDir.c_str(),           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		return GetLastError();
	}

	// Wait until child process exits.
	//WaitForSingleObject(pi.hProcess, INFINITE);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}


HttpBuffer::HttpBuffer(const char* szFileName)
{
	if (szFileName)
		fp = fopen(szFileName, "wb");
}
HttpBuffer::~HttpBuffer()
{
	if (fp)
	{
		fflush(fp);
		fclose(fp);
		fp = nullptr;
	}
}

int HttpRecv(void* buffer, size_t sz, size_t nmemb, void* ResInfo)
{
	HttpBuffer* pRespond = (HttpBuffer*)ResInfo;

	if (!pRespond)
		return sz * nmemb;
	if (buffer && sz * nmemb)
		pRespond->strBuffer.append((const char*)buffer, sz * nmemb);

	if (pRespond->fp)
	{
		fwrite(buffer, sz * nmemb, 1, pRespond->fp);
	}
	return sz * nmemb;  //返回接受数据的多少
}

// strMothod	GET ,PUT
int SendHttpRequest(string strMothod, string szUrl, string& strRespond, string& strMessage)
{
	CURLcode nCurResult = CURLE_OK; {}
	long retcode = 0;
	CURL* pCurl = nullptr;
	int nResult = -1;
	int nHttpCode = 200;
	HttpBuffer hb;
	do
	{
		pCurl = curl_easy_init();
		if (pCurl == NULL)
			break;
		nCurResult = curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, strMothod.c_str());
		curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5L);			//请求超时时长
		curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 5L);	//连接超时时长 
		curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);	//允许重定向
		//curl_easy_setopt(pCurl, CURLOPT_HEADER, 1L);			//若启用，会将头文件的信息作为数据流输出
		curl_easy_setopt(pCurl, CURLOPT_TCP_NODELAY, 0L);
		curl_easy_setopt(pCurl, CURLOPT_TCP_FASTOPEN, 1L);

		HttpBuffer* pHttpBuffer = (HttpBuffer*)&hb;
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, HttpRecv);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pHttpBuffer);

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
		strRespond = hb.strBuffer;
		nCurResult = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &nHttpCode);
		if (nHttpCode == 200)
			nResult = 0;
	}

	if (pCurl)
	{
		curl_easy_cleanup(pCurl);
		pCurl = nullptr;
	}
	return nResult;
}

int Download(string szUrl, string strFileName, string& strMessage)
{
	CURLcode nCurResult = CURLE_OK;
	long retcode = 0;
	CURL* pCurl = nullptr;
	int nResult = false;
	int nHttpCode = 200;
	HttpBuffer hb(strFileName.c_str());
	do
	{
		pCurl = curl_easy_init();
		if (pCurl == NULL)
			break;
		nCurResult = curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5L);			//请求超时时长
		curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 5L);	//连接超时时长 
		curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);	//允许重定向
		//curl_easy_setopt(pCurl, CURLOPT_HEADER, 1L);			//若启用，会将头文件的信息作为数据流输出
		curl_easy_setopt(pCurl, CURLOPT_TCP_NODELAY, 0L);
		curl_easy_setopt(pCurl, CURLOPT_TCP_FASTOPEN, 1L);

		HttpBuffer* pHttpBuffer = (HttpBuffer*)&hb;
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, HttpRecv);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pHttpBuffer);

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
		nCurResult = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &nHttpCode);
		if (nHttpCode == 200)
			nResult = 0;
	}

	if (pCurl)
	{
		curl_easy_cleanup(pCurl);
		pCurl = nullptr;
	}
	return nResult;
}

/*
//说明：供自助机检查更新
GET http://ip:port/updateservice/checkupdate?targetarea={targetarea}&version={version}&type={1}
调用参数 targetarea：区域 version：现版本
返回参数 code：0成功，其他失败 version：新版本 uploadtime：上传时间 description：描述

//下载对应版本zip
GET http://ip:port/updateservice/download?targetarea={targetarea}&version={version}&type={1}
调用参数 targetarea：区域 version：新版本
返回参数 文件流
*/

int LoadUpgradeInfo(string& strMessage)
{
	QString strAppPath = QCoreApplication::applicationDirPath();

	QString strConfigure = strAppPath + "/Configure.ini";
	QFileInfo fi(strConfigure);
	if (!fi.isFile())
	{
		strMessage = "找到不系统配置文件,系统无法启动!";
		return 1;
	}
	QSettings setting(strConfigure, QSettings::IniFormat);
	setting.beginGroup("Region");
	strUpdateCode = setting.value("UpgradeCode", "").toString().toStdString();
	strUpdateServer = setting.value("UpgradeServer", "").toString().toStdString();
	gInfo() << "UpgradeCode = " << strUpdateCode;
	gInfo() << "UpgradeServer = " << strUpdateServer;
	if (strUpdateCode.empty())
	{
		strMessage = "系统升级区域代码为空!";
		return 1;
	}
	setting.endGroup();
	return 0;
}

int  CheckNewVersion(UpdateType nType, string& strLocalVersion, string& strNewVersion, string& strMessage)
{
	QString strUrl = QString("http://%1/updateservice/checkupdate?targetarea=%2&version=%3&type=%4").arg(strUpdateServer.c_str()).arg(strUpdateCode.c_str()).arg(strLocalVersion.c_str()).arg((int)nType);

	string strRespond;
	string strErrmsg;
	if (QFailed(SendHttpRequest("GET", strUrl.toStdString(), strRespond, strErrmsg)))
	{
		strMessage = strErrmsg.c_str();
		return 1;
	}
	gInfo() << "Query Version Respond:" << strRespond;
	QJsonParseError jsError;
	auto jsdoc = QJsonDocument::fromJson(strRespond.c_str(), &jsError);
	auto jsObj = jsdoc.object();
	/*
	 {
		"code": "0",
		"version": "",
		"uploadtime": "",
		"description": ""
     }
	*/
	string strCode = jsObj.value("code").toString().toStdString();
	if (strCode == "0")
	{
		strNewVersion = jsObj.value("version").toString().toStdString();
		string strUploadTime = jsObj.value("uploadtime").toString().toStdString();
		string strDesc = jsObj.value("description").toString().toLocal8Bit().data();
		gInfo() << "Code:" << strCode << "Version:" << strNewVersion << "UploadTime" << strUploadTime << "Description" << strDesc;
		return 0;
	}
	else
		return -1;
}

/*
//下载对应版本zip
GET http://ip:port/updateservice/download?targetarea={targetarea}&version={version}
调用参数 targetarea：区域 version：新版本
返回参数 文件流
*/
int DownloadNewVerion(UpdateType nType, string& strNewVersion, string& strFilePath, string& strMessage)
{
	QString strUrl = QString("http://%1/updateservice/download?targetarea=%2&version=%3&type=%4").arg(strUpdateServer.c_str()).arg(strUpdateCode.c_str()).arg(strNewVersion.c_str()).arg((int)nType);
	// http://ip:port/updateservice/checkupdate?targetarea={targetarea}&version={version}
	strFilePath = QCoreApplication::applicationDirPath().toStdString();
	strFilePath += "/Update";
	QFileInfo fi(strFilePath.c_str());
	if (fi.exists())
	{
		if (fi.isFile())
		{
			QFile::remove(strFilePath.c_str());
			QDir dir;
			dir.mkdir(strFilePath.c_str());
		}
	}
	else
	{
		QDir dir;
		dir.mkdir(strFilePath.c_str());
	}

	if (nType == UpdateType::MainProcess)
		strFilePath += "/Patch";
	else
		strFilePath += "/Lanucher";

	strFilePath += strNewVersion + ".zip";
	QFileInfo fi2(strFilePath.c_str());
	if (fi.isFile())
		QFile::remove(strFilePath.c_str());

	if (QFailed(Download(strUrl.toStdString(), strFilePath, strMessage)))
		return 1;

	return 0;
}
/*
Index   Name                    Extension       Path                    					IsDir   Size    Packed size
0       IDCard_API.dll  		dll     		Patch1.0.2.1\IDCard_API.dll     			0       18944   10437
1       Image           						Patch1.0.2.1\Image      					1       0       0
2       BatchMode.png   		png     		Patch1.0.2.1\Image\BatchMode.png        	0       9832    9743
3       Sandong         						Patch1.0.2.1\Image\Sandong      			1       0    	0
4       newcard1.png    		png     		Patch1.0.2.1\Image\Sandong\newcard1.png 	0       26218   23939
5       KT_Printer.dll  		dll     		Patch1.0.2.1\KT_Printer.dll     			0       80384   37670
*/

wstring& replace_all(wstring& str, const wstring& old_value, const wstring& new_value)
{
	for (wstring::size_type pos(0); pos != wstring::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != wstring::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return   str;
}

int GetLocalVersion(string strProcess, string& strVersion, string& strMessage)
{
	QString strModulePath = QCoreApplication::applicationDirPath();

	strModulePath += "/";
	strModulePath += strProcess.c_str();
	QFileInfo fi(strModulePath);
	if (!fi.isFile())
		return -1;
	short nMajorVer, nMinorVer, nBuildNum, nRevsion;
	if (!GetModuleVersion(strModulePath.toStdString(), nMajorVer, nMinorVer, nBuildNum, nRevsion))
		return -1;

	strVersion = QString("%1.%2.%3.%4").arg(nMajorVer).arg(nMinorVer).arg(nBuildNum).arg(nRevsion).toStdString();
	return 0;
}

void BackupOldVersion(UpdateType nType, string strFileZipPath)
{
	try
	{
		QString strAppPath = QCoreApplication::applicationDirPath();

		string strLocalVersion, strMessage;
		string strProcess = "/SSCard_System.exe";
		if (nType == UpdateType::Launcher)
			strProcess = "/Launcher.exe";

		if (QFailed(GetLocalVersion(strProcess, strLocalVersion, strMessage)))
		{
			gError() << "Failed in GetLocalVersion.";
			return;
		}
		QString strBackupPath = strAppPath + "/Backup";

		QString strBackupVersionPath = strBackupPath + "/" + "Patch" + strLocalVersion.c_str();
		if (nType == UpdateType::Launcher)
			strBackupVersionPath = strBackupPath + "/" + "Launcher" + strLocalVersion.c_str();

		vector<string> vecDir = { strBackupPath.toStdString() };

		QFileInfo fidir(strBackupPath);
		if (!fidir.isDir())
		{
			gInfo() << "Directory " << gQStr(strBackupPath) << "not exist,try to create...!";
			QDir dir;
			if (!dir.mkpath(strBackupPath))
			{
				gInfo() << "************Failed in creating directory '" << gQStr(strBackupPath) << "',may cause some exception!************";
			}
		}

		Bit7zLibrary lib{ L"7z.dll" };
		QString strFilePathW = strFileZipPath.c_str();
		BitArchiveInfo arc{ lib, strFilePathW.toStdWString(), BitFormat::Zip };

		//printing archive metadata
		//wcout << L"Archive properties" << endl;
		//wcout << L" Items count: " << arc.itemsCount() << endl;
		//wcout << L" Folders count: " << arc.foldersCount() << endl;
		//wcout << L" Files count: " << arc.filesCount() << endl;
		//wcout << L" Size: " << arc.size() << endl;
		//wcout << L" Packed size: " << arc.packSize() << endl;
		//wcout << endl;

		auto arc_items = arc.items();
		//wcout << L"Index\t" << L"Name\t\t\t" << L"Extension\t" << L"Path\t\t\t" << L"IsDir\t" << L"Size\t" << L"Packed size" << endl;
		vector<wstring> vecFiles;
		for (auto& item : arc_items)
		{
			wstring strPath = item.path();
			wstring strOldFilePath = strAppPath.toStdWString() + L"/" + strPath;
			if (fs::exists(strOldFilePath))
				vecFiles.push_back(strOldFilePath);
			/*wstring strBackFilePath = strBackupVersionPath.toStdWString() + L"/" + strPath;
			try
			{
				if (fs::exists(strBackFilePath))
					fs::remove(strBackFilePath);
				fs::copy(strOldFilePath, strBackFilePath);
			}
			catch (std::exception& e)
			{
				gError() << "Catch exception:" << e.what();
			}*/
			//	wcout << item.index() << "\t"
			//		<< item.name() << "\t"
			//		<< item.extension() << "\t"
			//		<< item.path() << "\t"
			//		<< item.isDir() << "\t"
			//		<< item.size() << "\t"
			//		<< item.packSize() << endl;
		}
		strBackupVersionPath += ".zip";
		if (fs::exists(strBackupVersionPath.toStdString()))
			fs::remove(strBackupVersionPath.toStdString());

		QString str7ZLib = QCoreApplication::applicationDirPath() + "/7z.dll";
		Bit7zLibrary lib7z{ str7ZLib.toStdWString() };
		BitCompressor compressor{ lib7z, BitFormat::Zip };
		compressor.compress(vecFiles, strBackupVersionPath.toStdWString());
	}
	catch (const BitException& ex)
	{
		gInfo() << "Catch a exception :" << ex.what();
	}
}

void ClearDirectory(string strPath)
{
	QDir dir(strPath.c_str());
	if (!dir.exists())
		return;

	QStringList filters = { "*.*" };
	QDirIterator dir_iterator(strPath.c_str(), filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	while (dir_iterator.hasNext())
	{
		dir_iterator.next();
		QFileInfo file_info = dir_iterator.fileInfo();
		QString absolute_file_path = file_info.absoluteFilePath();
#ifdef _DEBUG
		cout << "remove file " << absolute_file_path.toStdString();
#endif
		//QFile::remove(absolute_file_path);
	}
}

int InstallNewVersion(string& strFilePath, string& strMessage)
{
	try
	{
		QString str7ZLib = QCoreApplication::applicationDirPath() + "/7z.dll";
		QString strExtractPath = QCoreApplication::applicationDirPath();// +"/Update/Temp";
		ClearDirectory(strExtractPath.toStdString());
		Bit7zLibrary lib7z{ str7ZLib.toStdWString() };
		BitExtractor extractor{ lib7z, BitFormat::Zip };
		QString strFilePathW = strFilePath.c_str();

		extractor.extract(strFilePathW.toStdWString(), strExtractPath.toStdWString()); //extracting a simple archive

		//extractor.extractMatching(L"path/to/arc.7z", L"file.pdf", L"out/dir/"); //extracting a specific file
		//extracting the first file of an archive to a buffer
		//std::vector< byte_t > buffer;
		//extractor.extract(L"path/to/archive.7z", buffer);

		//extracting an encrypted archive
		//extractor.setPassword(L"password");
		//extractor.extract(L"path/to/another/archive.7z", L"out/dir/");
		return 0;
	}
	catch (const BitException& ex)
	{
		gInfo() << "Catch a exception :" << ex.what();
		return 1;
	}
}
