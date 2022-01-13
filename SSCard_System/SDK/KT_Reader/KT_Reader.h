#pragma once
#ifndef KT_READER_H
#define KT_READER_H
//#include "pch.h"

enum ReaderBrand
{
	IN_VALID = -1,
	READER_MIN = 1,
	DC_READER = 1,	//德卡读卡器
	MH_READER,		//明华读卡器
	HD_READER,		//华大读卡器
	READER_MAX = HD_READER
};

enum CardPowerType
{
	READER_CONTACT = 1,		//接触式
	READER_UNCONTACT,		//非接式
};

class KT_Reader
{
public:
	KT_Reader() {};
	virtual ~KT_Reader() {};
public:
	/**
	 * @brief 创建读卡器类型,参考枚举Reader
	 * @param[in] ReaderType 读卡器类型
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Reader_Create(ReaderBrand ReaderType, char* resCode) = 0;
	/**
	 * @brief 初始化读卡器端口
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Reader_Init(char* resCode) = 0;
	/**
	 * @brief 读卡器端口退出
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Reader_Exit(char* resCode) = 0;
	/**
	 * @brief 上电操作
	 * @param[in] Type 上电类型,接触or非接
	 * @param[out] CardAtr ATR数据
	 * @param[out] AtrLen  ATR数据长度
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Reader_PowerOn(CardPowerType Type, char* CardAtr, int& AtrLen, char* resCode) = 0;
	/**
	 * @brief 下电操作
	 * @param[in] Type 下电类型,接触or非接
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Reader_PowerOff(CardPowerType Type, char* resCode) = 0;
	/**
	 * @brief 执行命令操作
	 * @param[in] Cmd 发送命令参数
	 * @param[in] CmdLen 发送的命令长度
	 * @param[out] OutData 返回的数据
	 * @param[out] OutLen 返回的数据长度
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Reader_APDU(BYTE* Cmd, int CmdLen, char* OutData, int& OutLen, char* resCode) = 0;
	/**
	 * @brief 读卡器蜂鸣
	 * @param[in] TimeOut 超时时间
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Reader_Beep(int TimeOut, char* resCode) { return 0; }
	/**
	 * @brief 获取读卡器版本信息
	 * @param[out] Version 返回的版本信息
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Reader_GetVersion(char* Version, char* resCode) { return 0; }
};

#endif // !KT_READER_H
