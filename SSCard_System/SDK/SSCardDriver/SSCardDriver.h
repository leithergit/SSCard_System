#pragma once
#include <windows.h>
#include <iostream>

#define DLL_PUBLIC __declspec(dllexport)

#ifdef __cplusplus
EXTERN_C_START
#endif
typedef struct CardInfo
{
	std::string bankNumber;//���п���
	std::string regionCode;//�������
	std::string ATR;//��λ��Ϣ
	std::string algoCode = "03";//�㷨��ʶ
	std::string identifyNum;//��ʶ����
	std::string cardVersion;//��Ƭ�汾
	std::string cardReleaseDate;//��������
	std::string cardValidDate;//����Ч��
	std::string cardNumber;//����
	std::string cardID;//���֤��
	std::string name;//����
	std::string nameExtend;//������չ
	std::string sex;//�Ա�
	std::string nation;//����
	std::string birthday;//��������
}CARDINFO;

/**
 * @brief �籣ģ���ʼ��������
 * @param[in] ReaderHandle ���������
 * @param[out] pOutInfo ���ز���"0000"�ɹ�,����ʧ��
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC long DriverInit(HANDLE ReaderHandle, char *regionCode, char* OLDPIN, char* OLDZKMY,char* pOutInfo);
/**
 * @brief ��������Ϣ
 * @param[in] iType ����������	1-�Ӵ� 2-�ǽ� 3-�Զ�Ѱ��(�Ӵ�����) 4-�Զ�Ѱ��(�ǽ�����)
 * @param[in] cardAtr ���ϵ緵�ص�ATR
 * @param[out] pOutInfo ������Ϣ
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC long iReadCardBas(int iType, char* pOutInfo);
/**
 * @brief д������Ϣ
 * @param[in] iType ����������	1-�Ӵ� 2-�ǽ� 3-�Զ�Ѱ��(�Ӵ�����) 4-�Զ�Ѱ��(�ǽ�����)
 * @param[out] pOutInfo ������Ϣ 
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC long iWriteCardBas(int iType, char* pOutInfo);
/**
 * @brief д������Ϣ���ܻ���һ��
 * @param[in] pKey �ⲿ��֤MAC
 * @param[in] pWriteInfo ��ʶ����|��ʼ����������|��������|����Ч��|����|��ᱣ�Ϻ���|����|������չ|�Ա�|����|��������|
 * @param[out] pOutInfo ������Ϣ
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC int iWriteCardBas_HSM_Step1(char *pKey,char* pWriteInfo, char* pOutInfo);
/**
 * @brief ���ڼ��ܻ��Ķ�������Ϣ ����1
 * @param[in] iType ����������	1-�Ӵ� 2-�ǽ� 3-�Զ�Ѱ��(�Ӵ�����) 4-�Զ�Ѱ��(�ǽ�����)
 * @param[out] pOutInfo ������Ϣ
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC long iReadCardBas_HSM_Step1(char* pOutInfo);
/**
 * @brief ���ڼ��ܻ��Ķ�������Ϣ ����2
 * @param[in] pKey ���ܻ����ص��ڲ���֤���ⲿ��֤����
 * @param[out] pOutInfo ������Ϣ
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC long iReadCardBas_HSM_Step2(char* pKey, char* pOutInfo);
/**
 * @brief ��CA��Կ
 * @param[in] iType ����������	1-�Ӵ� 2-�ǽ� 3-�Զ�Ѱ��(�Ӵ�����) 4-�Զ�Ѱ��(�ǽ�����)
 * @param[out] pOutInfo ������Ϣ
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC long iReadSignatureKey(int iType, char* pOutInfo);
/**
 * @brief дCA��Ϣ
 * @param[in] QMZS��ǩ��֤��
 * @param[in] JMZS������֤��
 * @param[in] JMMY��������Կ
 * @param[in] GLYPIN������Աpin
 * @param[in] ZKMY��������Կ
 * @param[out] pOutInfo ������Ϣ
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC long iWriteCA(int iType, char* QMZS, char* JMZS, char* JMMY, char* GLYPIN, char* ZKMY,char* pOutInfo);
/**
 * @brief ��ȡ���п���
 * @param[in] iType ����������	1-�Ӵ� 2-�ǽ� 3-�Զ�Ѱ��(�Ӵ�����) 4-�Զ�Ѱ��(�ǽ�����)
 * @param[out] pOutInfo ������Ϣ
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC int iReadBankNumber(int iType,char* pOutInfo);
/**
 * @brief ��֤�û�pin
 * @param[in] iType ����������	1-�Ӵ� 2-�ǽ� 3-�Զ�Ѱ��(�Ӵ�����) 4-�Զ�Ѱ��(�ǽ�����)
 * @param[in] pin �û�pin���û����룩
 * @param[out] pOutInfo ������Ϣ
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC int iVerifyPin(int iType, char* pin, char* pOutInfo);
/**
 * @brief �޸��û�pin
 * @param[in] iType ����������	1-�Ӵ� 2-�ǽ� 3-�Զ�Ѱ��(�Ӵ�����) 4-�Զ�Ѱ��(�ǽ�����)
 * @param[in] oldPin ���û�pin���û����룩
 * @param[in] newPin:���û�pin���û���pin��
 * @param[out] pOutInfo ������Ϣ
 * @return 0-�ɹ� 1-ʧ��
 */
DLL_PUBLIC int iChangePin(int iType, char* oldPin, char* newPin, char* pOutInfo);

#ifdef __cplusplus
EXTERN_C_END
#endif
