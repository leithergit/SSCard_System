// dllmain.cpp : Defines the entry point for the DLL application.

#include "SSCardService_Sandong.h"
#include "pch.h"
#pragma comment(lib,"../sdk/sscardinfo_sandong/SD_SSCardInfod.lib")

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

	LPVOID WINAPI CreateInstance(void* pServiceType)
	{
		//if (!pServiceType)
		//	return nullptr;
		//ServiceType nServiceType = (ServiceType) * (int*)pServiceType;
		//if (nServiceType < ServiceType::Service_NewCard ||
		//	nServiceType > ServiceType::Service_RegisterLost)
		//	return nullptr;

		SSCardService_Sandong* pService = new SSCardService_Sandong();
		//pService->SetSerivdeType((ServiceType)nServiceType);
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
