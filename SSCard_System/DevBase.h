#ifndef DEVBASE_H
#define DEVBASE_H
#include <string>
#include <memory>
#include <QString>

#pragma execution_character_set("utf-8")
//#include "Gloabal.h"

using namespace std;
using pCreateInstance = LPVOID(*)(LPVOID lpReserve);
using pFreeInstance = void (*)(LPVOID lpDev);

template<typename T>
class KTModule
{
public:
	T* pInstance = nullptr;
	HMODULE hLibhandle = nullptr;
	pCreateInstance pCreateInst = nullptr;
	pFreeInstance pFreeInst = nullptr;
	QString strError;
	KTModule(string strLibPath)
	{
		// 		QString strLibPath = QCoreApplication::applicationDirPath();
		// 		strLibPath += "/KT_Printer.dll";
		hLibhandle = ::LoadLibraryA(strLibPath.c_str());
		if (hLibhandle)
		{
			pCreateInst = (pCreateInstance)GetProcAddress(hLibhandle, "CreateInstance");
			pFreeInst = (pFreeInstance)GetProcAddress(hLibhandle, "FreeInstance");
			if (!pCreateInst || !pFreeInst)
			{
				strError = QString("从动态库%1中加载'CreateInstance'或'FreeInstance'函数失败!").arg(strLibPath.c_str());
				throw std::exception(strError.toStdString().c_str());
				return;
			}
			pInstance = (T*)pCreateInst(nullptr);
			if (!pInstance)
			{
				strError = QString("创建模块'%1'实例失败!").arg(strLibPath.c_str());
				throw std::exception(strError.toStdString().c_str());
				return;
			}
		}
		else
		{
			strError = QString("加载动态库'%1'失败，错误码:%1").arg(strLibPath.c_str()).arg(errno);
			throw std::exception(strError.toStdString().c_str());
		}
	}
	~KTModule()
	{
		if (pInstance)
		{
			//char szCode[1024] = { 0 };
			//pInstance->Printer_Close(szCode);
			pFreeInst(pInstance);
			pInstance = nullptr;
		}
		if (hLibhandle)
			FreeLibrary(hLibhandle);
	}

	T* Instance()
	{
		return pInstance;
	}
};


#endif // DEVBASE_H
