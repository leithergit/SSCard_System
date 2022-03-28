#pragma once
#include <windows.h>
#include "KT_Define.h"

#define DLL_PUBLIC __declspec(dllexport)

#ifdef __cplusplus
EXTERN_C_START
#endif

//����ֵ: 8--ͨѶ���� 9--�в���Ϊ��

/**
* @brief ��ʼ��������Ϣ
* @param[in] inJson url(��Ҫ����port)/licenseKey/serviceName
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int initCardInfo(const char* inJson, std::string& pOutInfo);

/**
* @brief ���쿨��ϢУ��
* @param[in] info ��ҪcardID,name,city,bankcode
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int chkCanCardSq(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief ��������������Ϣ
* @param[in] info ��ҪcardID,cardType,name,city,bankcode,mobile,operator,OccupType,releaseDate,validDate,Birthday,sex,nation,address,photo
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardSqList(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief ��������������Ϣ(δ�����°쿨)
* @param[in] info ��ҪcardID,cardType,name,city,bankcode,mobile,operator,releaseDate,validDate,Birthday,sex,nation,address,GuardianCardType,GuardianCardID,GuardianName,GuardianType,photo
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardSqListWithChild(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief ��ѯ����������Ϣ
* @param[in] info ��Ҫcity,bankcode,validDate,dealType,releaseDate
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int queryCardZksqList(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief ��ʱ�ƿ�Ԥ����
* @param[in] info ��ҪcardID,cardType,name,bankCode,operator,city
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardOpen(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief �ƿ����
* @param[in] info ��ҪcardID,cardType,name,bankCode,operator,city,cardNum,chipNum,MagNum,ATR,identifyNum,cardVersion,chipType
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardCompleted(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief �籣������
* @param[in] info ��ҪcardID,cardType,name,cardNum,bankCode,operator,city
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardActive(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief �����籣�������˺ż����¼
* @param[in] info ��ҪcardID,cardType,name,magNum,bankCode,operator,city
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardJrzhActive(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief ��ѯ�籣����Ϣ
* @param[in] info ��ҪcardID,cardType,name,city
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int queryCardInfoBySfzhm(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief �籣����ʱ��ʧ
* @param[in] info ��ҪcardID,cardType,name,cardNum,Operator,city
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardLsgs(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief ��ʱ��ʧ���
* @param[in] info ��ҪcardID,cardType,name,cardNum,city
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardLsgsjg(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief �籣����ʧ
* @param[in] info ��ҪcardID,cardType,name,cardNum,city,cardNum,operator,bankcode
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardGs(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief ��ѯ������Ƭ��Ϣ
* @param[in] info ��ҪcardID,cardType,city,name
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int queryPerPhoto(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief �޸ĸ��˻�����Ϣ
* @param[in] info ��ҪcardID,cardType,name(ǰ�����޷��޸�),sex,birthday, nation,address,mobile,operator,city,releaseDate,validDate,cardUnit,postcal,birthplace,PerAddr,RegAttr,Education,Marital,ContactsName,ContactsMobile
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int modifyPersonInfo(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief ��ѯ���˻�����Ϣ
* @param[in] info ��ҪcardID,cardType,name,city
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int queryPersonInfo(SD_SSCardInfo& info,std::string& pOutInfo);

/**
* @brief �籣��������У��
* @param[in] info ��ҪcardID,cardType,name,bankCode,city
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int chkCanCardBh(SD_SSCardInfo& info,std::string& pOutInfo);

/**
* @brief ���油����������Ϣ
* @param[in] info ��ҪcardID,cardType,name,bankCode,city,mobile,reason,operator,OccupType,birthday,sex,nation,address,photo
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardBhList(SD_SSCardInfo& info,std::string& pOutInfo);

/**
* @brief ��ʧ���������Ľӿ�
* @param[in] info ��ҪcardID,cardType,name,bankCode,city,cardNum,operator
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int saveCardBhk(SD_SSCardInfo& info,std::string& pOutInfo);

/**
* @brief ��ȡCA
* @param[in]  name  ����
* @param[in]  cardID  ���֤��
* @param[in]  cardNum  ����
* @param[in]  signatureKey  ǩ����Կ
* @param[in]  SF  �㷨
* @param[in]  city  ������������
* @param[out] pOutInfo ����json�ַ���
* @return 0:�ɹ� 1:ʧ��
 */
DLL_PUBLIC int getCA(std::string name, std::string cardID, std::string cardNum, std::string signatureKey, std::string SF, std::string city,std::string& pOutInfo);

#ifdef __cplusplus
EXTERN_C_END
#endif