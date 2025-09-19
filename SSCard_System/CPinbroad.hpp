#pragma once
#include <windows.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <exception>
#include "Utility.h"
using namespace std;
using fnOpenCom = int (WINAPI*)(int nPort, long nBaud);
using fnCloseCom = int (WINAPI*)();
using fnUseEppPlainTextMode10 = int (WINAPI*)(unsigned char ucTextModeFormat, unsigned char* ReturnInfo);
using fnUseEppPlainTextMode31 = int (WINAPI*)(unsigned char ucTextModeFormat, unsigned char AutoEnd, unsigned char* ReturnInfo);
using fnScanKeyPress = int (WINAPI*)(unsigned char* ucKeyValue);
enum class PinBoardType
{
	PinBoard_F31,
	PinBoard_F10
};

class CPinbroad
{
	HMODULE hModule = nullptr;
	fnOpenCom pOpenCom = nullptr;
	fnCloseCom	pCloseCom = nullptr;
	fnUseEppPlainTextMode10	pUseEppPlainTextMode10 = nullptr;
	fnUseEppPlainTextMode31	pUseEppPlainTextMode31 = nullptr;
	fnScanKeyPress	pScanKeyPress = nullptr;
	PinBoardType	nType = PinBoardType::PinBoard_F31;
public:
	CPinbroad(PinBoardType nType = PinBoardType::PinBoard_F31)
	{
		const char* szType[] = { "\\XZ_F31_API.dll","\\XZ_F10_API.dll" };
		if (nType != PinBoardType::PinBoard_F31 &&
			nType != PinBoardType::PinBoard_F10)
		{
			char szError[128] = { 0 };
			sprintf_s((char*)szError, 1024, "Invalid device type,it must be F10 or F31!");
			throw exception(szError);
		}
		this->nType = nType;
		char szAppPath[1024] = { 0 };
		GetAppPathA(szAppPath, 1024);
		strcat_s(szAppPath, 1024, szType[(int)nType]);
		char szError[1024] = { 0 };
		if (!LoadModule(szAppPath, szError, 1024))
			throw exception(szError);
	}

	bool LoadModule(const char *szModule,char *szError,int nBuffSize)
	{
		if (!PathFileExistsA(szModule))
		{
			sprintf_s((char*)szError, nBuffSize, "The specified module %s not exist!", szModule);
			return false;
		}
		hModule = LoadLibraryA(szModule);
		if (!hModule)
		{
			sprintf_s((char*)szError, nBuffSize, "Failed in loading module %s!", szModule);
			return false;
		}
		pOpenCom = (fnOpenCom)GetProcAddress(hModule, "SUNSON_OpenCom");
		pCloseCom = (fnCloseCom)GetProcAddress(hModule, "SUNSON_CloseCom");
		pScanKeyPress = (fnScanKeyPress)GetProcAddress(hModule, "SUNSON_ScanKeyPress");
		if (nType == PinBoardType::PinBoard_F31)
			pUseEppPlainTextMode31 = (fnUseEppPlainTextMode31)GetProcAddress(hModule, "SUNSON_UseEppPlainTextMode");
		else
			pUseEppPlainTextMode10 = (fnUseEppPlainTextMode10)GetProcAddress(hModule, "SUNSON_UseEppPlainTextMode");
		if (!pOpenCom || 
			!pCloseCom || 
			!pScanKeyPress || 
			(nType == PinBoardType::PinBoard_F31 &&!pUseEppPlainTextMode31)||
			(nType == PinBoardType::PinBoard_F10 && !pUseEppPlainTextMode10))
		{
			sprintf_s((char*)szError, 1024, "Failed in locating procedure in module %s!", szModule);
			return false;
		}
		return true;
	}
	~CPinbroad()
	{
		if (hModule)
		{
			FreeLibrary(hModule);
		}
	}
	// ·µ³É1³É¹¦£¬-1Ê§°Ü
	int OpenCom(int nPort,int nBaudrate)
	{
		if (pOpenCom)
		{
			if (nType == PinBoardType::PinBoard_F10)
			{
				if (!pOpenCom(nPort, nBaudrate))
					return 1;
				else
					return -1;
			}
			else
				return pOpenCom(nPort, nBaudrate);
		}
		else
		{
			return -1;
		}
	}

	int CloseCom()
	{
		if (pCloseCom)
		{
			if (nType == PinBoardType::PinBoard_F10)
			{
				if (!pCloseCom())
					return 1;
				else
					return -1;
			}
			else 
				return pCloseCom();
		}
		else
			return -1;
	}

	int UseEppPlainTextMode(unsigned char ucTextModeFormat,unsigned char AutoEnd, unsigned char* ReturnInfo)
	{
		if (nType == PinBoardType::PinBoard_F10)
		{
			if (!pUseEppPlainTextMode10)
				return -1;
			if (!pUseEppPlainTextMode10(3, ReturnInfo))
				return 1;
			else
				return -1;
		}
		else if (pUseEppPlainTextMode31)
			return pUseEppPlainTextMode31(ucTextModeFormat, AutoEnd, ReturnInfo);
		else
			return -1;
	}

	int ScanKeyPress(unsigned char* ucKeyValue)
	{
		if (pScanKeyPress)
		{
			if (nType == PinBoardType::PinBoard_F10)
			{
				if (!pScanKeyPress(ucKeyValue))
					return 1;
				else
					return -1;
			}
			else
				return pScanKeyPress(ucKeyValue);
		}
		else
			return -1;
	}
};

