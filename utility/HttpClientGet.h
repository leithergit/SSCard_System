#pragma once
#include "HttpClient.h"

class CHttpClientGet:public CHttpClient
{
public:
    CHttpClientGet(void);
    ~CHttpClientGet(void);
public:
    BOOL TransDataToServer(const std::wstring& wstrUrl, DWORD dwTimeout, 
		VecStParam& vecParam, LPVOID lpData = NULL, DWORD dwDataLenInBytes = 0);
	
private:
    DWORD GetDataSize();
    BOOL GetData(LPVOID lpBuffer, DWORD dwBufferSize, DWORD& dwWrite);
    std::wstring GenerateExtInfo(const VecStParam& VecExtInfo);
    virtual BOOL ModifyRequestHeader(HINTERNET hRequest);
    VOID AddExtInfo(VecStParam& VecExtInfo);
	
private:
    LPVOID m_lpData;
    DWORD m_dwDataSize;
    DWORD m_dwWriteIndex;
    VecStParam m_vecParam;
};

class CHttpRestClient:public CHttpClientGet
{
public:
	CHttpRestClient()
	{
	};
	~CHttpRestClient()
	{
 
	};

	virtual BOOL ModifyRequestHeader(HINTERNET hRequest)
	{
		std::wstring wstrHeader[] = {L"Origin: chrome-extension://bljmokabgbdkoefbmccaeficehkmlnao\r\n",
		L"User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36\r\n",
		L"Content-Type: application/x-www-form-urlencoded\r\n",
		L"Accept: */*\r\n",
		L"DNT: 1\r\n",
		L" Accept-Encoding: gzip, deflate\r\n",
		L"Accept-Language: zh-CN,zh;q=0.8\r\n"};
		for ( size_t i = 0; i < ARRAYSIZE(wstrHeader); i++ ) {
			WinHttpAddRequestHeaders(hRequest, wstrHeader[i].c_str(), wstrHeader[i].length(), WINHTTP_ADDREQ_FLAG_ADD);
		}
		return TRUE;

	}

};