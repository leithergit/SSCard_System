#ifndef IDCARDSDK_H
#define IDCARDSDK_H
#include "../utility/Utility.h"
#include <Windows.h>
#include "idcard_api.h"

using fnCVR_InitComm         = int (PASCAL *)(int port);
using fnCVR_CloseComm        = int(PASCAL *)();
using fnCVR_Authenticate     = int(PASCAL *)();
using fnCVR_Read_Content     = int(PASCAL *)();
using fnGetPeopleName        = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetPeopleSex         = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetPeopleNation      = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetPeopleBirthday    = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetPeopleAddress     = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetPeopleIDCode      = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetDepartment        = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetStartDate         = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetEndDate           = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetNationCode        = int(PASCAL *)(unsigned char  *sexData, int * pLen);
using fnGetNationCodeStr     = int(PASCAL *)(unsigned char  *nationData, int *pLen);
using fnCVR_GetSAMID         = int(PASCAL *)(unsigned char  *SAMID);
using fnGetPeopleChineseName = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetPeopleCertVersion = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetNewAppMsg         = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetCertType          = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetBmpData           = int(PASCAL *)(unsigned char  *bmpdate, int * pLen);
using fnGetFPDate            = int(PASCAL *)(unsigned char  *strTmp, int * pLen);
using fnGetPassCheckID       = int(PASCAL *)(unsigned char  *strTmp, int *strLen);
using fnGetIssuesNum         = int(PASCAL *)(int *IssuesNum);


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
    IDCardSDK();
    ~IDCardSDK();
    void LoadSDK();
    bool operator()()
    {
        return m_hLibHandle != nullptr;
    }

    int OpenUSB(int nPort = -1);

    int OpenSerialport(int nPort = -1);

    int OpenDev(int nPort = -1);

    int CloseDev();

    int Authenticate();

    int LoadCardInfo(IDCardInfo & IDCard);

    int GetErorrMessage(IDCard_Status nCode,char *szMessage,int nBufferSize);

};

#endif // IDCARDSDK_H
