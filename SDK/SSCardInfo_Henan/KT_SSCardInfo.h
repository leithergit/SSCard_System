#ifndef KT_SSCARDINFO_H
#define KT_SSCARDINFO_H
#include <windows.h>
#include "KT_Define.h"

#define DLL_PUBLIC __declspec(dllexport)

#ifdef __cplusplus
EXTERN_C_START
#endif

//pOutInfo����: 07--δ֪ʡ�� 08--ͨѶ���� 09--�в���Ϊ��

/**
 * @brief ��ʼ����̬��ӿ�
 * @param[in]  inJson json��ʽ�ַ�����Ҫkeyֵ:province/user/pwd/city		ö�ٴ���Ӧ����ֵ
 * @param[out] pOutInfo	���ؽ��,00�ɹ�,01ʧ��
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int initCardInfo(const char* inJson, char* pOutInfo);

/**
 * @brief ��ѯ��Ա����
 * @param[in/out] info	��Ҫname/cardID,���صĸ�����Ϣ,���籣���ŵ�
 * @param[out] pOutInfo	���ؽ��,00�ɹ�,01ʧ��
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int queryPersonInfo(SSCardInfo& info,char *pOutInfo);
/**
 * @brief �ƿ����Ȳ�ѯ
 * @param[in/out] info	��Ҫname/cardID,���صĸ�����Ϣ,�ƿ����ȵ�
* @param[out] pOutInfo	���ؽ��,00�ɹ�,01ʧ��
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int queryCardProgress(SSCardInfo& info, char* pOutInfo);
/**
 * @brief ��״̬��ѯ������Ȩ��
 * @param[in] info	��Ҫname/cardID/cardNum(�ɿ���)
* @param[out] pOutInfo	����:OK��״̬����,������������
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int queryCardStatus(SSCardInfo& info, char *pOutInfo);
/**
 * @brief ��ʽ��ʧ
 * @param[in] info	��Ҫname/cardID/cardNum(�ɿ���)
 * @param[out] pOutInfo	���ؽ��,00�ɹ�,01�ѹ�ʧ
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int reportLostCard(SSCardInfo& info, char *pOutInfo);
/**
 * @brief �����ʧ
 * @param[in] info	��Ҫname/cardID/cardNum(�ɿ���)
 * @param[out] pOutInfo	���ؽ��,00�ɹ�,01�ѹ�ʧ
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int cancelLostCard(SSCardInfo& info, char* pOutInfo);
/**
 * @brief ��ѯ���ѵǼ�״̬
 * @param[in] info	��Ҫname/cardID
 * @param[out] pOutInfo	���ؽ��,00�ɹ�,����ʧ��
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int queryPayment(SSCardInfo& info, char* pOutInfo);
/**
 * @brief ע�ύ�ѵǼ�
 * @param[in] info	��Ҫname/cardID
 * @param[out] pOutInfo	���ؽ��,00�ɹ�,01�ѽ���
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int registerPayment(SSCardInfo& info, char* pOutInfo);
/**
 * @brief �����ɷѵǼ�
 * @param[in] info	�������Ա��Ϣ
 * @param[out] pOutInfo	���ؽ��,00�ɹ�
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int cancelPayment(SSCardInfo& info, char* pOutInfo);

/**
 * @brief �°쿨
 * @param[in] info	��ҪpersonType/name/cardID/CardNum/OrganID(�������)/BankCode/Mobile/validDate/sex/nation/birthday/address/postalCode/photo(base64�ַ���)
 * @param[out] pOutInfo	���ؽ��,00�ɹ�
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int applyNewCard(SSCardInfo& info, char* pOutInfo);

/**
 * @brief ���벹����
 * @param[in] info	��Ҫname/cardID/CardNum/OrganID(�������)/BankCode/Mobile
 * @param[out] pOutInfo	���ؽ��,00�ɹ�
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int applyCardReplacement(SSCardInfo& info, char *pOutInfo );
/**
 * @brief �������벹����
 * @param[in] info	��Ҫname/cardID
 * @param[out] pOutInfo	���ؽ��,00�ɹ�
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int cancelCardReplacement(SSCardInfo& info, char* pOutInfo);
/**
 * @brief ���ƿ���ע
 * @param[in] info	��Ҫname/cardID
 * @param[out] pOutInfo	���ؽ��,00�ɹ�,01�ѱ�ע
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int markCard(SSCardInfo& info, char* pOutInfo);
 /**
 * @brief ������ע
 * @param[in] info	��Ҫname/cardID
 * @param[out] pOutInfo	���ؽ��,00�ɹ�,01�Ѿ���δ��ע
 * @return 0�ɹ�,��0ʧ��
 */
DLL_PUBLIC int cancelMarkCard(SSCardInfo& info, char* pOutInfo);
/**
* @brief ��ȡ�ƿ�����--1
* @param[in] info	��ҪcardID/TransType(��������)
* @param[out] pOutInfo	���ؽ��,00�ɹ�
* @return 0�ɹ�,��0ʧ��
*/
DLL_PUBLIC int getJZKRS(SSCardInfo& info, char* pOutInfo);
/**
* @brief ��ȡ���ƿ���Ա--2
* @param[in] info	��ҪcardID/TransType(��������)
* @param[out] pOutInfo	���ؽ��,00�ɹ�
* @return 0�ɹ�,��0ʧ��
*/
DLL_PUBLIC int getJZKRY(SSCardInfo& info, char* pOutInfo);
/**
* @brief ��ȡ���κ�--3
* @param[in] info	��ҪSSQX(��������)
* @param[out] pOutInfo	���ؽ��,00�ɹ�
* @return 0�ɹ�,��0ʧ��
*/
DLL_PUBLIC int getHQPCH(SSCardInfo& info, char* pOutInfo);
/**
* @brief ��ȡ���ƿ�����--4
* @param[in] info	��ҪcardID/TransType(��������)/card(����)
* @param[out] pOutInfo	���ؽ��,00�ɹ�
* @return 0�ɹ�,��0ʧ�� 
*/
DLL_PUBLIC int getJZKPC(SSCardInfo& info, char* pOutInfo);
/**
* @brief ��ȡ�ƿ�����--5
* @param[in] info	��ҪBankCode,����cardNum(�¿���)/sex/nation/birthday/releaseDate/validDate/photo
* @param[out] pOutInfo	���ؽ��,00�ɹ�
* @return 0�ɹ�,��0ʧ��
*/
DLL_PUBLIC int getCardData(SSCardInfo& info, char* pOutInfo);
/**
* @brief �ƿ�����
* @param[in] info	�������Ա��Ϣ,�ƿ��ɹ�:CardNum,CardID,Name,ReleaseDate,ValidDate,IdentifyNum,CardATR, BankNum,PCH   �ƿ�ʧ��:CardNum,CardID,Name,PCH
* @param[out] pOutInfo	���ؽ��,00�ɹ�
* @return 0�ɹ�,��0ʧ��
*/
DLL_PUBLIC int returnCardData(SSCardInfo& info, char* pOutInfo,bool bFailed = false);
/**
* @brief �쿨����
* @param[in] info	�������Ա��Ϣ
* @param[out] pOutInfo	���ؽ��,00�ɹ�
* @return 0�ɹ�,��0ʧ��
*/
DLL_PUBLIC int enableCard(SSCardInfo& info, char* pOutInfo);
/**
* @brief ��ȡCA
* @param[in] info	�������Ա��Ϣ
* @param[out] pOutInfo	���ؽ��,00�ɹ�
* @return 0�ɹ�,��0ʧ��
*/
DLL_PUBLIC int getCA(const char* user, const char* pwd, const char* city, const char* cardID, const char* cardNum, const char* QMGY, const char* name, const char* SF, CAInfo &info, char* pOutInfo);

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif