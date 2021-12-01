/**
 * @file        KT_Printer.h
 * @author      Pointer
 * @version     10/26/2021
 */
#pragma once
#ifndef _KT_PRINT_H_
#define _KT_PRINT_H_
#include <windows.h>

#ifndef var_UNUSED
#define var_UNUSED(x) (void)x;
#endif

namespace Kingaotech
{
	typedef enum class Printer
	{
		EVOLIS_KC200 = 1,
		EVOLIS_ZENIUS,
		EVOLIS_AVANSIA,
		HITI_CS200,
		HITI_CS220,
		HITI_CS290,
		ENTRUCT_EM1,
		ENTRUCT_EM2,
		ENTRUCT_CD809,
	}PT;

	typedef struct _PRINTERBOX
	{
		int Type;			//卡箱类型 0:未知，1：发卡箱，2：回收箱
		int BoxNumber;		//卡箱号
		int BoxStatus;		//卡箱状态,0：正常; 1：卡少; 2:无卡; 3：高(快满)；4：满；
	}PRINTERBOXUNIT, * LPPRINTERBOXUNIT;

	typedef struct BOXINFO
	{
		int nCount;				//卡箱总个数(所有卡箱)
		LPPRINTERBOXUNIT BoxList;		//卡箱信息结构体指针
	}LBOXINFO, * LPBOXINFO;

	typedef struct _PRINTERSTATUS
	{
		WORD fwDevice;			//打印机状态, 0-Ready；1-Busy；2-Offline；3-ErrMachine；4-Printing
		WORD fwMedia;			//介质状态，0-无卡；1-卡在门口；2-卡在内部；3-卡在上电位，4-卡在闸门外；5-堵卡；6-卡片未知（根据硬件特性返回,必须支持有无卡检测）
		WORD fwToner;			//平印色带状态,0-FLLL;1-LOW;2-OUT;3-NOTSUPP;4-UNKNOW
	}PRINTERSTATUS, * LPPRINTERSTATUS;
}

class KT_Printer
{
public:
	KT_Printer() {};
	virtual ~KT_Printer() {};

public:
	/**
	 * @brief 初始化打印机
	 * @param[in] PrinterType 打印机类型   Ex: Evolis kc200
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_Init(Kingaotech::PT PrinterType, char* resCode) = 0;
	/**
	 * @brief 打开打印机
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_Open(char* resCode) = 0;
	/**
	 * @brief 关闭打印机
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_Close(char* resCode) = 0;
	/**
	 * @brief 打印机复位
	 * @param[in] Action 1-复位卡保持在设备内；2-复位退卡到打印机取卡位置；3-复位吞卡到打印机回收箱(若硬件支持需实现)
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_Reset(int Action, char* resCode) = 0;
	/**
	 * @brief 获取动态库版本信息
	 * @param[out] VersionInfo 动态库版本信息
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_GetVersion(char* VersionInfo, char* resCode) = 0;
	/**
	 * @brief 打印机发卡
	 * @prarm[in] BoxNo 卡箱号
	 * @param[in] CardPos 发卡位置 1-读磁位；2-接触IC位;3-非接IC位;4-打印位
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_Dispense(int BoxNo, int CardPos, char* resCode) = 0;
	/**
	 * @brief 退卡到出卡口
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_Eject(char* resCode) = 0;
	/**
	 * @brief 打印机回收卡
	 * @param BoxNo 回收卡卡箱
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_Retract(int BoxNo, char* resCode) = 0;
	/**
	 * @brief 获取卡箱状态
	 * @param[out] lpBoxInfo 返回卡箱的结构体,见定义
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_BoxStatus(Kingaotech::LPBOXINFO& lpBoxInfo, char* resCode) = 0;
	/**
	 * @brief 获取打印机状态
	 * @param[out] lpStatus 返回打印机状态的结构体,见定义
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_Status(Kingaotech::LPPRINTERSTATUS& lpStatus, char* resCode) = 0;
	/**
	 * @brief 打印初始化
	 * @param[in] SetParam 打印机参数 可为NULL
	 * @param[out] resCode 返回4位数返回值,成功返回 "0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_InitPrint(char* SetParam, char* resCode) = 0;
	/**
	 * @brief 初始化图片信息,如有多张,多次调用
	 *@param[in] ImgPath 图片全路径
	 * @param[in] Angle 图片顺时针旋转角度
	 * @param[in] Xpos 打印X坐标
	 * @param[in] Ypos 打印Y坐标
	 * @param[in] ImgHeight 打印高度
	 * @param[in] ImgWidth 打印宽度
	 * @param[out] resCode失败时返回4位错误码，成功时返回"0000"
	 * @return 0:成功 1:失败
	 */
	virtual int Printer_AddImage(char* ImgPath, int Angle, float Xpos, float Ypos, float ImgHeight, float ImgWidth, char* resCode) = 0;
	/**
	 * @brief  预打印文字,如有多条文字,多次调用
	 * @param[in] Text 打印的内容
	 * @param[in] Angle 顺时针旋转角度,用于打印水平或竖直文字
	 * @param[in] Xpos 打印X坐标
	 * @param[in] Ypos 打印Y坐标
	 * @param[in] FontName 打印字体,例如"宋体"
	 * @param[in] FontSize 字体大小
	 * @param[in] FontStyle 字体风格：1-常规；2-粗体；4-斜体；8-黑体
	 * @param[in] TextColor 打印字体颜色,RGB颜色值
	 * @param[out] resRcCode 失败时返回4位错误码，成功时返回"0000"
	 * @return 0：成功；1：失败
	 */
	virtual int Printer_AddText(char* Text, int Angle, float Xpos, float Ypos, char* FontName, int FontSize, int FontStyle, int TextColor, char* resRcCode) = 0;
	/**
	 * @brief  提交任务,开始打印
	 * @param[out] resCode 失败时返回4位错误码，成功时返回"0000"
	 * @return 0：成功；1：失败
	 */
	virtual int Printer_StartPrint(char* resCode) = 0;
	/**
	 * @brief  翻转卡片
	 * @param[in] nAngle 旋转角度,一般只支持180度翻转卡片
	 * @param[out] resCode 失败时返回4位错误码，成功时返回"0000"
	 * @return 0：成功；1：失败
	 */
	virtual int Printer_Rotate(int Angle, char* resCode) = 0;
	/**
	 * @brief  关闭休眠
	 * @param[out] resCode 失败时返回4位错误码，成功时返回"0000"
	 * @return 0：成功；1：失败
	 */
	virtual int Printer_CloseSleepMode(char* resCode) { var_UNUSED(resCode); return 0; }
};

#endif
