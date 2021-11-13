#ifndef __XZ_F10_API_4SIM__
#define __XZ_F10_API_4SIM__

extern "C"
{
	//创建设备
	//nDeviceType: 
	//打开设备 nPort : 0 USB, 1 COM
	int  WINAPI	SUNSON_OpenCom(int nPort, long nBaud);
	//关闭设备
	int WINAPI	SUNSON_CloseCom(void);

	//获取按下的按键键值
	int WINAPI  SUNSON_ScanKeyPress(unsigned char *ucKeyValue);
	//使用键盘仿真
	int WINAPI SUNSON_UseEppPlainTextMode(unsigned char ucTextModeFormat,unsigned char *ReturnInfo);
	//键盘复位自检，所有密钥复位
	int WINAPI SUNSON_ResetEPP(bool bInitKey = FALSE);
	//下载主密钥
	int WINAPI SUNSON_LoadMasterKey (unsigned char ucMKeyId,unsigned char ucMKeyLen,unsigned char *NewMasterKey,unsigned char * ReturnInfo);
	//下载工作密钥
	int WINAPI SUNSON_LoadWorkKey(unsigned char ucMKeyId,unsigned char ucWKeyId,unsigned char ucWKeyLen,unsigned char* WorkKeyCiphertext,unsigned char *ReturnInfo);
	//激活工作密钥
	int WINAPI SUNSON_ActiveKey(unsigned char MasterKeyId,unsigned char ucWkeyId,unsigned char *ReturnInfo);
	//启动密码键盘加密
	int WINAPI SUNSON_StartEpp(unsigned char ucPinLen,unsigned char AlgorithmMode,unsigned char timeout,unsigned char *ReturnInfo);
	//取PIN密文
	int WINAPI SUNSON_ReadCypherPin(unsigned char *ReturnInfo);
	//下载账号
	int WINAPI SUNSON_LoadCardNumber(unsigned char* ucCardNumber,unsigned char *ReturnInfo);
	//下载终端号
	int WINAPI SUNSON_LoadTerminalNumber(unsigned char* ucTerminalNo,unsigned char *ReturnInfo);
	//数据加密运算
	int WINAPI SUNSON_DataEncrypt(unsigned  char ucDataLen,unsigned char* SourceData,unsigned char *ReturnInfo);
	//数据解密运算
	int WINAPI SUNSON_DataDecrypt(unsigned  char ucDataLen,unsigned char* SourceData,unsigned char *ReturnInfo);
	//数据mac运算
	int WINAPI SUNSON_MakeMac(int nMacDataLen,unsigned char* ucMacData,unsigned char *ReturnInfo);
	//回送字符
	int WINAPI SUNSON_SendAscII(unsigned char Ascii,unsigned char *ReturnInfo);
	//设置IC卡卡座及类型
	int WINAPI SUNSON_SetSimCardIdAndKind(unsigned char CardId,unsigned char CardKind,unsigned char *ReturnInfo);
	//获取IC卡卡座及类型
	int WINAPI SUNSON_GetSimCardIdAndKind(unsigned char CardId,unsigned char *ReturnInfo);
	//上电复位IC卡
	int WINAPI SUNSON_CardPowerOn(unsigned char *ReturnInfo);
	//IC卡下电
	int WINAPI SUNSON_CardPowerOff(unsigned char *ReturnInfo);
	//访问COS
	int WINAPI SUNSON_UseAPDU(unsigned  char len,unsigned char* apdu,unsigned char *ReturnInfo);
	//取PIN数据(用于sam加密)
	int WINAPI SUNSON_GetPinFromSIM(unsigned  char len,unsigned char* apdu,unsigned char *ReturnInfo);
	//写产品版本号
	int WINAPI SUNSON_SetVersion(unsigned char*version,unsigned char*SerialNo,unsigned char *ReturnInfo);
	//读取版本号
	int WINAPI SUNSON_GetVersion(unsigned char *version);
	//读产品终端号
	int WINAPI SUNSON_GetSerialNumber(unsigned char *SerialNumber);
	//写产品终端号
	int WINAPI SUNSON_SetSerialNumber(unsigned char *SerialNumber,unsigned char *ReturnInfo);
	//ubc mac 计算
	int WINAPI SUNSON_MakeUBCMac(int nMacDataLen,unsigned char* ucMacData,unsigned char *ReturnInfo,unsigned char *hexReturnInfo);
	//X9.19的ECB mac
	int WINAPI SUNSON_MakeX919ECBMac(int nMacDataLen,unsigned char* ucMacData,unsigned char *hexReturnInfo);
	//X9.9的ECB mac
	int WINAPI SUNSON_MakeX99ECBMac(int nMacDataLen,unsigned char* ucMacData,unsigned char *hexReturnInfo);
	//标准的 mac
	int WINAPI SUNSON_MakeBaseMac(int nMacDataLen,unsigned char* ucMacData,unsigned char *hexReturnInfo);
	////X9.9的CBC mac
	int WINAPI SUNSON_MakeX99CBCMac(int nMacDataLen,unsigned char* initvector,unsigned char* ucMacData,unsigned char *hexReturnInfo);
	///设置算法处理参数
	int WINAPI SUNSON_SetAlgorithmParameter(unsigned char ucPPara,unsigned char ucFPara,unsigned char *ReturnInfo);	
	//利用SAM卡下载工作密钥
	int WINAPI SUNSON_LoadWorkKeyBySAM(unsigned char APDULen,unsigned char MKeyID,unsigned char WKeyID, unsigned char * APDU,unsigned char *errorinfo);
	
	//获取随机数  升腾要求增加  2014-4-16    xiaweiqian
	int WINAPI SUNSON_GetRandom(unsigned char *pReturnInfo);
	//产生过程密钥
	int WINAPI SUNSON_GenerateProcessKey(unsigned char *pRandom, int nRandomLen,unsigned char *pReturnInfo);
	//根据升腾资讯要求封装接口 2014-5-6 xiaweiqian
	//下载主密钥
	int WINAPI SUNSON_DownloadMasterKey(int nKeyId,int nDecKeyId,unsigned char ucKeyMode,unsigned char *ucKeyValue,unsigned char *ucCheckValue);
	//下载工作密钥
	int WINAPI SUNSON_DownloadWorkKey(int nMKeyId,int nWKeyId,unsigned char ucKeyMode,unsigned char *ucKeyValue,unsigned char *ucCheckValue);
	//GetPin
	int WINAPI SUNSON_GetPin(int nMKeyId,int nWKeyId,unsigned char ucKeyMode,unsigned char ucPinlen,unsigned char ucPinType,unsigned char *ucCardNo,int nTimeOut,unsigned char *ucDisperse);
	//Mac
	int WINAPI SUNSON_GetMac(int nMKeyId,int nWKeyId,unsigned char ucKeyMode,unsigned char ucMacType,int nMaclen,unsigned char *ucMacData,unsigned char *ucDisperse,unsigned char *ucMacResult);
	//数据运算
	int WINAPI SUNSON_DataCompute(int nMKeyId,int nWKeyId,unsigned char ucKeyMode,unsigned char ucType,int nDatalen,unsigned char *ucData,unsigned char *ucDisperse,unsigned char *ucDataResult);

	//国密算法
	//设置国密非密模式
	int WINAPI SUNSON_SetEPPSMMode(unsigned char ucSMMode);
	//获取键盘模式
	int WINAPI SUNSON_GetEPPSMMode(unsigned char *ucEPPMode);
	//生成SM2密钥
	int WINAPI SUNSON_GenerateSM2Key(unsigned char ucKeyId,unsigned char ucKeyAttr);
	//SM2加密
	int WINAPI SUNSON_SM2DataEncrypt(unsigned char ucKeyId,WORD nDatalen,unsigned char * ucData,unsigned char *ucReturnInfo);
	//SM2解密
	int WINAPI SUNSON_SM2DataDecrypt(unsigned char ucKeyId,WORD nDatalen,unsigned char * ucData,unsigned char *ucReturnInfo);
	//SM2签名
	int WINAPI SUNSON_GetSM2Signature(unsigned char ucKeyId,WORD nDatalen,unsigned char * ucData,unsigned char *ucReturnInfo);
	//SM2验签
	int WINAPI SUNSON_VerifySM2Signature(unsigned char ucKeyId,WORD nDatalen,unsigned char * ucData);
	//下载SM2密钥
	int WINAPI SUNSON_LoadSM2Key(unsigned char ucKeyId,WORD KeyAttr,WORD PubKeylen,unsigned char *PubKey,WORD PriKeylen,unsigned char *PriKey);
	//导出SM2公钥
	int WINAPI SUNSON_ExportSM2PublicKey(unsigned char ucKeyId,unsigned char *ucPublicKey);
	//SM3Hash运算
	int WINAPI SUNSON_SM3HashOperate(unsigned char ucHashType,WORD nDatalen,unsigned char *ucData,unsigned char *ucReturnInfo);
	//Hash用户ID
	int WINAPI SUNSON_InitHashID(unsigned char ucKeyId,WORD ucIDlen,unsigned char *ucData);

	//开启/关闭系统键盘功能 0 关闭，1 打开
	int WINAPI SUNSON_EnableSystemKeyboard(int nMode, unsigned char *ucReturnInfo);

	//更新固件
	//返回的升级进度，0-100
	int WINAPI SUNSON_LoadFirmwareFile(char* sFileName, int * pnPos);
};

#endif



