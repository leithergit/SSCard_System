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
				strError = QString("�Ӷ�̬��%1�м���'CreateInstance'��'FreeInstance'����ʧ��!").arg(strLibPath.c_str());
				throw std::exception(strError.toStdString().c_str());
				return;
			}
			pInstance = (T*)pCreateInst(nullptr);
			if (!pInstance)
			{
				strError = QString("����ģ��'%1'ʵ��ʧ��!").arg(strLibPath.c_str());
				throw std::exception(strError.toStdString().c_str());
				return;
			}
		}
		else
		{
			strError = QString("���ض�̬��'%1'ʧ�ܣ�������:%1").arg(strLibPath.c_str()).arg(errno);
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
