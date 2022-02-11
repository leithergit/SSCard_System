// dllmain.cpp : Defines the entry point for the DLL application.

#include "SSCardService_Sandong.h"
#include "pch.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef __cplusplus
extern "C" {
#endif

	LPVOID WINAPI CreateInstance(int nServiceType)
	{
		if (nServiceType < Service_NewCard ||
			nServiceType > Service_RegisterLost)
			return nullptr;

		SSCardService_Sandong* pService = new SSCardService_Sandong((ServiceType)nServiceType);
		return (LPVOID)pService;
	}

	void WINAPI FreeInstance(LPVOID lpDev)
	{
		if (!lpDev)
			return;
		delete (SSCardService_Sandong*)lpDev;
	}

#ifdef __cplusplus
}
#endif
