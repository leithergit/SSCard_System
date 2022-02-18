#pragma once
#include <windows.h>
#include <WinSock2.h>
#include "KT_Define.h"

#define DLL_PUBLIC __declspec(dllexport)

#ifdef __cplusplus
EXTERN_C_START
#endif


/**
 * @brief ��ʼ����,���õ�ǰ����
 * @param[in] region ����ö��
 */
DLL_PUBLIC bool InitEnv(BaseInfo& info);

/**
 * @brief ���ܻ��ⲿ��֤
 * @param[in] Info ���HSMInfo�ṹ��
 * @param[out] pOutInfo ������Ϣ,�ɹ�ʱ����MAC,ʧ�ܷ��ش�����Ϣ
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int cardExternalAuth(HSMInfo& Info, char* pOutInfo);
/**
 * @brief �ڲ���֤�����ⲿ��֤��
 * @param[in] Info ���HSMInfo�ṹ��
 * @param[out] pOutInfo ������Ϣ,�ɹ�ʱ����MAC,ʧ�ܷ��ش�����Ϣ
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int InnerAuth(HSMInfo& Info, char* pOutInfo);

#ifdef __cplusplus
EXTERN_C_END
#endif