#ifndef KT_SSCARDINFO_H
#define KT_SSCARDINFO_H
#include <windows.h>
#include "KT_Define.h"

#define DLL_PUBLIC __declspec(dllexport)

#ifdef __cplusplus
EXTERN_C_START
#endif

//pOutInfo返回: 07--未知省份 08--通讯故障 09--有参数为空

/**
 * @brief 初始化动态库接口
 * @param[in]  inJson json格式字符串需要key值:province/user/pwd/city		枚举传对应整型值
 * @param[out] pOutInfo	返回结果,00成功,01失败
 * @return 0成功,非0失败
 */
DLL_PUBLIC int initCardInfo(const char* inJson, char* pOutInfo);

/**
 * @brief 查询人员数据
 * @param[in/out] info	需要name/cardID,返回的个人信息,旧社保卡号等
 * @param[out] pOutInfo	返回结果,00成功,01失败
 * @return 0成功,非0失败
 */
DLL_PUBLIC int queryPersonInfo(SSCardInfo& info,char *pOutInfo);
/**
 * @brief 制卡进度查询
 * @param[in/out] info	需要name/cardID,返回的个人信息,制卡进度等
* @param[out] pOutInfo	返回结果,00成功,01失败
 * @return 0成功,非0失败
 */
DLL_PUBLIC int queryCardProgress(SSCardInfo& info, char* pOutInfo);
/**
 * @brief 卡状态查询（卡鉴权）
 * @param[in] info	需要name/cardID/cardNum(旧卡号)
* @param[out] pOutInfo	返回:OK卡状态正常,其他中文描述
 * @return 0成功,非0失败
 */
DLL_PUBLIC int queryCardStatus(SSCardInfo& info, char *pOutInfo);
/**
 * @brief 正式挂失
 * @param[in] info	需要name/cardID/cardNum(旧卡号)
 * @param[out] pOutInfo	返回结果,00成功,01已挂失
 * @return 0成功,非0失败
 */
DLL_PUBLIC int reportLostCard(SSCardInfo& info, char *pOutInfo);
/**
 * @brief 解除挂失
 * @param[in] info	需要name/cardID/cardNum(旧卡号)
 * @param[out] pOutInfo	返回结果,00成功,01已挂失
 * @return 0成功,非0失败
 */
DLL_PUBLIC int cancelLostCard(SSCardInfo& info, char* pOutInfo);
/**
 * @brief 查询交费登记状态
 * @param[in] info	需要name/cardID
 * @param[out] pOutInfo	返回结果,00成功,其他失败
 * @return 0成功,非0失败
 */
DLL_PUBLIC int queryPayment(SSCardInfo& info, char* pOutInfo);
/**
 * @brief 注册交费登记
 * @param[in] info	需要name/cardID
 * @param[out] pOutInfo	返回结果,00成功,01已交费
 * @return 0成功,非0失败
 */
DLL_PUBLIC int registerPayment(SSCardInfo& info, char* pOutInfo);
/**
 * @brief 撤销缴费登记
 * @param[in] info	传入的人员信息
 * @param[out] pOutInfo	返回结果,00成功
 * @return 0成功,非0失败
 */
DLL_PUBLIC int cancelPayment(SSCardInfo& info, char* pOutInfo);

/**
 * @brief 新办卡
 * @param[in] info	需要personType/name/cardID/CardNum/OrganID(经办机构)/BankCode/Mobile/validDate/sex/nation/birthday/address/postalCode/photo(base64字符串)
 * @param[out] pOutInfo	返回结果,00成功
 * @return 0成功,非0失败
 */
DLL_PUBLIC int applyNewCard(SSCardInfo& info, char* pOutInfo);

/**
 * @brief 申请补换卡
 * @param[in] info	需要name/cardID/CardNum/OrganID(经办机构)/BankCode/Mobile
 * @param[out] pOutInfo	返回结果,00成功
 * @return 0成功,非0失败
 */
DLL_PUBLIC int applyCardReplacement(SSCardInfo& info, char *pOutInfo );
/**
 * @brief 撤销申请补换卡
 * @param[in] info	需要name/cardID
 * @param[out] pOutInfo	返回结果,00成功
 * @return 0成功,非0失败
 */
DLL_PUBLIC int cancelCardReplacement(SSCardInfo& info, char* pOutInfo);
/**
 * @brief 即制卡标注
 * @param[in] info	需要name/cardID
 * @param[out] pOutInfo	返回结果,00成功,01已标注
 * @return 0成功,非0失败
 */
DLL_PUBLIC int markCard(SSCardInfo& info, char* pOutInfo);
 /**
 * @brief 撤销标注
 * @param[in] info	需要name/cardID
 * @param[out] pOutInfo	返回结果,00成功,01已经是未标注
 * @return 0成功,非0失败
 */
DLL_PUBLIC int cancelMarkCard(SSCardInfo& info, char* pOutInfo);
/**
* @brief 获取制卡人数--1
* @param[in] info	需要cardID/TransType(申请类型)
* @param[out] pOutInfo	返回结果,00成功
* @return 0成功,非0失败
*/
DLL_PUBLIC int getJZKRS(SSCardInfo& info, char* pOutInfo);
/**
* @brief 获取即制卡人员--2
* @param[in] info	需要cardID/TransType(申请类型)
* @param[out] pOutInfo	返回结果,00成功
* @return 0成功,非0失败
*/
DLL_PUBLIC int getJZKRY(SSCardInfo& info, char* pOutInfo);
/**
* @brief 获取批次号--3
* @param[in] info	需要SSQX(所属区县)
* @param[out] pOutInfo	返回结果,00成功
* @return 0成功,非0失败
*/
DLL_PUBLIC int getHQPCH(SSCardInfo& info, char* pOutInfo);
/**
* @brief 获取即制卡批次--4
* @param[in] info	需要cardID/TransType(申请类型)/card(卡商)
* @param[out] pOutInfo	返回结果,00成功
* @return 0成功,非0失败 
*/
DLL_PUBLIC int getJZKPC(SSCardInfo& info, char* pOutInfo);
/**
* @brief 获取制卡数据--5
* @param[in] info	需要BankCode,传出cardNum(新卡号)/sex/nation/birthday/releaseDate/validDate/photo
* @param[out] pOutInfo	返回结果,00成功
* @return 0成功,非0失败
*/
DLL_PUBLIC int getCardData(SSCardInfo& info, char* pOutInfo);
/**
* @brief 制卡回盘
* @param[in] info	传入的人员信息,制卡成功:CardNum,CardID,Name,ReleaseDate,ValidDate,IdentifyNum,CardATR, BankNum,PCH   制卡失败:CardNum,CardID,Name,PCH
* @param[out] pOutInfo	返回结果,00成功
* @return 0成功,非0失败
*/
DLL_PUBLIC int returnCardData(SSCardInfo& info, char* pOutInfo,bool bFailed = false);
/**
* @brief 领卡启用
* @param[in] info	传入的人员信息
* @param[out] pOutInfo	返回结果,00成功
* @return 0成功,非0失败
*/
DLL_PUBLIC int enableCard(SSCardInfo& info, char* pOutInfo);
/**
* @brief 获取CA
* @param[in] info	传入的人员信息
* @param[out] pOutInfo	返回结果,00成功
* @return 0成功,非0失败
*/
DLL_PUBLIC int getCA(const char* user, const char* pwd, const char* city, const char* cardID, const char* cardNum, const char* QMGY, const char* name, const char* SF, CAInfo &info, char* pOutInfo);

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif