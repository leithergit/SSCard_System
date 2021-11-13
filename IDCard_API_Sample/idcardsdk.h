#ifndef IDCARDSDK_H
#define IDCARDSDK_H
#include "../Include/Utility.h"
#include <Windows.h>

using uchar     = unsigned char;
using ushort    = unsigned short;

using fnCVR_InitComm         = int (PASCAL *)(int port);
using fnCVR_CloseComm        = int(PASCAL *)();
using fnCVR_Authenticate     = int(PASCAL *)();
using fnCVR_Read_Content     = int(PASCAL *)();
using fnGetPeopleName        = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetPeopleSex         = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetPeopleNation      = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetPeopleBirthday    = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetPeopleAddress     = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetPeopleIDCode      = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetDepartment        = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetStartDate         = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetEndDate           = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetNationCode        = int(PASCAL *)(uchar *sexData, int * pLen);
using fnGetNationCodeStr     = int(PASCAL *)(uchar *nationData, int *pLen);
using fnCVR_GetSAMID         = int(PASCAL *)(uchar *SAMID);
using fnGetPeopleChineseName = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetPeopleCertVersion = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetNewAppMsg         = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetCertType          = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetBmpData           = int(PASCAL *)(uchar *bmpdate, int * pLen);
using fnGetFPDate            = int(PASCAL *)(uchar *strTmp, int * pLen);
using fnGetPassCheckID       = int(PASCAL *)(uchar *strTmp, int *strLen);
using fnGetIssuesNum         = int(PASCAL *)(int *IssuesNum);


enum SettingItem
{
    AutoPort		= 0,
    ComPort1		= 1,
    ComPort2		= 2,
    ComPort3		= 3,
    ComPort4		= 4,
    ComPort5		= 5,
    ComPort6		= 6,
    ComPort7		= 7,
    ComPort8		= 8,
    ComPort9		= 9,
    ComPort10		= 10,
    ComPort11		= 11,
    ComPort12		= 12,
    ComPort13		= 13,
    ComPort14		= 14,
    ComPort15		= 15,
    ComPort16		= 16,
    UsbPort1		= 1001,
    UsbPort2		= 1002,
    UsbPort3		= 1003,
    UsbPort4		= 1004,
    UsbPort5		= 1005,
    UsbPort6		= 1006,
    UsbPort7		= 1007,
    UsbPort8		= 1008,
    UsbPort9		= 1009,
    UsbPort10		= 1010,
    UsbPort11		= 1011,
    UsbPort12		= 1012,
    UsbPort13		= 1013,
    UsbPort14		= 1014,
    UsbPort15		= 1015,
    UsbPort16		= 1016
};

enum IDCard_Status
{
    IDCard_Succeed  =     0,
    IDCard_Invalid_Parameter,
    IDCard_Library_Notload,
    IDCard_Reader_NotConnected,
    IDCard_Failed_OpenPort,
    IDCard_Failed_Find    ,
    IDCard_Failed_Select  ,
    IDCard_Failed_Read,
    IDCard_Error_Unknow = 99
};

#pragma pack(push,1)
struct _FingerPrint
{
    uchar   szReserver[4];
    uchar   nFlag;                  // 必须为1，否则无效
    uchar   nNumber;                // 指纹序号
    uchar   nQuality;               // 指纹质量
    uchar   szData[505];
};
#pragma pack(pop)

struct IDCardInfo
{
    uchar    szName[32];             // 姓名
    uchar    szGender[8];            // 性别
    uchar    szNation[20];           // 民族
    uchar    szNationCode[20];       // 民族代码
    uchar    szBirthday[24];         // 出生
    uchar    szAddress[80];          // 户籍
    uchar    szIdentify[36];         // 身份证号
    uchar    szIszssueAuthority[32]; // 发证机关
    uchar    szExpirationDate1[16];  // 有效期起始日
    uchar    szExpirationDate2[16];  // 有效期结束日
    uchar    szPhoto[40*1024];       // 照片数据，不超过38862字节
    ushort   nPhotoSize;
    _FingerPrint FingerPrint[2];

    IDCardInfo()
    {
        memset(this,0,sizeof(IDCardInfo));
    }
    void Reset()
    {
        memset(this,0,sizeof(IDCardInfo));
    }
};

#define GetProcAddr(h,x)    (fn##x)GetProcAddress(h,#x)

class IDCardSDK
{
    HINSTANCE   m_hLibHandle = nullptr;
    int         m_nPort = -1;
    int         m_nStatus = 0;
private:
    fnCVR_InitComm          CVR_InitComm				= nullptr;
    fnCVR_CloseComm         CVR_CloseComm				= nullptr;
    fnCVR_Authenticate      CVR_Authenticate			= nullptr;
    fnCVR_Read_Content      CVR_Read_Content			= nullptr;
    fnCVR_GetSAMID          CVR_GetSAMID				= nullptr;
    fnGetPeopleChineseName  GetPeopleChineseName        = nullptr;
    fnGetPeopleCertVersion  GetPeopleCertVersion        = nullptr;
    fnGetPeopleName         GetPeopleName               = nullptr;
    fnGetPeopleSex          GetPeopleSex                = nullptr;
    fnGetPeopleNation       GetPeopleNation             = nullptr;
    fnGetPeopleBirthday     GetPeopleBirthday           = nullptr;
    fnGetPeopleAddress      GetPeopleAddress            = nullptr;
    fnGetPeopleIDCode       GetPeopleIDCode             = nullptr;
    fnGetDepartment         GetDepartment               = nullptr;
    fnGetStartDate          GetStartDate                = nullptr;
    fnGetEndDate            GetEndDate                  = nullptr;
    fnGetNationCode         GetNationCode               = nullptr;
    fnGetNationCodeStr      GetNationCodeStr            = nullptr;
    fnGetNewAppMsg          GetNewAppMsg                = nullptr;
    fnGetCertType           GetCertType                 = nullptr;
    fnGetBmpData            GetBmpData                  = nullptr;
    fnGetFPDate             GetFPDate                   = nullptr;
    fnGetPassCheckID        GetPassCheckID              = nullptr;
    fnGetIssuesNum          GetIssuesNum                = nullptr;
public:
    uchar szSamID[256] = {0};
    IDCardSDK();
    void LoadSDK();
    bool operator()()
    {
        return m_hLibHandle != nullptr;
    }

    int OpenUSB(int nPort = -1)
    {
        if (!m_hLibHandle)
             return IDCard_Library_Notload;
        if (m_nStatus)
        {
            CVR_CloseComm();
            m_nStatus = 0;
        }

        if (nPort != -1)
        {
            m_nStatus = CVR_InitComm(nPort);
        }
        else
        {
            if (m_nPort != -1)
            {
                 m_nStatus = CVR_InitComm(m_nPort);
            }
            else
            {
                for (int nIndex = UsbPort1;nIndex < UsbPort16;nIndex ++)
                {
                    m_nStatus = CVR_InitComm(nIndex);
                    if (m_nStatus == 1)
                    {
                        m_nPort = nIndex;
                        break;
                    }
                }
            }
        }
        if (m_nStatus == 1)
            return IDCard_Succeed;
        else
        {
            switch(m_nStatus)
            {
                case 2:
                    return IDCard_Failed_OpenPort;
                case -2:
                    return IDCard_Library_Notload;
                default:
                case -1:
                    return IDCard_Error_Unknow;
            }
        }
    }

    int OpenSerialport(int nPort = -1)
    {
        if (!m_hLibHandle)
             return IDCard_Library_Notload;
        if (m_nStatus)
        {
            CVR_CloseComm();
            m_nStatus = 0;
        }

        if (nPort != -1)
        {
            m_nStatus = CVR_InitComm(nPort);
        }
        else
        {
            if (m_nPort != -1)
            {
                 m_nStatus = CVR_InitComm(nPort);
            }
            else
            {
                for (int nIndex = ComPort1;nIndex < ComPort16;nIndex ++)
                {
                    m_nStatus = CVR_InitComm(nIndex);
                    if (m_nStatus == 1)
                    {
                        m_nPort = nIndex;
                        break;
                    }
                }
            }
        }
        if (m_nStatus == 1)
            return IDCard_Succeed;
        else
        {
            switch(m_nStatus)
            {
                case 2:
                    return IDCard_Failed_OpenPort;
                case -2:
                    return IDCard_Library_Notload;
                default:
                case -1:
                    return IDCard_Error_Unknow;
            }
        }
    }

    int OpenDev(int nPort = -1)
    {
        int nResult = 0;
        if (nPort == -1)
        {
            nResult = OpenUSB(nPort);
            if (nResult == IDCard_Succeed)
                return nResult;
            else
                return OpenSerialport(nPort);
        }
        else if (nPort >= ComPort1 && nPort <= ComPort16)
            return OpenSerialport(nPort);
        else if (nPort >= UsbPort1 && nPort <= UsbPort16)
            return OpenUSB(nPort);
        else
            return IDCard_Invalid_Parameter;
    }

    int CloseDev()
    {
        if (!m_hLibHandle)
             return IDCard_Library_Notload;
        if (m_nStatus)
        {
            //CVR_CloseComm();
            m_nStatus = 0;
        }
        return IDCard_Succeed;
    }

    int Authenticate()
    {
        int nResult = CVR_Authenticate();
        switch (nResult)
        {
        case 1:
            return IDCard_Succeed;
            break;
        case 2:
            return IDCard_Failed_Find;
            break;
        case 3:
            return IDCard_Failed_Select;
            break;
        case 4:
            return IDCard_Reader_NotConnected;
            break;
        case 0:
            return IDCard_Library_Notload;
            break;
        default:
            return IDCard_Error_Unknow;
            break;
        }
    }

    int LoadCardInfo(IDCardInfo & IDCard)
    {
        int nResult = CVR_Read_Content();
        if (nResult != 1)
        {
            switch (nResult)
            {
                case 0: // 0    错误，读身份证失败
                    return IDCard_Failed_Read;
                case 4: // 4    错误，身份证读卡器未连接
                    return IDCard_Reader_NotConnected;
                case 99:// 动态库未加载
                    return IDCard_Library_Notload;
                default:
                    return IDCard_Error_Unknow;
            }
        }

        int nLen;
        IDCard.Reset();
        nLen = sizeof(IDCard.szName);
        GetPeopleName(IDCard.szName, &nLen);

        nLen = sizeof(IDCard.szGender);
        GetPeopleSex(IDCard.szGender, &nLen);

        nLen = sizeof(IDCard.szNation);
        GetPeopleNation(IDCard.szNation, &nLen);

        nLen = sizeof(IDCard.szBirthday);
        GetPeopleBirthday(IDCard.szBirthday, &nLen);

        nLen = sizeof(IDCard.szBirthday);
        GetPeopleAddress(IDCard.szAddress, &nLen);

        nLen = sizeof(IDCard.szIdentify);
        GetPeopleIDCode(IDCard.szIdentify, &nLen);

        nLen = sizeof(IDCard.szIszssueAuthority);
        GetDepartment(IDCard.szIszssueAuthority, &nLen);

        nLen = sizeof(IDCard.szExpirationDate1);
        GetStartDate(IDCard.szExpirationDate1, &nLen);

        nLen = sizeof(IDCard.szExpirationDate2);
        GetEndDate(IDCard.szExpirationDate2, &nLen);

        nLen = sizeof(IDCard.szNationCode);
        GetNationCode(IDCard.szNationCode, &nLen);

        nLen = sizeof(IDCard.szPhoto);
        GetBmpData(IDCard.szPhoto, (int *)&IDCard.nPhotoSize);

        uchar szFingerData[2048] = {0};
        GetFPDate(szFingerData, &nLen);
        memcpy_s(&IDCard.FingerPrint,sizeof(IDCard.FingerPrint),szFingerData,nLen);
        return IDCard_Succeed;
    }

    int GetErorrMessage(IDCard_Status nCode,char *szMessage,int nBufferSize)
    {
        switch(nCode)
        {
        case IDCard_Succeed:
        {
          strcpy_s(szMessage,nBufferSize,u8"操作成功");
          break;
        }
        case IDCard_Invalid_Parameter:
        {
          strcpy_s(szMessage,nBufferSize,u8"参数无效");
          break;
        }
        case IDCard_Library_Notload:
        {
          //strcpy_s(szMessage,nBufferSize,"The library is not loaded");
            strcpy_s(szMessage,nBufferSize,u8"动态库或相关依赖库未加载");
          break;
        }
        case IDCard_Reader_NotConnected:
        {
          strcpy_s(szMessage,nBufferSize,u8"读卡器未连接");
          break;
        }
        case IDCard_Failed_OpenPort:
        {
          strcpy_s(szMessage,nBufferSize,u8"打开端口失败");
          break;
        }
        case IDCard_Failed_Find:
        {
          strcpy_s(szMessage,nBufferSize,u8"寻卡失败");
          break;
        }
        case IDCard_Failed_Select:
        {
          strcpy_s(szMessage,nBufferSize,u8"选卡失败");
          break;
        }
        case IDCard_Failed_Read:
        {
          strcpy_s(szMessage,nBufferSize,u8"读卡失败");
          break;
        }
        case IDCard_Error_Unknow:
        {
            strcpy_s(szMessage,nBufferSize,u8"未知错误");
            break;
        }
        }
        return strlen(szMessage);
    }
    ~IDCardSDK();
};

#endif // IDCARDSDK_H
