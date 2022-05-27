#pragma once

#include "update_global.h"
#include <string>

using namespace std;

enum class UpdateType
{
	MainProcess,
	Launcher,
};

class CurlInitializer
{
public:
	CurlInitializer();
	~CurlInitializer();
};

struct HttpBuffer
{
	FILE* fp = nullptr;
	string strBuffer;
	HttpBuffer(const char* szFileName = nullptr);
	~HttpBuffer();
};
bool  GetModuleVersion(string strModulePath, short& nMajorVer, short& nMinorVer, short& nBuildNum, short& nRevsion);
int	  RunProcess(string strPath, string strWorkDir);
int	  SendHttpRequest(string strMothod, string szUrl, string& strRespond, string& strMessage);
int	  Download(string szUrl, string strFileName, string& strMessage);
int	  LoadUpgradeInfo(string& strMessage);
int	  CheckNewVersion(UpdateType nType, string& strLocalVersion, string& strNewVersion, string& strMD5, string& strMessage);
int	  DownloadNewVerion(UpdateType nType, string& strNewVersion, string& strFilePath, string& strMessage);
int	  GetLocalVersion(string strProcess, string& strVersion, string& strMessage);
void  BackupOldVersion(UpdateType nType, string strFileZipPath);
void  ClearDirectory(string strPath);
int	  InstallNewVersion(string& strFilePath, string& strMessage);
int	  MD5(const string& strFileName, string& strMD5);
bool  Update(UpdateType nType, string& strMessage);
