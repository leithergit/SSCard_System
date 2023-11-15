#pragma once
#include <windows.h>
#include "..\..\..\SDK.git\sd_sscardinfo\SD_SSCardInfo\SD_SSCardInfo\KT_DEFINE.h"

#define DLL_PUBLIC __declspec(dllexport)

#ifdef __cplusplus
EXTERN_C_START
#endif

//返回值: 8--通讯故障 9--有参数为空

/**
* @brief 初始化卡管信息
* @param[in] inJson url(需要加上port)/licenseKey/serviceName
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
	DLL_PUBLIC int initCardInfo(const char* inJson, std::string& pOutInfo);

/**
* @brief 申领卡信息校验
* @param[in] info 需要cardID,name,city,bankcode
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int chkCanCardSq(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 保存申领名单信息
* @param[in] info 需要cardID,cardType,name,city,bankcode,mobile,operator,OccupType,releaseDate,validDate,Birthday,sex,nation,address,photo
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardSqList(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 保存申领名单信息(未成年新办卡)
* @param[in] info 需要cardID,cardType,name,city,bankcode,mobile,operator,releaseDate,validDate,Birthday,sex,nation,address,GuardianCardType,GuardianCardID,GuardianName,GuardianType,photo
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardSqListByGuardian(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 查询申领名单信息
* @param[in] info 需要city,bankcode,validDate,dealType,releaseDate
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int queryCardZksqList(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 查询社保余额
* @param[in] info 需要cardID,cardType,name,city,cardNum,chipNum
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int queryCardAccountBalance(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 即时制卡预开户
* @param[in] info 需要cardID,cardType,name,bankCode,operator,city
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */

DLL_PUBLIC int saveCardOpen(SD_SSCardInfo& info, std::string& pOutInfo);
/**
* @brief 即时制卡预开户换卡，输入info，下为需要的info信息
* @param[in]  info.cardID			有效证件号码
* @param[in]  info.cardType			有效证件类型
* @param[in]  info.name				姓名
* @param[in]  info.systemType		系统类别 默认0
* @param[in]  info.operator			经办人
* @param[in]  info.city				行政区划代码
* @param[in]  info.bankCode			银行编号
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardOpenHk(SD_SSCardInfo& info, std::string& pOutInfo);


/**
* @brief 制卡完成
* @param[in] info 需要cardID,cardType,name,bankCode,operator,city,cardNum,chipNum,MagNum,ATR,identifyNum,cardVersion,chipType
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardCompleted(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 保存卡片交接到个人
* @param[in] info 需要CardId,CardNum,Name,BankNodeNum,InterId,InterName,InterMobile,city,systemType,Operator
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardHandToGr(SD_SSCardInfo& info, std::string& pOutInfo);
/**
* @brief 查询领卡点信息
* @param[in] info 需要city,systemType,bankCode
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int queryLkdInfoLov(SD_SSCardInfo& info, std::string& pOutInfo);
/**
* @brief 社保卡启用
* @param[in] info 需要cardID,cardType,name,cardNum,bankCode,operator,city
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardActive(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 保存社保卡金融账号激活记录
* @param[in] info 需要cardID,cardType,name,magNum,bankCode,operator,city
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardJrzhActive(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 查询社保卡信息
* @param[in] info 需要cardID,cardType,name,city
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int queryCardInfoBySfzhm(SD_SSCardInfo& info, std::string& pOutInfo, string strStatustoQuery);

/**
* @brief 社保卡临时挂失
* @param[in] info 需要cardID,cardType,name,cardNum,Operator,city
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardLsgs(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 临时挂失解挂
* @param[in] info 需要cardID,cardType,name,cardNum,city
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardLsgsjg(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 社保卡挂失
* @param[in] info 需要cardID,cardType,name,cardNum,city,cardNum,operator,bankcode
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardGs(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 查询个人照片信息
* @param[in] info 需要cardID,cardType,city,name
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int queryPerPhoto(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 修改个人基本信息
* @param[in] info 需要cardID,cardType,name(前三项无法修改),sex,birthday, nation,address,mobile,operator,city,releaseDate,validDate,cardUnit,postcal,birthplace,PerAddr,RegAttr,Education,Marital,ContactsName,ContactsMobile
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int modifyPersonInfo(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 查询个人基本信息
* @param[in] info 需要cardID,cardType,name,city
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int queryPersonInfo(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 社保卡补换卡校验
* @param[in] info 需要cardID,cardType,name,bankCode,city
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int chkCanCardBh(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 保存补换卡名单信息(监护人代申请,需填监护人信息)
* @param[in] info 需要cardID,cardType,name,bankCode,city,mobile,reason,operator,OccupType,birthday,sex,nation,address,photo,GuardianName,GuardianCardType,GuardianCardID,GuardianType
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardBhListByGuardian(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 保存补换卡名单信息
* @param[in] info 需要cardID,cardType,name,bankCode,city,mobile,reason,operator,OccupType,birthday,sex,nation,address,photo
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardBhList(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 挂失补换卡核心接口
* @param[in] info 需要cardID,cardType,name,bankCode,city,cardNum,operator
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardBhk(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 保存更换银行换卡名单信息,输入info，下为需要的info信息
* @param[in]  systemType		系统类别
* @param[in]  cardID			有效证件号码
* @param[in]  cardType			有效证件类型
* @param[in]  name				姓名
* @param[in]  cardNum			卡号
* @param[in]  bankCode			新申请银行编号
* @param[in]  oldBankCode		老银行编号
* @param[in]  oldMagNum			老磁条账号
* @param[in]  oldChipNum		老卡芯片号
* @param[in]  getMethod			获取方式
* @param[in]  operator			经办人
* @param[in]  city				行政区划代码
* @param[in]  phoneNum			手机号
* @param[in]  OccupType			职业类别
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int saveCardHkGhyhList(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 获取CA
* @param[in]  name  姓名
* @param[in]  cardID  身份证号
* @param[in]  cardNum  卡号
* @param[in]  signatureKey  签名公钥
* @param[in]  SF  算法
* @param[in]  city  行政区划代码
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int getCA(std::string name, std::string cardID, std::string cardNum, std::string signatureKey, std::string SF, std::string city, std::string& pOutInfo);

/**
* @brief 校验授权信息
* @param[in] info 需要cardID,cardType,bankCode,city
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int checkAuthorizeInfo(SD_SSCardInfo& info, std::string& pOutInfo);

/**
* @brief 生成授权信息
* @param[in] info 需要cardID,cardType,bankCode,city,AuthorizeType,AuthorizeTime,AuthorizeData
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int makeAuthorizeInfo(SD_SSCardInfo& info, std::string& pOutInfo);


/**
* @brief 修改个人基本信息
* @param[in] info 需要cardID,cardType,name(前三项无法修改),sex,birthday, nation,address,mobile,operator,city,releaseDate,validDate,cardUnit,postcal,birthplace,PerAddr,RegAttr,Education,Marital,ContactsName,ContactsMobile
* @param[out] pOutInfo 返回json字符串
* @return 0:成功 1:失败
 */
DLL_PUBLIC int modifyPersonInfoByGuardian(SD_SSCardInfo& info, std::string& pOutInfo);

#ifdef __cplusplus
EXTERN_C_END
#endif
