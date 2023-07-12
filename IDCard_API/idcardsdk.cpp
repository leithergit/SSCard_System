#pragma execution_character_set("utf-8")
#include "idcardsdk.h"
#include <Shlwapi.h>

IDCardSDK::IDCardSDK()
{
	char szAppPath[1024] = { 0 };
	GetAppPathA(szAppPath, 1024);
	strcat_s(szAppPath, 1024, "\\termb.dll");
	if (!::PathFileExistsA(szAppPath))
	{
		return;
	}
	m_hLibHandle = LoadLibraryA(szAppPath);
	if (!m_hLibHandle)
		return;
	LoadSDK();
}

IDCardSDK::~IDCardSDK()
{
	if (m_hLibHandle)
	{
		__try
		{
			FreeLibrary(m_hLibHandle);
			m_hLibHandle = nullptr;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			m_hLibHandle = nullptr;
			TraceMsgA("%s %d Catch a exection!", __FUNCTION__, __LINE__);
		}

	}
}

void IDCardSDK::LoadSDK()
{
	if (m_hLibHandle != NULL)
	{
		CVR_InitComm = (fnCVR_InitComm)GetProcAddress(m_hLibHandle, "CVR_InitComm");
		CVR_CloseComm = (fnCVR_CloseComm)GetProcAddress(m_hLibHandle, "CVR_CloseComm");
		CVR_Authenticate = (fnCVR_Authenticate)GetProcAddress(m_hLibHandle, "CVR_Authenticate");
		CVR_Read_Content = (fnCVR_Read_Content)GetProcAddress(m_hLibHandle, "CVR_Read_FPContent");
		CVR_GetSAMID = (fnCVR_GetSAMID)GetProcAddress(m_hLibHandle, "CVR_GetSAMID");		////
		GetFPDate = (fnGetFPDate)GetProcAddress(m_hLibHandle, "GetFPDate");
		GetBmpData = (fnGetBmpData)GetProcAddress(m_hLibHandle, "GetBMPData");
		GetPeopleName = (fnGetPeopleName)GetProcAddress(m_hLibHandle, "GetPeopleName");
		GetPeopleSex = (fnGetPeopleSex)GetProcAddress(m_hLibHandle, "GetPeopleSex");
		GetPeopleNation = (fnGetPeopleNation)GetProcAddress(m_hLibHandle, "GetPeopleNation");
		GetPeopleBirthday = (fnGetPeopleBirthday)GetProcAddress(m_hLibHandle, "GetPeopleBirthday");
		GetPeopleAddress = (fnGetPeopleAddress)GetProcAddress(m_hLibHandle, "GetPeopleAddress");
		GetPeopleIDCode = (fnGetPeopleIDCode)GetProcAddress(m_hLibHandle, "GetPeopleIDCode");
		GetDepartment = (fnGetDepartment)GetProcAddress(m_hLibHandle, "GetDepartment");
		GetStartDate = (fnGetStartDate)GetProcAddress(m_hLibHandle, "GetStartDate");
		GetEndDate = (fnGetEndDate)GetProcAddress(m_hLibHandle, "GetEndDate");
		GetPeopleChineseName = (fnGetPeopleChineseName)GetProcAddress(m_hLibHandle, "GetPeopleChineseName");
		GetPeopleCertVersion = (fnGetPeopleCertVersion)GetProcAddress(m_hLibHandle, "GetPeopleCertVersion");
		GetNationCode = (fnGetNationCode)GetProcAddress(m_hLibHandle, "GetNationCode");
		GetNewAppMsg = (fnGetNewAppMsg)GetProcAddress(m_hLibHandle, "GetNewAppMsg");
		GetCertType = (fnGetCertType)GetProcAddress(m_hLibHandle, "GetCertType");
		GetNationCode = (fnGetNationCode)GetProcAddress(m_hLibHandle, "GetNationCode");
		GetPassCheckID = (fnGetPassCheckID)GetProcAddress(m_hLibHandle, "GetPassCheckID");
		GetIssuesNum = (fnGetIssuesNum)GetProcAddress(m_hLibHandle, "GetIssuesNum");
	}
}

int IDCardSDK::OpenUSB(int nPort)
{
	if (!m_hLibHandle)
		return IDCard_Library_Notload;
	if (m_nStatus)
	{
		CVR_CloseComm();
		m_nStatus = 0;
	}

	if (nPort != -1)
	{
		m_nStatus = CVR_InitComm(nPort);
	}
	else
	{
		if (m_nPort != -1)
		{
			m_nStatus = CVR_InitComm(m_nPort);
		}
		else
		{
			for (int nIndex = UsbPort1; nIndex < UsbPort16; nIndex++)
			{
				m_nStatus = CVR_InitComm(nIndex);
				if (m_nStatus == 1)
				{
					m_nPort = nIndex;
					break;
				}
			}
		}
	}
	if (m_nStatus == 1)
		return IDCard_Succeed;
	else
	{
		switch (m_nStatus)
		{
		case 2:
			return IDCard_Failed_OpenPort;
		case -2:
			return IDCard_Library_Notload;
		default:
		case -1:
			return IDCard_Error_Unknow;
		}
	}
}

int IDCardSDK::OpenSerialport(int nPort)
{
	if (!m_hLibHandle)
		return IDCard_Library_Notload;
	if (m_nStatus)
	{
		CVR_CloseComm();
		m_nStatus = 0;
	}

	if (nPort != -1)
	{
		m_nStatus = CVR_InitComm(nPort);
	}
	else
	{
		if (m_nPort != -1)
		{
			m_nStatus = CVR_InitComm(nPort);
		}
		else
		{
			for (int nIndex = ComPort1; nIndex < ComPort16; nIndex++)
			{
				m_nStatus = CVR_InitComm(nIndex);
				if (m_nStatus == 1)
				{
					m_nPort = nIndex;
					break;
				}
			}
		}
	}
	if (m_nStatus == 1)
		return IDCard_Succeed;
	else
	{
		switch (m_nStatus)
		{
		case 2:
			return IDCard_Failed_OpenPort;
		case -2:
			return IDCard_Library_Notload;
		default:
		case -1:
			return IDCard_Error_Unknow;
		}
	}
}

int IDCardSDK::OpenDev(int nPort)
{
	int nResult = 0;
	if (nPort == -1)
	{
		nResult = OpenUSB(nPort);
		if (nResult == IDCard_Succeed)
			return nResult;
		else
			return OpenSerialport(nPort);
	}
	else if (nPort >= ComPort1 && nPort <= ComPort16)
		return OpenSerialport(nPort);
	else if (nPort >= UsbPort1 && nPort <= UsbPort16)
		return OpenUSB(nPort);
	else
		return IDCard_Invalid_Port;
}

int IDCardSDK::CloseDev()
{
	if (!m_hLibHandle)
		return IDCard_Library_Notload;
	if (m_nStatus)
	{
		__try
		{
			m_nStatus = 0;
			CVR_CloseComm();

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			TraceMsgA("%s %d Catch a exection!", __FUNCTION__, __LINE__);
		}
	}
	return IDCard_Succeed;
}

int IDCardSDK::Authenticate()
{
	int nResult = CVR_Authenticate();
	switch (nResult)
	{
	case 1:
		return IDCard_Succeed;
		break;
	case 2:
		return IDCard_Failed_Find;
		break;
	case 3:
		return IDCard_Failed_Select;
		break;
	case 4:
		return IDCard_Reader_NotConnected;
		break;
	case 0:
		return IDCard_Library_Notload;
		break;
	default:
		return IDCard_Error_Unknow;
		break;
	}
}

int IDCardSDK::LoadCardInfo(IDCardInfo& IDCard)
{
	int nResult = CVR_Read_Content();
	if (nResult != 1)
	{
		switch (nResult)
		{
		case 0: // 0    错误，读身份证失败
			return IDCard_Failed_Read;
		case 4: // 4    错误，身份证读卡器未连接
			return IDCard_Reader_NotConnected;
		case 99:// 动态库未加载
			return IDCard_Library_Notload;
		default:
			return IDCard_Error_Unknow;
		}
	}

	int nLen;
	IDCard.Reset();
	nLen = sizeof(IDCard.szName);
	GetPeopleName(IDCard.szName, &nLen);

	nLen = sizeof(IDCard.szGender);
	GetPeopleSex(IDCard.szGender, &nLen);

	nLen = sizeof(IDCard.szNation);
	GetPeopleNation(IDCard.szNation, &nLen);

	nLen = sizeof(IDCard.szBirthday);
	GetPeopleBirthday(IDCard.szBirthday, &nLen);

	nLen = sizeof(IDCard.szBirthday);
	GetPeopleAddress(IDCard.szAddress, &nLen);

	nLen = sizeof(IDCard.szIdentity);
	GetPeopleIDCode(IDCard.szIdentity, &nLen);

	nLen = sizeof(IDCard.szIszssueAuthority);
	GetDepartment(IDCard.szIszssueAuthority, &nLen);

	nLen = sizeof(IDCard.szExpirationDate1);
	GetStartDate(IDCard.szExpirationDate1, &nLen);

	nLen = sizeof(IDCard.szExpirationDate2);
	GetEndDate(IDCard.szExpirationDate2, &nLen);

	nLen = sizeof(IDCard.szNationCode);
	GetNationCode(IDCard.szNationCode, &nLen);

	nLen = sizeof(IDCard.szPhoto);
	GetBmpData(IDCard.szPhoto, (int*)&IDCard.nPhotoSize);

	//    unsigned char  szFingerData[2048] = {0};
	//    GetFPDate(szFingerData, &nLen);
	//    memcpy_s(&IDCard.FingerPrint,sizeof(IDCard.FingerPrint),szFingerData,nLen);
	return IDCard_Succeed;
}

const int IDCardSDK::GetCurrentPort()
{
	return m_nPort;
}

int IDCardSDK::GetErorrMessage(IDCard_Status nCode, char* szMessage, int nBufferSize)
{
	switch (nCode)
	{
	case IDCard_Succeed:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"操作成功");
		break;
	}
	case IDCard_Invalid_Port:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"读卡器端口无效");
		break;
	}
	case IDCard_Invalid_Parameter:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"输入参数无效");
		break;
	}
	case IDCard_Reader_NotOpen:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"未打开身份证读卡器");
		break;
	}
	case IDCard_Library_Notload:
	{
		//strcpy_s(szMessage,nBufferSize,"The library is not loaded");
		strcpy_s(szMessage, nBufferSize, (char*)u8"读卡器动态库或相关依赖库未加载");
		break;
	}
	case IDCard_Reader_NotConnected:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"读卡器未连接");
		break;
	}
	case IDCard_Failed_OpenPort:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"读卡器打开端口失败");
		break;
	}
	case IDCard_Failed_Find:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"未检测到身份证");
		break;
	}
	case IDCard_Failed_Select:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"身份证选卡失败");
		break;
	}
	case IDCard_Failed_Read:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"读取身份证失败");
		break;
	}
	case IDCard_Error_InsufficentMemory:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"内存不足");
		break;
	}
	case IDCard_Error_Unknow:
	{
		strcpy_s(szMessage, nBufferSize, (char*)u8"未知错误");
		break;
	}
	}
	return strlen(szMessage);
}
