#include "HttpClient.h"
#include <atlbase.h>
#include <atlconv.h>

#pragma comment(lib,"Winhttp.lib")

#define MAXSTATICPARAMCOUNT 32

CHttpClient::CHttpClient()
{
    m_hSession = NULL;
    m_hConnect = NULL;
    m_hRequest = NULL;
    m_dwTimeout = 0;
    m_lpReceiveData = NULL;
    m_dwReceiveDataLength = 0;
}


CHttpClient::~CHttpClient(void)
{
    ClearEvn();
}

BOOL CHttpClient::InitializeHttp( const std::wstring& wstrUrl, DWORD dwTimeout, EType eType,WCHAR *szAccount ,WCHAR *szPassword )
{
    BOOL bSuc = FALSE;
    do {
        URL_COMPONENTS urlCom;
        memset(&urlCom, 0, sizeof(urlCom));
        urlCom.dwStructSize = sizeof(urlCom);
        WCHAR wchScheme[64] = {0};
        urlCom.lpszScheme = wchScheme;
        urlCom.dwSchemeLength = ARRAYSIZE(wchScheme);
        WCHAR wchHostName[1024] = {0};
        urlCom.lpszHostName = wchHostName;
        urlCom.dwHostNameLength = ARRAYSIZE(wchHostName);
        WCHAR wchUrlPath[1024] = {0};
        urlCom.lpszUrlPath = wchUrlPath;
        urlCom.dwUrlPathLength = ARRAYSIZE(wchUrlPath);
        WCHAR wchExtraInfo[1024] = {0};
        urlCom.lpszExtraInfo = wchExtraInfo;
        urlCom.dwExtraInfoLength = ARRAYSIZE(wchExtraInfo);

        if ( FALSE == WinHttpCrackUrl( wstrUrl.c_str(), wstrUrl.length(), ICU_ESCAPE, &urlCom) ) {
            break;
        }

		if (szAccount && wcslen(szAccount))
			m_strAccount = szAccount;
		if (szPassword && wcslen(szPassword))
			m_strPassword = szPassword;

        std::wstring wstrExtraInfo;
		if (eGet == eType) 
		{
			wstrExtraInfo = urlCom.lpszExtraInfo;
			ParseParams(wstrExtraInfo);
			AddExtInfo(m_VecExtInfo);
		}

        m_hSession = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0 ); 
        if ( NULL == m_hSession ) {
            break;
        }

        if ( FALSE == WinHttpSetTimeouts(m_hSession, dwTimeout, dwTimeout, dwTimeout, dwTimeout) ) {
            break;
        }

        m_hConnect = WinHttpConnect( m_hSession, urlCom.lpszHostName, urlCom.nPort, 0 );
        if ( NULL == m_hConnect ) {
            break;
        }

		m_wstrUrlPath = urlCom.lpszUrlPath;
		if (eGet != eType) {
			m_wstrUrlPath += urlCom.lpszExtraInfo;
		}

        bSuc = TRUE;
    } while (0);
    return bSuc;
}

VOID CHttpClient::UninitializeHttp()
{
    if ( NULL != m_hRequest ) {
        WinHttpCloseHandle(m_hRequest);
        m_hRequest = NULL;
    }

    if ( NULL != m_hConnect ) {
        WinHttpCloseHandle(m_hConnect);
        m_hConnect = NULL;
    }

    if ( NULL != m_hSession ) {
        WinHttpCloseHandle(m_hSession);
        m_hSession = NULL;
    }
}
DWORD ChooseAuthScheme( DWORD dwSupportedSchemes )
{
	//  It is the server's responsibility only to accept 
	//  authentication schemes that provide a sufficient
	//  level of security to protect the servers resources.
	//
	//  The client is also obligated only to use an authentication
	//  scheme that adequately protects its username and password.
	//
	//  Thus, this sample code does not use Basic authentication  
	//  becaus Basic authentication exposes the client's username
	//  and password to anyone monitoring the connection.

	if( dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NEGOTIATE )
		return WINHTTP_AUTH_SCHEME_NEGOTIATE;
	else if( dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NTLM )
		return WINHTTP_AUTH_SCHEME_NTLM;
	else if( dwSupportedSchemes & WINHTTP_AUTH_SCHEME_PASSPORT )
		return WINHTTP_AUTH_SCHEME_PASSPORT;
	else if( dwSupportedSchemes & WINHTTP_AUTH_SCHEME_DIGEST )
		return WINHTTP_AUTH_SCHEME_DIGEST;
	else
		return 0;
}


// 
// ERROR_WINHTTP_CANNOT_CONNECT
// Returned if connection to the server failed.
// 
// ERROR_WINHTTP_CHUNKED_ENCODING_HEADER_SIZE_OVERFLOW
// Returned when an overflow condition is encountered in the course of parsing chunked encoding.
// 
// ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED
// Returned when the server requests client authentication.
// 
// ERROR_WINHTTP_CONNECTION_ERROR
// The connection with the server has been reset or terminated, or an incompatible SSL protocol was encountered. For example, WinHTTP version 5.1 does not support SSL2 unless the client specifically enables it.
// 
// ERROR_WINHTTP_HEADER_COUNT_EXCEEDED
// Returned when a larger number of headers were present in a response than WinHTTP could receive.
// 
// ERROR_WINHTTP_HEADER_SIZE_OVERFLOW
// Returned by WinHttpReceiveResponse when the size of headers received exceeds the limit for the request handle.
// 
// ERROR_WINHTTP_INCORRECT_HANDLE_STATE		12019
// The requested operation cannot be carried out because the handle supplied is not in the correct state.
// 
// ERROR_WINHTTP_INCORRECT_HANDLE_TYPE
// The type of handle supplied is incorrect for this operation.
// 
// ERROR_WINHTTP_INTERNAL_ERROR
// An internal error has occurred.
// 
// ERROR_WINHTTP_INVALID_SERVER_RESPONSE
// The server response could not be parsed.
// 
// ERROR_WINHTTP_INVALID_URL
// The URL is invalid.
// 
// ERROR_WINHTTP_LOGIN_FAILURE
// The login attempt failed. When this error is encountered, the request handle should be closed with WinHttpCloseHandle. A new request handle must be created before retrying the function that originally produced this error.
// 
// ERROR_WINHTTP_NAME_NOT_RESOLVED
// The server name could not be resolved.
// 
// ERROR_WINHTTP_OPERATION_CANCELLED
// The operation was canceled, usually because the handle on which the request was operating was closed before the operation completed.
// 
// ERROR_WINHTTP_REDIRECT_FAILED
// The redirection failed because either the scheme changed or all attempts made to redirect failed (default is five attempts).
// 
// ERROR_WINHTTP_RESEND_REQUEST
// The WinHTTP function failed. The desired function can be retried on the same request handle.
// 
// ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW
// Returned when an incoming response exceeds an internal WinHTTP size limit.
// 
// ERROR_WINHTTP_SECURE_FAILURE
// One or more errors were found in the Secure Sockets Layer (SSL) certificate sent by the server. To determine what type of error was encountered, check for a WINHTTP_CALLBACK_STATUS_SECURE_FAILURE notification in a status callback function. For more information, see WINHTTP_STATUS_CALLBACK.
// 
// ERROR_WINHTTP_TIMEOUT
// The request has timed out.
// 
// ERROR_WINHTTP_UNRECOGNIZED_SCHEME
// The URL specified a scheme other than "http:" or "https:".
// 
// ERROR_NOT_ENOUGH_MEMORY


BOOL CHttpClient::ReceiveData()
{
    BOOL bSuc = FALSE;
    DWORD dwReceivedBufferLength = 0;
    LPBYTE lpReceivedBuffer = NULL;
	BOOL bResults = FALSE ;
	BOOL bReSendRequest = FALSE;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwStatusCode = 0 ;
	DWORD dwSupportedSchemes ;
	DWORD dwFirstScheme ;
	DWORD dwTarget;
	DWORD dwSelectedScheme;
	DWORD dwLastStatus = 0;
	DWORD dwProxyAuthScheme = 0 ;
	BOOL  bDone = FALSE;

    while(!bDone)
	{
		if (bReSendRequest)	// Send a request.
		{
			bResults = WinHttpSendRequest( m_hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0, 0, 0 );
			if (!bResults)
				break;
			bReSendRequest = FALSE;
		}
		
		bResults =  WinHttpReceiveResponse( m_hRequest, NULL);
		if( !bResults && GetLastError() == ERROR_WINHTTP_RESEND_REQUEST)
		{
			bReSendRequest = TRUE;
			continue;
		}
		
		if (bResults)		// Check the status code.
			bResults = WinHttpQueryHeaders( m_hRequest, WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,NULL,&dwStatusCode,	&dwSize, NULL );
		if (bResults)
		{
			switch( dwStatusCode )
			{
			case HTTP_STATUS_OK: 
				// The resource was successfully retrieved.
				// You can use WinHttpReadData to read the 
				// contents of the server's response.
				// The resource was successfully retrieved.
				bDone = TRUE;
				break;
			case HTTP_STATUS_DENIED:
				// The server requires authentication.
				// The server requires authentication. Sending credentials

				// Obtain the supported and preferred schemes.
				bResults = WinHttpQueryAuthSchemes( m_hRequest, 
					&dwSupportedSchemes, 
					&dwFirstScheme, 
					&dwTarget );

				// Set the credentials before resending the request.
				if( bResults )
				{
					dwSelectedScheme = ChooseAuthScheme( dwSupportedSchemes);

					if( dwSelectedScheme == 0 )
						bDone = TRUE;
					else
						bResults = WinHttpSetCredentials( m_hRequest, 
						dwTarget, 
						dwSelectedScheme,
						m_strAccount.c_str(),
						m_strPassword.c_str(),
						NULL );
				}

				// If the same credentials are requested twice, abort the
				// request.  For simplicity, this sample does not check
				// for a repeated sequence of status codes.
				if( dwLastStatus == HTTP_STATUS_DENIED )
					bDone = TRUE;
				
				bReSendRequest = TRUE;
				break;

			case HTTP_STATUS_PROXY_AUTH_REQ:
				// The proxy requires authentication.
				printf( "The proxy requires authentication.  Sending credentials...\n" );

				// Obtain the supported and preferred schemes.
				bResults = WinHttpQueryAuthSchemes( m_hRequest, 
					&dwSupportedSchemes, 
					&dwFirstScheme, 
					&dwTarget );

				// Set the credentials before resending the request.
				if( bResults )
					dwProxyAuthScheme = ChooseAuthScheme(dwSupportedSchemes);

				// If the same credentials are requested twice, abort the
				// request.  For simplicity, this sample does not check 
				// for a repeated sequence of status codes.
				if( dwLastStatus == HTTP_STATUS_PROXY_AUTH_REQ )
					bDone = TRUE;
					
				bReSendRequest = TRUE;
				break;

			default:
				// The status code does not indicate success.
				printf("Error. Status code %d returned.\n", dwStatusCode);
				bDone = TRUE;
			}
			
		}
		// Keep track of the last status code.
		dwLastStatus = dwStatusCode;

		// If there are any errors, break out of the loop.
		if( !bResults ) 
			bDone = TRUE;
	}
	// Report any errors.
	if( !bResults )
	{
		DWORD dwLastError = GetLastError( );		
		return FALSE;
	}
	
    DWORD dwRetLength = 0;
    do {
        bSuc = FALSE;
        if ( FALSE == WinHttpQueryDataAvailable(m_hRequest, &dwRetLength ) ) {
            break;
        }

        if ( 0 == dwRetLength ) {
            bSuc = TRUE;
            break;
        }

        LPBYTE lpReceivedData = new BYTE[dwRetLength];
        if ( NULL == lpReceivedData ) {
            break;
        }
        memset(lpReceivedData, 0, dwRetLength);

        DWORD dwRead = 0;
        if ( FALSE == WinHttpReadData(m_hRequest, lpReceivedData, dwRetLength, &dwRead) ) {
            break;
        }

        if ( 0 == dwRead ) {
            break;
        }

        DWORD dwReceivedBufferLengthNew = dwReceivedBufferLength + dwRetLength;
        LPBYTE lpReceivedBufferNew = new BYTE[dwReceivedBufferLengthNew];
        if ( NULL == lpReceivedBufferNew ) {
            break;
        }
        memset(lpReceivedBufferNew, 0, dwReceivedBufferLengthNew);

        if ( NULL != lpReceivedBuffer ) {
            memcpy_s( lpReceivedBufferNew, dwReceivedBufferLengthNew, lpReceivedBuffer, dwReceivedBufferLength);
            delete [] lpReceivedBuffer;
            lpReceivedBuffer = NULL;
        }
        if ( NULL != lpReceivedData ) {
            memcpy_s( lpReceivedBufferNew + dwReceivedBufferLength, 
                dwReceivedBufferLengthNew - dwReceivedBufferLength, lpReceivedData, dwRetLength);
            delete [] lpReceivedData;
            lpReceivedData = NULL;
        }
        lpReceivedBuffer = lpReceivedBufferNew;
        dwReceivedBufferLength = dwReceivedBufferLengthNew;

        bSuc = TRUE;
    } while ( dwRetLength > 0);


    if ( bSuc ) {
        m_lpReceiveData = lpReceivedBuffer;
        m_dwReceiveDataLength = dwReceivedBufferLength;
    }

    return bSuc;
}

BOOL CHttpClient::ReceiveData( LPBYTE lpBuffer, DWORD& dwBufferSize )
{
    BOOL bSuc = FALSE;
    do {
        if ( NULL == m_lpReceiveData ) {
            ::SetLastError(ERROR_WINHTTP_NOT_INITIALIZED);
            break;
        }
        if ( NULL == lpBuffer ) {
            ::SetLastError(ERROR_INVALID_ADDRESS);
            break;
        }

        if ( m_dwReceiveDataLength > dwBufferSize ) {
            dwBufferSize = m_dwReceiveDataLength;
            ::SetLastError(ERROR_INSUFFICIENT_BUFFER);
            break;
        }
        
        errno_t e =  memcpy_s( lpBuffer, dwBufferSize, m_lpReceiveData, m_dwReceiveDataLength);
        if ( 0 != e ) {
            break;
        }

        ClearEvn();

        bSuc = TRUE;
    } while (0);
    return bSuc;
}

VOID CHttpClient::ClearEvn()
{
    UninitializeHttp();
    m_dwTimeout = 0;
    if ( NULL != m_lpReceiveData ) {
        delete [] m_lpReceiveData;
        m_lpReceiveData = NULL;
    }
    m_dwReceiveDataLength = 0;
}

BOOL CHttpClient::TransmiteData( const std::wstring& wstrUrl, EType eType, DWORD dwTimeout,WCHAR *szAccount ,WCHAR *szPassword )
{
    BOOL bSuc = FALSE;
    do {
        if ( FALSE == InitializeHttp(wstrUrl, dwTimeout, eType,szAccount,szPassword)) {
            break;
        }
        if ( FALSE == TransmiteData(eType) ) {
            break;
        }
        ReceiveData();
        UninitializeHttp();
        bSuc = TRUE;
    } while (0);
    return bSuc;
}

BOOL CHttpClient::TransmiteData(EType eType)
{
    BOOL bSuc = FALSE;
    switch (eType) {
    case eGet:{
        bSuc = TransmiteDataToServerByGet();
        }break;
    case ePost:{
        bSuc = TransmiteDataToServerByPost();
        }break;
    case eUpload:{
        bSuc = TransmiteDataToServerByUpload();
        }break;
    default: break;
    }
    return bSuc;
}

BOOL CHttpClient::GetData( LPVOID lpBuffer, DWORD dwBufferSize, DWORD& dwWrite )
{
    return FALSE;
}

BOOL CHttpClient::ModifyRequestHeader( HINTERNET hRequest )
{
    return TRUE;
}

VOID CHttpClient::ParseParams(const std::wstring& wstrExtraInfo)
{
    int nPos = 0;
    nPos = wstrExtraInfo.find('?');
    if ( -1 == nPos ) {
        return;
    }
    std::wstring wstrParam = wstrExtraInfo;
    int nStaticMaxParamCount = MAXSTATICPARAMCOUNT;
    do{
        wstrParam = wstrParam.substr(nPos + 1, wstrExtraInfo.length() - nPos - 1);
        nPos = wstrParam.find('&', nPos);
        std::wstring wstrKeyValuePair;

        if ( -1 == nPos ) {
            wstrKeyValuePair = wstrParam;
        }
        else {
            wstrKeyValuePair = wstrParam.substr(0, nPos);
        }
        
        int nSp = wstrKeyValuePair.find('=');
        if ( -1 != nSp ) {
            StParam stParam;
            stParam.wstrKey = wstrKeyValuePair.substr(0, nSp);
            stParam.wstrValue = wstrKeyValuePair.substr( nSp + 1, wstrKeyValuePair.length() - nSp - 1);
            m_VecExtInfo.push_back(stParam);
        }
    }while(-1 != nPos && nStaticMaxParamCount > 0);
}


INT CHttpClient::CheckResponStatus()
{
	DWORD dwStatusCode = 0;
	DWORD dwSupportedSchemes;
	DWORD dwFirstScheme;
	DWORD dwSelectedScheme;
	DWORD dwTarget;
	DWORD dwLastStatus = 0;
	DWORD dwSize = sizeof(DWORD);
	BOOL  bResults = FALSE;
	BOOL  bDone = FALSE;
	BOOL  bAuthSucceed = FALSE;

	DWORD dwProxyAuthScheme = 0;
	// Continue to send a request until status code 
	// is not 401 or 407.
	if( m_hRequest == NULL )
		bDone = TRUE;

	while( !bDone )
	{
		// Send a request.
		bResults = WinHttpSendRequest( m_hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0,
			WINHTTP_NO_REQUEST_DATA,
			0, 
			0, 
			0 );

		// End the request.
		if( bResults )
			bResults = WinHttpReceiveResponse( m_hRequest, NULL );

		// Resend the request in case of 
		// ERROR_WINHTTP_RESEND_REQUEST error.
		if( !bResults && GetLastError() == ERROR_WINHTTP_RESEND_REQUEST)
			continue;

		// Check the status code.
		if( bResults ) 
			bResults = WinHttpQueryHeaders( m_hRequest, WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,NULL,&dwStatusCode,	&dwSize, NULL );

		if( bResults )
		{
			switch( dwStatusCode )
			{
			case HTTP_STATUS_OK: 
				// The resource was successfully retrieved.
				// You can use WinHttpReadData to read the 
				// contents of the server's response.
				printf( "The resource was successfully retrieved.\n" );
				bDone = TRUE;
				bAuthSucceed = TRUE;
				break;
			case HTTP_STATUS_DENIED:
				// The server requires authentication.
				printf(" The server requires authentication. Sending credentials...\n" );

				// Obtain the supported and preferred schemes.
				bResults = WinHttpQueryAuthSchemes( m_hRequest, 
					&dwSupportedSchemes, 
					&dwFirstScheme, 
					&dwTarget );

				// Set the credentials before resending the request.
				if( bResults )
				{
					dwSelectedScheme = ChooseAuthScheme( dwSupportedSchemes);

					if( dwSelectedScheme == 0 )
						bDone = TRUE;
					else
						bResults = WinHttpSetCredentials( m_hRequest, 
						dwTarget, 
						dwSelectedScheme,
						m_strAccount.c_str(),
						m_strPassword.c_str(),
						NULL );
				}

				// If the same credentials are requested twice, abort the
				// request.  For simplicity, this sample does not check
				// for a repeated sequence of status codes.
				if( dwLastStatus == HTTP_STATUS_DENIED )
					bDone = TRUE;

				break;

			case HTTP_STATUS_PROXY_AUTH_REQ:
				// The proxy requires authentication.
				printf( "The proxy requires authentication.  Sending credentials...\n" );

				// Obtain the supported and preferred schemes.
				bResults = WinHttpQueryAuthSchemes( m_hRequest, 
					&dwSupportedSchemes, 
					&dwFirstScheme, 
					&dwTarget );

				// Set the credentials before resending the request.
				if( bResults )
					dwProxyAuthScheme = ChooseAuthScheme(dwSupportedSchemes);

				// If the same credentials are requested twice, abort the
				// request.  For simplicity, this sample does not check 
				// for a repeated sequence of status codes.
				if( dwLastStatus == HTTP_STATUS_PROXY_AUTH_REQ )
					bDone = TRUE;
				break;

			default:
				// The status code does not indicate success.
				printf("Error. Status code %d returned.\n", dwStatusCode);
				bDone = TRUE;
			}
		}

		// Keep track of the last status code.
		dwLastStatus = dwStatusCode;

		// If there are any errors, break out of the loop.
		if( !bResults ) 
			bDone = TRUE;
	}

	// Report any errors.
	if( !bResults )
	{
		DWORD dwLastError = GetLastError( );
		printf( "Error %d has occurred.\n", dwLastError );
		return FALSE;
	}
	else
		return bAuthSucceed;
}

BOOL CHttpClient::TransmiteDataToServerByGet()
{
	BOOL bSucceed = FALSE;
	BOOL bResults = FALSE;
	do 
	{
		std::wstring wstrUrlPathAppend = m_wstrUrlPath;
		// 采用Get方式时，要将参数放在OpenRequest中
		if ( false == wstrUrlPathAppend.empty() ) {
			wstrUrlPathAppend += L"?";
		}
		wstrUrlPathAppend += GenerateExtInfo(m_VecExtInfo);

		m_hRequest = WinHttpOpenRequest(m_hConnect,
			L"Get",
			wstrUrlPathAppend.c_str(), 
			//NULL, 
			L" HTTP/1.0",
			WINHTTP_NO_REFERER, 
			WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

		if ( NULL == m_hRequest ) {
			break;
		}
		
		//CheckResponStatus();
		ModifyRequestHeader(m_hRequest);

		if (!WinHttpSendRequest( m_hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,	0,	WINHTTP_NO_REQUEST_DATA,	0, 	0, 	0 ))
			break;
			
		bSucceed = TRUE;

	} while (0);

	return bSucceed;

}


BOOL CHttpClient::TransmiteDataToServerByPost()
{
    BOOL bSuc = FALSE;
    do {
        m_hRequest = WinHttpOpenRequest(m_hConnect, L"Post",
            m_wstrUrlPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        if ( NULL == m_hRequest ) {
            break;
        }
	
        ModifyRequestHeader(m_hRequest);

        std::wstring wstrExtInfo = GenerateExtInfo(m_VecExtInfo);
		wstrExtInfo += m_wstrPayload;
        std::string strExtInfo = CW2A(wstrExtInfo.c_str(), CP_UTF8);

        DWORD dwTotal = strExtInfo.length();
        dwTotal += GetDataSize();

        if ( FALSE == WinHttpSendRequest( m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, dwTotal, 0)) {
            break;
        }

        if ( 0 != strExtInfo.length() ) {
            // 默认可以一次全部写完
            if ( FALSE == WinHttpWriteData(m_hRequest, strExtInfo.c_str(), strExtInfo.length(), NULL ) ) {
                break;
            }
        }

        // 静态分配一个数组
        BYTE buffer[1024]= {0};
        BOOL bContinue = FALSE;
        BOOL bSendOK = FALSE;

        do {
            DWORD dwBufferLength = sizeof(buffer);
            SecureZeroMemory(buffer, dwBufferLength);
            DWORD dwWriteSize = 0;
            bContinue = GetData(buffer, dwBufferLength, dwWriteSize);
            if ( 0 != dwWriteSize ) {
                bSendOK= WinHttpWriteData( m_hRequest, buffer, dwWriteSize, NULL);
            }
            else {
                bSendOK = TRUE;
            }
        } while ( bContinue && bSendOK );

        bSuc = bSendOK;

    } while (0);
    return bSuc;
}

BOOL CHttpClient::TransmiteDataToServerByUpload()
{
    BOOL bSuc = FALSE;
    do {
        m_hRequest = WinHttpOpenRequest(m_hConnect, L"Post",
            m_wstrUrlPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        if ( NULL == m_hRequest ) {
            break;
        }
	
        ModifyRequestHeader(m_hRequest);

        std::wstring wstrExtInfo = GenerateExtInfo(m_VecExtInfo);
        std::string strExtInfo = CW2A(wstrExtInfo.c_str(), CP_UTF8);

        DWORD dwTotal = strExtInfo.length();
        dwTotal += GetDataSize();

        if ( FALSE == WinHttpSendRequest( m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, dwTotal, 0)) {
            break;
        }

        // 静态分配一个数组
        BYTE buffer[1024]= {0};
        BOOL bContinue = FALSE;
        BOOL bSendOK = FALSE;

        do {
            DWORD dwBufferLength = sizeof(buffer);
            SecureZeroMemory(buffer, dwBufferLength);
            DWORD dwWriteSize = 0;
            bContinue = GetData(buffer, dwBufferLength, dwWriteSize);
            if ( 0 != dwWriteSize ) {
                bSendOK= WinHttpWriteData( m_hRequest, buffer, dwWriteSize, NULL);
            }
            else {
                bSendOK = TRUE;
            }
        } while ( bContinue && bSendOK );

        if ( 0 != strExtInfo.length() ) {
            if ( FALSE == WinHttpWriteData(m_hRequest, strExtInfo.c_str(), strExtInfo.length(), NULL ) ) {
                break;
            }
        }
        bSuc = bSendOK;
    } while (0);
    return bSuc;
}
