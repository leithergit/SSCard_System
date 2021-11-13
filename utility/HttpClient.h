#pragma once
#include <string>
#include <WINDOWS.H>
#include <winhttp.h>
#include <vector>

typedef struct _StParam_{
    std::wstring wstrKey;
    std::wstring wstrValue;
}StParam,*PStParam;

typedef std::vector<StParam> VecStParam;
typedef VecStParam::iterator VecStParamIter;
typedef VecStParam::const_iterator VecStParamCIter;

enum EType{
    eGet,
    ePost,
    eUpload,
};

class CHttpClient
{
public:
    CHttpClient();
    ~CHttpClient(void);

public:
    BOOL TransmiteData(const std::wstring& wstrUrl, EType eType, DWORD dwTimeout,WCHAR *szAccount = NULL,WCHAR *szPassword = NULL);

    VOID ClearEvn();

    BOOL ReceiveData(LPBYTE lpBuffer, DWORD& dwBufferSize);
	void SetPayload(const std::wstring strPayload)
	{
		m_wstrPayload = strPayload;
	}
private:
    BOOL InitializeHttp( const std::wstring& wstrUrl, DWORD dwTimeout, EType eType,WCHAR *szAccount = NULL,WCHAR *szPassword = NULL);
    VOID UninitializeHttp();

    BOOL ReceiveData();

    BOOL TransmiteData(EType eType);

    VOID ParseParams(const std::wstring& wstrExtraInfo);

    BOOL TransmiteDataToServerByGet();
    BOOL TransmiteDataToServerByPost();
    BOOL TransmiteDataToServerByUpload();
	INT CheckResponStatus();

private: 
    virtual DWORD GetDataSize() = 0;
    virtual BOOL GetData(LPVOID lpBuffer, DWORD dwBufferSize, DWORD& dwWrite);
    virtual BOOL ModifyRequestHeader(HINTERNET hRequest) = 0;
    virtual VOID AddExtInfo(VecStParam& VecExtInfo) = 0;
    virtual std::wstring GenerateExtInfo(const VecStParam& VecExtInfo) = 0;
private: 
    HINTERNET m_hSession;
    HINTERNET m_hConnect;
    HINTERNET m_hRequest;
	std::wstring  m_strAccount;
	std::wstring  m_strPassword;
    DWORD m_dwTimeout;
    LPBYTE m_lpReceiveData;
    DWORD m_dwReceiveDataLength;
    std::wstring m_wstrUrlPath;
    VecStParam m_VecExtInfo;
	std::wstring m_wstrPayload;
};

