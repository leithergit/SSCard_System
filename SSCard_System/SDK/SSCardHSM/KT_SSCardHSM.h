#pragma once
#include <windows.h>
#include <WinSock2.h>
#include "../KT_Define.h"

#define DLL_PUBLIC __declspec(dllexport)

#ifdef __cplusplus
EXTERN_C_START
#endif


/**
 * @brief 初始化库,设置当前环境
 * @param[in] region 地区枚举
 */
	DLL_PUBLIC bool InitEnv(BaseInfo& info);

/**
 * @brief 加密机外部认证
 * @param[in] Info 详见HSMInfo结构体
 * @param[out] pOutInfo 返回信息,成功时返回MAC,失败返回错误信息
 * @return 0成功,非0失败
 */
DLL_PUBLIC int cardExternalAuth(HSMInfo& Info, char* pOutInfo);
/**
 * @brief 内部认证（内外部认证）
 * @param[in] Info 详见HSMInfo结构体
 * @param[out] pOutInfo 返回信息,成功时返回MAC,失败返回错误信息
 * @return 0成功,非0失败
 */
DLL_PUBLIC int InnerAuth(HSMInfo& Info, char* pOutInfo);

#ifdef __cplusplus
EXTERN_C_END
#endif
