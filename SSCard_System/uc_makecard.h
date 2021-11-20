#ifndef UC_MAKECARD_H
#define UC_MAKECARD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
#include "./SDK/Printer/KT_Printer.h"
#include "./SDK/Reader/KT_Reader.h"
#include <QCoreApplication>
#include <QtDebug>
#include <string.h>
#include <exception>
#include <Windows.h>
#include "Gloabal.h"

using namespace std;
using namespace Kingaotech;

namespace Ui {
	class MakeCard;
}

using pCreateInstance =  LPVOID  (*)(LPVOID lpReserve);
using pFreeInstance = void (*)(LPVOID lpDev);

class KT_Printerlib
{
public:
    KT_Printer* pInstance = nullptr;
    HMODULE hLibhandle = nullptr;
    pCreateInstance pCreateInst = nullptr;
    pFreeInstance pFreeInst = nullptr;
    QString strError;
    KT_Printerlib()
    {
        QString strLibPath = QCoreApplication::applicationDirPath();
        strLibPath+=  u8"/KT_Printer.dll";
        qDebug()<<"Try to load "<<strLibPath;
        hLibhandle = ::LoadLibraryA(strLibPath.toStdString().c_str());
        if (hLibhandle)
        {
            pCreateInst = (pCreateInstance) GetProcAddress(hLibhandle,"CreateInstance");
            pFreeInst = (pFreeInstance)GetProcAddress(hLibhandle,"FreeInstance");
            if (!pCreateInst || !pFreeInst)
            {
                strError = QString("从动态库%1中加载‘CreateInstance’或‘FreeInstance’函数失败!").arg(strLibPath);
                throw std::exception(strError.toStdString().c_str());
                return ;
            }
            pInstance = (KT_Printer*)pCreateInst(nullptr);
            if (!pInstance)
            {
                strError = QString(u8"创建'KT_Printer'实例失败!");
                throw std::exception(strError.toStdString().c_str());
                return;
            }
        }
        else
        {
            strError = QString("加载动态库‘%1’失败，错误码:%1").arg(strLibPath).arg(errno);
            throw std::exception(strError.toStdString().c_str());
        }
    }
    ~KT_Printerlib()
    {
        if (pInstance)
        {
            char szCode[1024] = {0};
            pInstance->Printer_Close(szCode);
            pFreeInst(pInstance);
            pInstance = nullptr;
        }
        if (hLibhandle)
            FreeLibrary(hLibhandle);
    }

    KT_Printer *Instance()
    {
        return pInstance;
    }
};

using KT_PrinterLibPtr = shared_ptr<KT_Printerlib>;

class uc_MakeCard : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_MakeCard(QLabel* pTitle, int nTimeout = 300, QWidget* parent = nullptr);
	~uc_MakeCard();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
    int OpenDevice(QString &strMessage);
    void CloseDevice();
    void ThreadWork();
    virtual void ShutDown() override;

private:
	Ui::MakeCard* ui;
    KT_PrinterLibPtr   m_pPrinterlib = nullptr;
    KT_Printer *m_pPrinter = nullptr;
    SSCardInfoPtr m_pSSCardInfo = nullptr;
};

#endif // UC_MAKECARD_H
