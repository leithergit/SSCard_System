#ifndef QPINKEYBROAD_H
#define QPINKEYBROAD_H

#include <QObject>
#include <memory>
#include <wtypes.h>
using namespace std;
enum PinBroadType
{   
    XZ_F10,
    XZ_F31
};

using fnSUNSON_OpenCom               = int (WINAPI *)(int nPort, long nBaud);
using fnSUNSON_CloseCom              = int (WINAPI*)(void);
using fnSUNSON_ScanKeyPress          = int (WINAPI*)(unsigned char* ucKeyValue);
using fnSUNSON_UseEppPlainTextMode10   = int (WINAPI*)(unsigned char ucTextModeFormat, unsigned char* ReturnInfo);
using fnSUNSON_UseEppPlainTextMode31   = int (WINAPI*)(unsigned char ucTextModeFormat, unsigned char AutoEnd, unsigned char* ReturnInfo);

class QPinKeybroad:public QObject
{
    Q_OBJECT
public:
    QPinKeybroad(QObject *parent = nullptr);
    ~QPinKeybroad();

    bool     OpenDevice(QString &strError);
    bool     OpenDevice(string strDev,int nBaud,QString& strError);
    bool     CloseDevice(QString &strError);
    //bool     ReadPin(uchar *pKeyValue);
    bool    m_bDevOpened = false;
    QString  m_strDevPort;
    ushort  m_nBaudreate = 9600;
    HMODULE hMoudle = nullptr;
    string  strModule;
    PinBroadType    nDevType = XZ_F31;
    int     nFailedFlag = 0;
    fnSUNSON_OpenCom                _SUNSON_OpenCom = nullptr;
    fnSUNSON_CloseCom               SUNSON_CloseCom = nullptr;
    fnSUNSON_ScanKeyPress           _SUNSON_ScanKeyPress = nullptr;
    fnSUNSON_UseEppPlainTextMode10  SUNSON_UseEppPlainTextMode10 = nullptr;
    fnSUNSON_UseEppPlainTextMode31  SUNSON_UseEppPlainTextMode31 = nullptr;
    bool SUNSON_UseEppPlainTextMode(unsigned char ucTextModeFormat, unsigned char AutoEnd, unsigned char* ReturnInfo)
    {
        if (strModule.find("XZ_F10") != string::npos)
            return SUNSON_UseEppPlainTextMode10(ucTextModeFormat, ReturnInfo) != nFailedFlag;
        else
            return SUNSON_UseEppPlainTextMode31(ucTextModeFormat, AutoEnd, ReturnInfo) != nFailedFlag;
    }
    bool SUNSON_OpenCom(int nPort, long nBaud)
    {
        return _SUNSON_OpenCom(nPort, nBaud) != nFailedFlag;
    }

    bool SUNSON_ScanKeyPress(unsigned char* ucKeyValue)
    {
        return _SUNSON_ScanKeyPress(ucKeyValue) != nFailedFlag;
    }

signals:
    void InputPin(uchar ch);
};

using QPinKeybroadPtr = shared_ptr<QPinKeybroad>;
#endif // QPINKEYBROAD_H
