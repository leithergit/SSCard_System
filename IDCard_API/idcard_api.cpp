#pragma execution_character_set("utf-8")
#include "idcard_api.h"
#include "idcardsdk.h"
#include <string.h>
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")

IDCardSDK *g_pIDCardSDK = nullptr;
// 打开读卡器
// szPort 设备所在端口，如COM1,COM2,USB2,USB2等
// 成功时返回0，否则返加相应的错误代码，GetErrorMessage可取得进一步的描述信息
int  OpenReader(IN const char *szPortIn)
{
    if (!szPortIn )
    {
        g_pIDCardSDK = new IDCardSDK();
        if (!g_pIDCardSDK)
            return IDCard_Error_InsufficentMemory;
        return g_pIDCardSDK->OpenDev(-1);
    }
    char szPort[32] = {0};
    strcpy_s(szPort,szPortIn);
    _strupr_s((char *)szPort,strlen(szPort) + 1);
    if (strstr(szPort,"USB"))
    {
        int nUPort = strtol((char *)&szPort[3],nullptr,10);
        if (nUPort < 0)
            return IDCard_Invalid_Parameter;
        g_pIDCardSDK = new IDCardSDK();
        if (!g_pIDCardSDK)
            return IDCard_Error_InsufficentMemory;
        return g_pIDCardSDK->OpenDev(UsbPort1 + nUPort - 1);
    }
    else if (strstr(szPort,"COM"))
    {
        int nSerialPort = strtol((char *)&szPort[3],nullptr,10);
        if (nSerialPort < 0)
            return IDCard_Invalid_Parameter;
        g_pIDCardSDK = new IDCardSDK();
        if (!g_pIDCardSDK)
            return IDCard_Error_InsufficentMemory;
        return g_pIDCardSDK->OpenDev(ComPort1 + nSerialPort - 1);
    }
    else
        return IDCard_Invalid_Parameter;
}

int  GetCurrentPort(OUT char *szPort,int nBufferSize)
{
    if (!g_pIDCardSDK)
        return IDCard_Reader_NotOpen;

    int nPort = g_pIDCardSDK->GetCurrentPort();
    if (nPort >= ComPort1 && nPort <= ComPort16)
    {
        sprintf_s(szPort,nBufferSize,"COM%d",nPort);
    }
    else if (nPort >= UsbPort1 && nPort <= UsbPort16)
    {
        sprintf_s(szPort,nBufferSize,"USB%d",nPort - UsbPort1 + 1);
    }
    return 0;
}

//关闭读卡器
void  CloseReader()
{
    if (!g_pIDCardSDK)
        return;

    g_pIDCardSDK->CloseDev();
    delete  g_pIDCardSDK;
    g_pIDCardSDK = nullptr;

}

// 寻身份证
// 返回值描述参见@OpenReader
int  FindIDCard()
{
    if (!g_pIDCardSDK)
        return IDCard_Reader_NotOpen;
    return g_pIDCardSDK->Authenticate();
}

//读身份证
// 返回值描述参见@OpenReader
int  ReadIDCard(OUT IDCardInfo &CardInfo)
{
    if (!g_pIDCardSDK)
        return IDCard_Reader_NotOpen;
    return g_pIDCardSDK->LoadCardInfo(CardInfo);
}

// 取得返回值或错误代码的具体描述信息
// 返回相信息描述信息的字节长度
int  GetErrorMessage(int nCode,OUT char *szMessage,int IN nBufferSize)
{
    if (!g_pIDCardSDK)
        return IDCard_Reader_NotOpen;
    return g_pIDCardSDK->GetErorrMessage((IDCard_Status)nCode,szMessage,nBufferSize);
}
