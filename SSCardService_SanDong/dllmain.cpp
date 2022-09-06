// dllmain.cpp : Defines the entry point for the DLL application.

#include "SSCardService_Sandong.h"
#include "pch.h"
#ifdef _DEBUG
#pragma comment(lib,"../sdk/SSCardinfo_Binzhou/SSCardInfo_BinZhoud.lib")
#else
#pragma comment(lib,"../sdk/SSCardinfo_Binzhou/SSCardInfo_BinZhou.lib")
#endif
map<string, string>g_mapNationnaltyCode;
struct NationaltyCode
{
	string strCode;
	string strNationalty;
};

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	X_UNUSED(hModule);
	X_UNUSED(lpReserved);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		NationaltyCode NCArray[] = {
			{"01", "汉族"},
			{ "02","蒙古族" },
			{ "03","回族" },
			{ "04","藏族" },
			{ "05","维吾尔族" },
			{ "06","苗族" },
			{ "07","彝族" },
			{ "08","壮族" },
			{ "09","布依族" },
			{ "10","朝鲜族" },
			{ "11","满族" },
			{ "12","侗族" },
			{ "13","瑶族" },
			{ "14","白族" },
			{ "15","土家族" },
			{ "16","哈尼族" },
			{ "17","哈萨克族" },
			{ "18","傣族" },
			{ "19","黎族" },
			{ "20","傈傈族" },
			{ "21","佤族" },
			{ "22","畲族" },
			{ "23","高山族" },
			{ "24","拉祜族" },
			{ "25","水族" },
			{ "26","东乡族" },
			{ "27","纳西族" },
			{ "28","景颇族" },
			{ "29","柯尔克孜族" },
			{ "30","土族" },
			{ "31","达翰尔族" },
			{ "32","仫佬族" },
			{ "33","羌族" },
			{ "34","布朗族" },
			{ "35","撒拉族" },
			{ "36","毛南族" },
			{ "37","仡佬族" },
			{ "38","锡伯族" },
			{ "39","阿昌族" },
			{ "40","普米族" },
			{ "41","塔吉克族" },
			{ "42","怒族" },
			{ "43","乌孜别克族" },
			{ "44","俄罗斯族" },
			{ "45","鄂温克族" },
			{ "46","德昂族" },
			{ "47","保安族" },
			{ "48","裕固族" },
			{ "49","京族" },
			{ "50","塔塔尔族" },
			{ "51","独龙族" },
			{ "52","鄂伦春族" },
			{ "53","赫哲族" },
			{ "54","门巴族" },
			{ "55","珞巴族" },
			{ "56","基诺族" },
			{ "57","穿青人" },
			{ "90","外籍人士" },
			{ "99","其他" }
		};
		for (auto var : NCArray)
		{
			auto [it, Inserted] = g_mapNationnaltyCode.try_emplace(var.strNationalty, var.strCode);
		}
	}
	break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
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
		X_UNUSED(pServiceType);
		//if (!pServiceType)
		//	return nullptr;
		//ServiceType nServiceType = (ServiceType) * (int*)pServiceType;
		//if (nServiceType < ServiceType::Service_NewCard ||
		//	nServiceType > ServiceType::Service_RegisterLost)
		//	return nullptr;

		SSCardService_BinZhou* pService = new SSCardService_BinZhou();
		//pService->SetSerivdeType((ServiceType)nServiceType);
		return (LPVOID)pService;
	}

	void WINAPI FreeInstance(LPVOID lpDev)
	{
		if (!lpDev)
			return;
		delete (SSCardService_BinZhou*)lpDev;
	}

#ifdef __cplusplus
}
#endif
