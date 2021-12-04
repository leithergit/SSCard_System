#ifndef UC_MAKECARD_H
#define UC_MAKECARD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

#include <QCoreApplication>
#include <QDebug>
#include <string.h>
#include <exception>
#include <Windows.h>
#include "Gloabal.h"

using namespace std;
using namespace Kingaotech;

namespace Ui {
	class MakeCard;
}

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
				strError = QString("从动态库%1中加载‘CreateInstance’或‘FreeInstance’函数失败!").arg(strLibPath.c_str());
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
			strError = QString("加载动态库‘%1’失败，错误码:%1").arg(strLibPath.c_str()).arg(errno);
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

using KT_PrinterLibPtr = shared_ptr<KTModule<KT_Printer>>;
using KT_ReaderLibPtr = shared_ptr<KTModule<KT_Reader>>;

class uc_MakeCard : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_MakeCard(QLabel* pTitle, QString strStepImage, int nTimeout = 300, QWidget* parent = nullptr);
	~uc_MakeCard();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	int OpenDevice(QString& strMessage);
	int OpenPrinter(QString& strMesssage);
	int OpenReader(QString& strMesssage);
	string MakeCardInfo(string strATR, SSCardInfoPtr& pSSCardInfo);
	int WriteCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage);
	int PrintCard(SSCardInfoPtr& pSSCardInfo, QString& strMessage);
	//int GetCA(string& strPublicKey, SSCardInfoPtr& pSSCardInfo, CAInfo& caInfo, QString& strMessage);
	int Depense(QString& strMessage);
	void CloseDevice();
	void ThreadWork();
	virtual void ShutDown() override;
	int     m_nSocketRetryInterval = 500;            // 支付结构查询时间间隔单 毫秒
	int     m_nSocketRetryCount = 5;                    // 网络失败重试次数
	int     m_nSocketFailedCount = 0;

private:
	Ui::MakeCard* ui;
	KT_PrinterLibPtr	m_pPrinterlib = nullptr;
	KT_ReaderLibPtr		m_pReaderLib = nullptr;
	KT_Printer* m_pPrinter = nullptr;
	KT_Reader* m_pReader = nullptr;

	SSCardInfoPtr m_pSSCardInfo = nullptr;
};

#endif // UC_MAKECARD_H
