#include "idcardsdk.h"
#include <Shlwapi.h>

IDCardSDK::IDCardSDK()
{
    char szAppPath[1024] = {0};
    GetAppPathA(szAppPath,1024);
    strcat_s(szAppPath,1024,"\\termb.dll");
    if (!::PathFileExistsA(szAppPath))
    {
        return ;
    }
    m_hLibHandle = LoadLibraryA(szAppPath);
    if (!m_hLibHandle)
        return ;
    LoadSDK();
}

IDCardSDK::~IDCardSDK()
{
    if (m_hLibHandle)
    {
        FreeLibrary(m_hLibHandle);
        m_hLibHandle = nullptr;
    }
}

void IDCardSDK::LoadSDK()
{
    if (m_hLibHandle != NULL)
    {
//        CVR_InitComm            = GetProcAddr(m_hLibHandle,CVR_InitComm         );
//        CVR_CloseComm           = GetProcAddr(m_hLibHandle,CVR_CloseComm        );
//        CVR_Authenticate        = GetProcAddr(m_hLibHandle,CVR_Authenticate     );
//        CVR_Read_Content        = GetProcAddr(m_hLibHandle,CVR_Read_Content     );
//        CVR_GetSAMID            = GetProcAddr(m_hLibHandle,CVR_GetSAMID         );
//        GetFPDate               = GetProcAddr(m_hLibHandle,GetFPDate            );
//        GetBmpData				= GetProcAddr(m_hLibHandle,GetBmpData			);
//        GetPeopleName			= GetProcAddr(m_hLibHandle,GetPeopleName		);
//        GetPeopleSex			= GetProcAddr(m_hLibHandle,GetPeopleSex		    );
//        GetPeopleNation			= GetProcAddr(m_hLibHandle,GetPeopleNation		);
//        GetPeopleBirthday		= GetProcAddr(m_hLibHandle,GetPeopleBirthday	);
//        GetPeopleAddress		= GetProcAddr(m_hLibHandle,GetPeopleAddress	    );
//        GetPeopleIDCode			= GetProcAddr(m_hLibHandle,GetPeopleIDCode		);
//        GetDepartment			= GetProcAddr(m_hLibHandle,GetDepartment		);
//        GetStartDate			= GetProcAddr(m_hLibHandle,GetStartDate		    );
//        GetEndDate				= GetProcAddr(m_hLibHandle,GetEndDate			);
//        GetPeopleChineseName	= GetProcAddr(m_hLibHandle,GetPeopleChineseName );
//        GetPeopleCertVersion	= GetProcAddr(m_hLibHandle,GetPeopleCertVersion );
//        GetNationCode			= GetProcAddr(m_hLibHandle,GetNationCode		);
//        GetNewAppMsg            = GetProcAddr(m_hLibHandle,GetNewAppMsg         );
//        GetCertType 			= GetProcAddr(m_hLibHandle,GetCertType 		    );
//        GetNationCode			= GetProcAddr(m_hLibHandle,GetNationCode		);
//        GetPassCheckID			= GetProcAddr(m_hLibHandle,GetPassCheckID		);
//        GetIssuesNum			= GetProcAddr(m_hLibHandle,GetIssuesNum		    );
        CVR_InitComm            = (fnCVR_InitComm)GetProcAddress(m_hLibHandle,"CVR_InitComm");
        CVR_CloseComm           = (fnCVR_CloseComm)GetProcAddress(m_hLibHandle,"CVR_CloseComm");
        CVR_Authenticate        = (fnCVR_Authenticate)GetProcAddress(m_hLibHandle,"CVR_Authenticate");
        CVR_Read_Content        = (fnCVR_Read_Content)GetProcAddress(m_hLibHandle,"CVR_Read_FPContent");
        CVR_GetSAMID            = (fnCVR_GetSAMID)GetProcAddress(m_hLibHandle, "CVR_GetSAMID");		////
        GetFPDate               = (fnGetFPDate)GetProcAddress(m_hLibHandle,"GetFPDate");
        GetBmpData				= (fnGetBmpData)GetProcAddress(m_hLibHandle,"GetBMPData");
        GetPeopleName			= (fnGetPeopleName)GetProcAddress(m_hLibHandle,"GetPeopleName");
        GetPeopleSex			= (fnGetPeopleSex)GetProcAddress(m_hLibHandle,"GetPeopleSex");
        GetPeopleNation			= (fnGetPeopleNation)GetProcAddress(m_hLibHandle,"GetPeopleNation");
        GetPeopleBirthday		= (fnGetPeopleBirthday)GetProcAddress(m_hLibHandle,"GetPeopleBirthday");
        GetPeopleAddress		= (fnGetPeopleAddress)GetProcAddress(m_hLibHandle,"GetPeopleAddress");
        GetPeopleIDCode			= (fnGetPeopleIDCode)GetProcAddress(m_hLibHandle,"GetPeopleIDCode");
        GetDepartment			= (fnGetDepartment)GetProcAddress(m_hLibHandle,"GetDepartment");
        GetStartDate			= (fnGetStartDate)GetProcAddress(m_hLibHandle,"GetStartDate");
        GetEndDate				= (fnGetEndDate)GetProcAddress(m_hLibHandle,"GetEndDate");
        GetPeopleChineseName	= (fnGetPeopleChineseName)GetProcAddress(m_hLibHandle,"GetPeopleChineseName");
        GetPeopleCertVersion	= (fnGetPeopleCertVersion)GetProcAddress(m_hLibHandle,"GetPeopleCertVersion");
        GetNationCode			= (fnGetNationCode)GetProcAddress(m_hLibHandle,"GetNationCode");
        GetNewAppMsg    		= (fnGetNewAppMsg)GetProcAddress(m_hLibHandle, "GetNewAppMsg");
        GetCertType 			= (fnGetCertType)GetProcAddress(m_hLibHandle, "GetCertType");
        GetNationCode			= (fnGetNationCode)GetProcAddress(m_hLibHandle, "GetNationCode");
        GetPassCheckID			= (fnGetPassCheckID)GetProcAddress(m_hLibHandle,"GetPassCheckID");
        GetIssuesNum			= (fnGetIssuesNum)GetProcAddress(m_hLibHandle,"GetIssuesNum");

    }
}
