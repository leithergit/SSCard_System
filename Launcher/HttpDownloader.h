#pragma once
#ifndef __Downloader_LibCurl_H__
#define __Downloader_LibCurl_H__

#pragma once
#include "../SDK/libcurl/curl.h"
#include <string>
#include <list>
#include <memory>
using namespace std;

struct RequestInfo
{
	string strUrl;
	string strFileName;
};

struct DownloadInfo :public RequestInfo
{
	long		nLocalLen;		//本地已下载的长度（大小）
	LONGLONG	nTotalLen;		//文件总长度（大小）
	long		CurDownloadLen;		//每次下载的文件长度(大小)
}CURDI;

using DownloadInfoPtr = shared_ptr<DownloadInfo>;

class HttpDownloader
{
public:
	HttpDownloader(void);
	~HttpDownloader(void);
	int StartDownloadThread();
	double GetTotalFileLenth(const char* url);             //获取将要下载的文件长度
	long GetLocalFileLenth(const char* fileName);         //获取本地问价长度
	void GetFileNameFormUrl(char* fileName, const char* url);      //从URL中获取文件名
	void AddDownloadWork(DLIO downloadWork);
	int SetConnectTimeOut(DWORD nConnectTimeOut);    //设置连接的超时时间
	int GetCurrentDownloadInfo(CURDI* lpCurDownloadInfor);
	BOOL CreateMultiDir(const char* pathName);       //是否在本地创建目录，没有就创建
	BOOL IsDownloadBegin();
	BOOL IsDownloadEnd();
protected:
	static DWORD WINAPI SingleDownloadProc(LPVOID lpParameter);       //线程函数
	static size_t WriteFunc(char* str, size_t size, size_t nmemb, void* stream);     //写入数据（回调函数）
	static size_t ProgressFunc(double* fileLen, double t, double d, double ultotal, double ulnow);   //下载进度
private:
	char m_downloadUrl[256];
	int m_downloadCourse;   //-1 还未下载 0正在下载 1下载完成
	long m_curLocalFileLenth; //因为下载的时候已经计算了本地文件的大小用来设置断点，所以对于每个文件，该数字只会被设置一次；就是下载前的本地大小；
	long m_nConnectTimeOut;      //连接的超时时间
	list<DownloadInfoPtr> listTask;

	CURL* m_pCurl;
};



#endif
