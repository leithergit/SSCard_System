#pragma execution_character_set("utf-8")
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QString>
#include <QMessageBox>
#include <QFileInfo>
#include <QProcess>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <Windows.h>
#include <process.h>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <QCryptographicHash>
#include "../SDK/libcurl/curl.h"
#include "../SDK/glog/logging.h"
#include "../Update/Update.h"

#define gInfo()      LOG(INFO)
#define gError()     LOG(ERROR)
#define gWarning()   LOG(WARNING)
#define Str(x)       #x
#define gVal(p)      #p<<" = "<<p <<"\t"
#define gQStr(p)	 #p<<" = "<<p.toLocal8Bit().data()<<"\t"
#define GBKString(x) QString(x).toLocal8Bit().data()

#define  QFailed(x)		(x != 0)
#define  QSucceed(x)		(x == 0)

using namespace std;
using namespace filesystem;
namespace fs = std::filesystem;

#ifdef _DEBUG
#pragma comment(lib, "../SDK/glog/glogd")
#pragma comment(lib, "../SDK/libcurl/libcurld")
#pragma comment(lib,"../SSCard_System/Debug/Update")
#else
#pragma comment(lib, "../SDK/glog/glog")
#pragma comment(lib, "../SDK/libcurl/libcurl")
#pragma comment(lib,"../SSCard_System/release/Update")
#endif
extern std::string strMainProcess;
extern int nRetryCount;

const wchar_t* g_pUniqueID = L"Global\\2F026B66-2CCA-40AB-AD72-435B5AC2E625";
HANDLE g_hAppMutex = nullptr;

int main(int argc, char* argv[])
{
	if (OpenMutexW(MUTEX_ALL_ACCESS, FALSE, g_pUniqueID))
		return 0;
	g_hAppMutex = CreateMutexW(nullptr, TRUE, g_pUniqueID);
	shared_ptr<void> FreeMutex(g_hAppMutex, CloseHandle);

	CurlInitializer curlInit;
	QCoreApplication a(argc, argv);

	QString strLogPath = QCoreApplication::applicationDirPath();
	QDir::setCurrent(strLogPath);
	qDebug() << QDir::currentPath();
	strLogPath += "/log";
	QDateTime curDate = QDateTime::currentDateTime();
	QString strLogDate = curDate.toString("yyyy_MM_dd");
	QString strLogDatePath = QString("%1/%2/").arg(strLogPath).arg(strLogDate);
	vector<QString> vecDir = { strLogPath,strLogDatePath };
	for (auto var : vecDir)
	{
		QFileInfo fidir(var);
		if (!fidir.isDir())
		{
			gInfo() << "Directory " << var.toStdString() << "not exist,try to create...!";
			QDir dir;
			if (!dir.mkpath(var))
			{
				gInfo() << "************Failed in creating directory '" << var.toStdString() << "',may cause some exception!************";
			}
		}
	}
	FLAGS_max_log_size = 256;
	google::SetLogDestination(google::GLOG_INFO, strLogDatePath.toLocal8Bit().data());	// 使用该设定时，默认情况下，所有级别日志都用同一的文件名
	google::SetStderrLogging(google::GLOG_INFO);	// 大于该级别的日志输出到stderr
	google::SetLogFilenameExtension(".log");
	google::InitGoogleLogging(strLogDatePath.toLocal8Bit().data());
	string strMessage, strLocalVersion, strNewVersion, strFilePath, strMD5_Query, strMD5_Download;
	bool bSucceed = false;
	QString strAppPath = QCoreApplication::applicationDirPath();

	/*
	do
	{
		bSucceed = false;
		if (QFailed(LoadUpgradeInfo(strMessage)))
			break;

		//string strProcess = strAppPath.toStdString() + "/" + strMainProcess;
		//if (QFailed(MD5(strProcess, strMD5)))
		//{
		//	gError() << "Failed in MD5 string for file :" << strProcess;
		//	break;
		//}

		//if (strMainProcess.find(strMD5) != string::npos)
		//{
		//	gError() << "The MD5 string is mismatch withe the file " << strProcess;
		//	break;
		//}

		if (QFailed(GetLocalVersion(strMainProcess, strLocalVersion, strMessage)))
		{
			gError() << "Failed in GetLocalVersion:" << strMessage;
			break;
		}
		gInfo() << strMainProcess << " Local Version" << strLocalVersion;
		if (QFailed(CheckNewVersion(UpdateType::MainProcess, strLocalVersion, strNewVersion, strMD5_Query, strMessage)))
		{
			gError() << "Failed in CheckNewVersion" << strMessage;
			break;
		}
		bool bDownloadSucceed = false;
		int nTryCount = 0;
		do
		{
			gInfo() << strMainProcess << " Remote Version" << strNewVersion << "\tMD5:" << strMD5_Query;
			if (QFailed(DownloadNewVerion(UpdateType::MainProcess, strNewVersion, strFilePath, strMessage)))
			{
				gError() << "Failed in DownloadNewVerion" << strMessage;
				break;
			}

			if (QFailed(MD5(strFilePath, strMD5_Download)))
			{
				gError() << "Failed in getgtin MD5 string of file :" << strFilePath;
				break;
			}
			std::transform(strMD5_Query.begin(), strMD5_Query.end(), strMD5_Query.begin(), ::toupper);
			std::transform(strMD5_Download.begin(), strMD5_Download.end(), strMD5_Download.begin(), ::toupper);
			if (strMD5_Query == strMD5_Download)
			{
				bDownloadSucceed = true;
				break;
			}
			nTryCount++;
			gError() << "Failed in MD5 string for file :" << strFilePath;

		} while (nTryCount < nRetryCount);
		if (!bDownloadSucceed)
		{
			gError() << "Failed in downloading new version:" << strNewVersion;
			break;
		}

		gInfo() << "Try to BackupOldVersion";
		BackupOldVersion(UpdateType::MainProcess, strFilePath);
		if (QFailed(InstallNewVersion(strFilePath, strMessage)))
		{
			gError() << "Failed in InstallNewVersion" << strMessage;
			break;
		}

		bSucceed = true;
	} while (true);
	if (!bSucceed)
		gInfo() << strMessage;
	*/

	if (!Update(UpdateType::MainProcess, strMessage))
	{
		gError() << "Failed in update MainProcess:" << strMessage;
	}
	strMainProcess = strAppPath.toStdString() + "/" + strMainProcess;

	RunProcess(strMainProcess.c_str(), strAppPath.toStdString().c_str());
	return 0;
}
