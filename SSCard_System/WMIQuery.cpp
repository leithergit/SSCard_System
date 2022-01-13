#pragma warning(push)
#pragma warning(disable:4819)
#include "WMIQuery.h"
#include "../utility/Utility.h"
#pragma warning(pop)

#include <objbase.h>
#pragma comment(lib,"wbemuuid.lib")

#define  X_Var(x)	(x)
CWMIQuery::CWMIQuery()
{
	//ZeroMemory(this,sizeof(CWMIQuery));
	m_pAccessValue = nullptr;
	ZeroMemory(m_wstrNamespace, sizeof(m_wstrNamespace));
	m_dwUserPtr = 0;
	m_pWbemLocator = nullptr;
	m_pWbemService = nullptr;
	m_bUnInitialize = false;
}

CWMIQuery::CWMIQuery(const wchar_t* strWMINameSpaces)
{
	//ZeroMemory(this,sizeof(CWMIQuery));
	m_pAccessValue = nullptr;
	m_dwUserPtr = 0;
	ZeroMemory(m_wstrNamespace, sizeof(m_wstrNamespace));
	m_pWbemLocator = nullptr;
	m_pWbemService = nullptr;
	SetNamespace(strWMINameSpaces);
	m_bUnInitialize = false;
}
CWMIQuery::~CWMIQuery()
{
}

VOID CWMIQuery::SetNamespace(const wchar_t* wstrNamespace)
{
	wcscpy(m_wstrNamespace, wstrNamespace);
}

HRESULT CWMIQuery::Initialize()
{
	HRESULT hr = E_FAIL;
	do {
		hr = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (hr != 0x80010106)
		{
			CHECKHR(hr);
		}
		hr = CoInitializeSecurity(
			NULL,
			-1,                          // COM negotiates service
			NULL,                        // Authentication services
			NULL,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
			NULL,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities 
			NULL                         // Reserved
		);
		CHECKHR(hr);

	} while (0);
	return hr;
}

HRESULT CWMIQuery::ConnectWMI(const wchar_t* strWMINameSpaces)
{
	HRESULT hr = E_FAIL;
	do {

		if (strWMINameSpaces && wcslen(strWMINameSpaces) > 0)
			wcscpy(m_wstrNamespace, strWMINameSpaces);

		if (!m_wstrNamespace || !wcslen(m_wstrNamespace))
			return E_FAIL;


		hr = CoCreateInstance(
			CLSID_WbemLocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator, (LPVOID*)&m_pWbemLocator);
		CHECKHR(hr);

		hr = m_pWbemLocator->ConnectServer(
			CComBSTR(m_wstrNamespace),
			NULL, NULL, NULL, NULL, NULL, NULL, &m_pWbemService);
		CHECKHR(hr);

		hr = CoSetProxyBlanket(
			m_pWbemService,                        // Indicates the proxy to set
			RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
			RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
			NULL,                        // Server principal name 
			RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
			RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
			NULL,                        // client identity
			EOAC_NONE                    // proxy capabilities 
		);
		CHECKHR(hr);

	} while (0);
	return hr;
}

HRESULT CWMIQuery::ProcessItem(CComBSTR& bstrName, CComVariant& Value, CIMTYPE type, LONG lFlavor)
{

	HRESULT hr = WBEM_S_NO_ERROR;
	switch (Value.vt) {
	case VT_UNKNOWN: {
		ProcessUnknownItem(bstrName, Value, type, lFlavor);
	}break;
	default: {
		SaveItem(bstrName, Value, type, lFlavor);
	};
	}
	return hr;
}

HRESULT CWMIQuery::ProcessUnknownItem(CComBSTR& bstrName, CComVariant& Value, CIMTYPE type, LONG lFlavor)
{
	X_Var(lFlavor);
	X_Var(type);
	X_Var(bstrName);

	HRESULT hr = WBEM_S_NO_ERROR;
	if (NULL == Value.punkVal) {
		return hr;
	}
	// object类型转换成IWbemClassObject接口指针，通过该指针枚举其他属性
	CComPtr<IWbemClassObject> pObjInstance = (IWbemClassObject*)Value.punkVal;
	hr = pObjInstance->BeginEnumeration(WBEM_FLAG_LOCAL_ONLY);
	do {
		CHECKHR(hr);
		CComBSTR bstrNewName;
		CComVariant NewValue;
		CIMTYPE newtype;
		LONG lnewFlavor = 0;
		hr = pObjInstance->Next(0, &bstrNewName, &NewValue, &newtype, &lnewFlavor);
		CHECKHR(hr);
		SaveItem(bstrNewName, NewValue, newtype, lnewFlavor);
	} while (WBEM_S_NO_ERROR == hr);
	hr = pObjInstance->EndEnumeration();
	return WBEM_S_NO_ERROR;
}

VOID CWMIQuery::SaveItem(CComBSTR bstrName, CComVariant Value, CIMTYPE type, LONG lFlavor)
{
	X_Var(lFlavor);
	X_Var(type);
	//wstring strName = (wstring)(WCHAR *)bstrName.m_str;
	if (m_pAccessValue)
		m_pAccessValue((WCHAR*)bstrName.m_str, Value, m_dwUserPtr);
}

HRESULT CWMIQuery::ProcessIWbemClassObject(CComPtr<IWbemClassObject> pclsObj)
{
	HRESULT hr = WBEM_S_NO_ERROR;
	do {
		CComVariant vtClass;
		hr = pclsObj->Get(L"__CLASS", 0, &vtClass, NULL, NULL);
		CHECKWMIHR(hr);
		if (VT_BSTR == vtClass.vt)
			TraceMsgW(L"\n%s\n", vtClass.bstrVal);

		TraceMsgW(L"\n");

		hr = pclsObj->BeginEnumeration(WBEM_FLAG_LOCAL_ONLY);


		do
		{
			CComBSTR bstrName;
			CComVariant Value;
			CIMTYPE type;
			LONG lFlavor = 0;
			hr = pclsObj->Next(0, &bstrName, &Value, &type, &lFlavor);
			CHECKWMIHR(hr);
			hr = ProcessItem(bstrName, Value, type, lFlavor);

		} while (WBEM_S_NO_ERROR == hr);
		TraceMsgW(L"\n");
		if (m_pAccessValue)
			m_pAccessValue(L"EnumerationFinished", (CComVariant)0, m_dwUserPtr);

		hr = pclsObj->EndEnumeration();
	} while (0);

	return hr;
}

HRESULT CWMIQuery::Query(const wchar_t* strWQL, AccessValueProc pCallBack, DWORD_PTR pUserPtr)
{
	m_pAccessValue = pCallBack;
	m_dwUserPtr = pUserPtr;
	HRESULT hr = E_FAIL;
	do {
		hr = WBEM_S_FALSE;
		do {
			CComPtr<IEnumWbemClassObject> pEnumerator = NULL;
			hr = m_pWbemService->ExecQuery(
				CComBSTR("WQL"),
				CComBSTR(strWQL),
				WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
				NULL,
				&pEnumerator);

			CHECKWMIHR(hr);

			ULONG uReturn = 0;

			while (pEnumerator)
			{
				CComPtr<IWbemClassObject> pclsObj = NULL;
				hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

				if (0 == uReturn)
				{
					break;
				}
				ProcessIWbemClassObject(pclsObj);
			}

		} while (0);

		CHECKHR(hr);

	} while (0);
	return hr;
}
