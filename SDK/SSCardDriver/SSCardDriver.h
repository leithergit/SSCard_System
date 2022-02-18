#pragma once
#include <windows.h>
#include <iostream>

#define DLL_PUBLIC __declspec(dllexport)

#ifdef __cplusplus
EXTERN_C_START
#endif
typedef struct CardInfo
{
	std::string bankNumber;//银行卡号
	std::string regionCode;//区域代码
	std::string ATR;//复位信息
	std::string algoCode = "03";//算法标识
	std::string identifyNum;//卡识别码
	std::string cardVersion;//卡片版本
	std::string cardReleaseDate;//发卡日期
	std::string cardValidDate;//卡有效期
	std::string cardNumber;//卡号
	std::string cardID;//身份证号
	std::string name;//姓名
	std::string nameExtend;//姓名拓展
	std::string sex;//性别
	std::string nation;//名族
	std::string birthday;//出生日期
}CARDINFO;

/**
 * @brief 社保模块初始化读卡器
 * @param[in] ReaderHandle 读卡器句柄
 * @param[out] pOutInfo 返回参数"0000"成功,其他失败
 * @return 0-成功 1-失败
 */
DLL_PUBLIC long DriverInit(HANDLE ReaderHandle, char *regionCode, char* OLDPIN, char* OLDZKMY,char* pOutInfo);
/**
 * @brief 读基本信息
 * @param[in] iType 卡操作类型	1-接触 2-非接 3-自动寻卡(接触优先) 4-自动寻卡(非接优先)
 * @param[in] cardAtr 卡上电返回的ATR
 * @param[out] pOutInfo 传出信息
 * @return 0-成功 1-失败
 */
DLL_PUBLIC long iReadCardBas(int iType, char* pOutInfo);
/**
 * @brief 写基本信息
 * @param[in] iType 卡操作类型	1-接触 2-非接 3-自动寻卡(接触优先) 4-自动寻卡(非接优先)
 * @param[out] pOutInfo 传出信息 
 * @return 0-成功 1-失败
 */
DLL_PUBLIC long iWriteCardBas(int iType, char* pOutInfo);
/**
 * @brief 写基本信息加密机第一步
 * @param[in] pKey 外部认证MAC
 * @param[in] pWriteInfo 卡识别码|初始化机构编码|发卡日期|卡有效期|卡号|社会保障号码|姓名|姓名拓展|性别|民族|出生日期|
 * @param[out] pOutInfo 传出信息
 * @return 0-成功 1-失败
 */
DLL_PUBLIC int iWriteCardBas_HSM_Step1(char *pKey,char* pWriteInfo, char* pOutInfo);
/**
 * @brief 基于加密机的读基本信息 步骤1
 * @param[in] iType 卡操作类型	1-接触 2-非接 3-自动寻卡(接触优先) 4-自动寻卡(非接优先)
 * @param[out] pOutInfo 传出信息
 * @return 0-成功 1-失败
 */
DLL_PUBLIC long iReadCardBas_HSM_Step1(char* pOutInfo);
/**
 * @brief 基于加密机的读基本信息 步骤2
 * @param[in] pKey 加密机返回的内部认证和外部认证数据
 * @param[out] pOutInfo 传出信息
 * @return 0-成功 1-失败
 */
DLL_PUBLIC long iReadCardBas_HSM_Step2(char* pKey, char* pOutInfo);
/**
 * @brief 读CA公钥
 * @param[in] iType 卡操作类型	1-接触 2-非接 3-自动寻卡(接触优先) 4-自动寻卡(非接优先)
 * @param[out] pOutInfo 传出信息
 * @return 0-成功 1-失败
 */
DLL_PUBLIC long iReadSignatureKey(int iType, char* pOutInfo);
/**
 * @brief 写CA信息
 * @param[in] QMZS：签名证书
 * @param[in] JMZS：加密证书
 * @param[in] JMMY：加密密钥
 * @param[in] GLYPIN：管理员pin
 * @param[in] ZKMY：主控密钥
 * @param[out] pOutInfo 传出信息
 * @return 0-成功 1-失败
 */
DLL_PUBLIC long iWriteCA(int iType, char* QMZS, char* JMZS, char* JMMY, char* GLYPIN, char* ZKMY,char* pOutInfo);
/**
 * @brief 读取银行卡号
 * @param[in] iType 卡操作类型	1-接触 2-非接 3-自动寻卡(接触优先) 4-自动寻卡(非接优先)
 * @param[out] pOutInfo 传出信息
 * @return 0-成功 1-失败
 */
DLL_PUBLIC int iReadBankNumber(int iType,char* pOutInfo);
/**
 * @brief 验证用户pin
 * @param[in] iType 卡操作类型	1-接触 2-非接 3-自动寻卡(接触优先) 4-自动寻卡(非接优先)
 * @param[in] pin 用户pin（用户密码）
 * @param[out] pOutInfo 传出信息
 * @return 0-成功 1-失败
 */
DLL_PUBLIC int iVerifyPin(int iType, char* pin, char* pOutInfo);
/**
 * @brief 修改用户pin
 * @param[in] iType 卡操作类型	1-接触 2-非接 3-自动寻卡(接触优先) 4-自动寻卡(非接优先)
 * @param[in] oldPin 老用户pin（用户密码）
 * @param[in] newPin:新用户pin（用户新pin）
 * @param[out] pOutInfo 传出信息
 * @return 0-成功 1-失败
 */
DLL_PUBLIC int iChangePin(int iType, char* oldPin, char* newPin, char* pOutInfo);

#ifdef __cplusplus
EXTERN_C_END
#endif
