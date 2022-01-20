
extern "C"
{
	//打开串口
	int  WINAPI	SUNSON_OpenCom(int nPort, long nBaudrate);
	//关闭串口
	int WINAPI	SUNSON_CloseCom(void);
	//获取按下的按键键值
	int WINAPI  SUNSON_ScanKeyPress(unsigned char *ucValue);  //扫描串口获取键值
	int WINAPI SUNSON_GetKeyValue(unsigned char *ucKeyValue); //发送指令获取键值
	//获取操作执行后的错误信息或成功标志信息
	int WINAPI  SUNSON_GetOperationResult(unsigned char *ucResultInfo);
	//B.1取EPP键盘版本号
	int WINAPI	SUNSON_GetVersionNo(unsigned char*VersionNo);
	//B.2系统复位
	int WINAPI	SUNSON_ResetEpp(unsigned char *ReturnInfo);
	//B.3设置按键属性(屏蔽、激活、终止)
	int WINAPI	SUNSON_SetKeyMark(DWORD ActiveFunc,DWORD ActiveFDK,DWORD EndFunc,DWORD EndFDK,unsigned char*ReturnInfo);
	//B.4读取密钥校验值
	int WINAPI	SUNSON_GetCheckValue(unsigned char ucCheckMode,unsigned char *CheckValue);
	//B.5下载用户密钥
	int WINAPI	SUNSON_LoadUserKey(int ucKeyId,int ucDecryptKeyId,unsigned char  KeyAttribute,unsigned char ucKeyLen,unsigned char *KeyValue,unsigned char *ReturnInfo);
	//B.6删除所有密钥
	int WINAPI SUNSON_DeleteAllKeys(unsigned char *ReturnInfo);  
	//B.7删除指定用户密钥
	int WINAPI	SUNSON_DelSpecialUserKey(int UserKeyId,unsigned char *ReturnInfo);
	//B.8蜂鸣器使能
	int WINAPI	SUNSON_SetBuzzerEnabled(unsigned char ucBuzzerStatus,unsigned char *ReturnInfo);
	//B.9 设定初始向量
	int WINAPI  SUNSON_SetStartValue(unsigned char *StartValue,unsigned char *ReturnInfo);
	int WINAPI  SUNSON_SetStartValueEx(int nDataLen,unsigned char *StartValue,unsigned char *ReturnInfo);
	//B.10数据运算
	int WINAPI	SUNSON_DataCompute(int KeyId,unsigned char JM_mode,unsigned char SF_mode,unsigned char padchar,int datalen,unsigned char *data,unsigned char *DataResult);
	//B.11取PIN操作
	int WINAPI	SUNSON_GetPin(unsigned char ucPinMinLen,unsigned char ucPinMaxLen,unsigned char AutoReturnFlag,unsigned char *ReturnInfo);
	//B.12Pinblock运算
	int WINAPI	SUNSON_GetPinBlock(int UserKeyId,int JM_mode,unsigned char padchar,unsigned char ucCardLen,unsigned char*ucCardNumber,unsigned char*PinBlockResult);
	//B.13MAC运算
	int WINAPI	SUNSON_MakeMac(int UserKeyId,int Mac_mode,int nMacDataLen,unsigned char *ucMacData,unsigned char *MacResult);
	//B.14使用EPP仿真键盘
	int WINAPI	SUNSON_UseEppPlainTextMode(unsigned char PlaintextLength,unsigned char AutoEnd,unsigned char *ReturnInfo);
	//B.15关闭EPP键盘  
	int WINAPI	SUNSON_CloseEppPlainTextMode(unsigned char *ReturnInfo);
	//B.16取随机数
	int WINAPI  SUNSON_GetRandomData(unsigned char ucRandomDataType,unsigned char ucDataLength,unsigned char *ReturnInfo);

	//B.17设置版本号
	int WINAPI	SUNSON_SetVersionNo(unsigned char*VersionNo,unsigned char *ReturnInfo);
	//UBC算法
	int WINAPI SUNSON_MakeUBCMac(int UserKeyId,int nMacLen,unsigned char* macdata,unsigned char *ReturnInfo);

	//获取键盘状态 
	int WINAPI	SUNSON_GetEppStatus(unsigned char*EppStatus);
	//设置卡键时间
	int WINAPI	SUNSON_SetLockKeyTimeOut(unsigned char TimeOut,unsigned char *ReturnInfo);

	//打开/关闭开封自毁   0x00 关闭固件自毁(仅100N支持)
	//                    0x01 开启固件自毁(仅100N支持)
	//                    0x02 关闭密钥自毁
	//                    0x04 开启密钥自毁
	int WINAPI	SUNSON_EnableMoveDestruct(unsigned char mode, unsigned char *ReturnInfo);

	//国密相关
	//设置国密非密模式
	int WINAPI SUNSON_SetEPPSMMode(unsigned char ucSMMode);
	//获取键盘模式
	int WINAPI SUNSON_GetEPPSMMode(unsigned char *ucEPPMode);
	//生成SM2密钥
	int WINAPI SUNSON_GenerateSM2Key(int ucKeyId,int KeyAtt,unsigned char *ReturnInfo);
	//下载SM2密钥
	int WINAPI SUNSON_LoadSM2Key(int ucKeyId,int KeyAtt,int nKeylen,unsigned char *KeyValue,unsigned char *ReturnInfo);
	//导出SM2公钥
	int WINAPI SUNSON_ExportSM2PublicKey(int ucKeyId,unsigned char *ReturnInfo);
	//Hash ID初始化
	int WINAPI SUNSON_InitHashID(int ucKeyId,unsigned char UserIDLen,unsigned char *UserID,unsigned char *ReturnInfo);
	//SM2签名
	int WINAPI SUNSON_GetSM2Signature(int ucKeyId,unsigned char EDataLen,unsigned char *EData,unsigned char *ReturnInfo);
	//SM2验签
	int WINAPI SUNSON_VerifySM2Signature(int ucKeyId,unsigned char EDataLen,unsigned char *EData,unsigned char SignatureLen,unsigned char *Signature,unsigned char *ReturnInfo);
	//SM2数据运算
	int WINAPI SUNSON_SM2DataCompute(int ucKeyId,unsigned char ucKeyType,int nDatalen,unsigned char *data,unsigned char *DataResult);
	//SM3哈希操作
	int WINAPI SUNSON_SM3HashOperate(unsigned char ucSM3HashType,int nDatalen,unsigned char *data,unsigned char *DataResult);
	//使能国密程序下载  
	int WINAPI SUNSON_EnableLoadSMFirmware(void);
	//SM4 MAC运算
	int WINAPI SUNSON_SM4MakeMac(int UserKeyId,int Mac_mode,int nMacDataLen,unsigned char *ucMacData,unsigned char *MacResult);
	//SM4数据运算
	int WINAPI SUNSON_SM4DataCompute(int KeyId,unsigned char JM_mode,int SF_mode,unsigned char padchar,int datalen,unsigned char *data,unsigned char *DataResult);
	//SM4 PINBLOCK
	int WINAPI SUNSON_SM4GetPinBlock(int UserKeyId,int JM_mode,unsigned char padchar,unsigned char ucCardLen,unsigned char*ucCardNumber,unsigned char*PinBlockResult);

	//导入SM4密钥，通过SM2解密
	int WINAPI SUNSON_ImportSM4KeyUseSM2key(int nSM2keyID, int nSM4keyID, int nKeyAttr, int nKCVmode, int nKeyLen, unsigned char* KeyValue, unsigned char* ReturnInfo);

	//设置能否重复下载密钥 00 允许重复下载； 01 不允许重复下载
	int WINAPI SUNSON_EnableLoadSameKey(unsigned char Mode, unsigned char *ReturnInfo);

	//获取指定密钥的KCV
	int WINAPI SUNSON_GetKCV(int keyId, unsigned char ucCheckMode,unsigned char *CheckValue);

	//获取密钥属性值
	int WINAPI	SUNSON_GetKeyAttInfo(int UserKeyId,unsigned char *KeyAttInfo);

	//控制LED灯开关(客户定制) 1, 打开LED ； 0 关闭LED
	int WINAPI  SUNSON_EnableLED(unsigned char flag);

	//设置波特率 mode 0x30 2400:
		          //  0x31 4800:
	              //  0x32 9600
	              //  0x33 19200
	              //  0x34 38400
	              //  0x35 57600
	              //  0x36 115200
	int WINAPI  SUNSON_SetBaudRate(unsigned char ucBaudRateMode,unsigned char *ReturnInfo);

	//开关usb明文系统键盘  0x01关闭USB系统键盘
    //                     0x02开启USB系统键盘                      
	int WINAPI  SUNSON_EnableUSBplaintext(unsigned char mode);

	//开关日志
	int WINAPI SUNSON_EnableLog(bool bOpen);

	//RSA
	//导出RSA公钥
	int WINAPI	SUNSON_ExportRSAKey(unsigned char ucKeyId, unsigned char *ucData);
	//导入RSA密钥
	int WINAPI	SUNSON_LoadRSAKey(unsigned char ucKeyId,unsigned char nSignatureId, int nKeylen, unsigned char *KeyValue, int nSignatureLen, unsigned char* SignatureData, unsigned char * ReturnInfo);
	//RSA签名
	int WINAPI SUNSON_GetRSASignature(unsigned char ucKeyId, unsigned char Padding,unsigned char HashId,int DataLen,
										unsigned char *ucData,unsigned char *DataResult);
	//RSA验签
	int WINAPI SUNSON_VerifyRSASignature(unsigned char ucKeyId, unsigned char Padding,unsigned char HashId,int SignLen,
											unsigned char *SignData,int DataLen,unsigned char *ucData,unsigned char *ReturnInfo);

	//RSA导出签名DER格式（部分键盘支持）
	int WINAPI SUNSON_ExportRSASignDer(unsigned char ucKeyId, unsigned char SignkeyId, unsigned long SignAlg, unsigned char *ucData);

	//导入无签名的RSAKEY KeyType = 0x01 导DER私钥
	//                   KeyType = 0x02 导DER公钥
	int WINAPI SUNSON_ImportNoSignRSAKey(unsigned char ucKeyId, unsigned char KeyType, int DataLen,unsigned char *ucData,unsigned char *ReturnInfo);

	//RKL 接口	
	//生成RSA密钥 nKeylen 1024,2048 exp 3, 17, 65537
	int WINAPI  SUNSON_GenerateRSAKey(unsigned char ucKeyId, long nKeylen,long Eval, long Dwuse, unsigned char* ReturnInfo);
	//导出RSA DerKey mode:0x01 DER格式, 0x02:256字节N值
	int WINAPI SUNSON_ExportRSADerKey(unsigned char ucKeyId, unsigned char mode, int *nkeyderLen, unsigned char *ucKeyDer, int *nSignLen, unsigned char *ucSignData, unsigned char* ReturnInfo);
	//导入RSA DerKey
	int WINAPI SUNSON_ImportRSADerKey(unsigned char ucKeyId, unsigned char SignkeyId, int DerKeyLen, unsigned char *DerKey, long Dwuse, 
										long SignAlg, long CheckMode, int SignLen, unsigned char *SignDat, unsigned char *ucResult);
	//导入签名值
	int WINAPI SUNSON_ImportSignValue(unsigned char nKeyId, int SignLen, unsigned char * SignData, unsigned char *ReturnInfo);
	//导入RSA签名的DES密钥
	int WINAPI SUNSON_ImportRSADesKey(unsigned char nDESKeyId, unsigned char nRSAKeyId, unsigned char SignKeyId, long EncipherAlg, long SignAlg, long Dwuse,
										int LpxValLen, unsigned char *LpxVal, int LpxSignLen, unsigned char *LpxSign, unsigned char *ucResult);

	//导入EPPID和签名值 signAlg 签名算法
	int WINAPI SUNSON_ImportEppidAndSignData(unsigned char SignAlg, int nEppidLen, unsigned char *EPPID, int SignLen, unsigned char *SignData, unsigned char* ReturnInfo);

	//导出EPPID和签名值
	int WINAPI SUNSON_ExportEppidAndSignData(unsigned char *SignAlg, int *nEppidLen, unsigned char *EPPID, int *SignLen, unsigned char *SignData, unsigned char* ReturnInfo);

	//RSA运算
	int WINAPI	SUNSON_RSACompute(int KeyId,unsigned char keyClass, unsigned char PaddingMode,
										unsigned char HashMode,int datalen,unsigned char *data,unsigned char *DataResult);

	//导出EPP键盘签名和指定密钥签名的EPPID
	int WINAPI SUNSON_ExportEPPsignEPPid(int SignKeyID, unsigned char SignAlgorithm, int *nEppidLen, unsigned char *EPPID, int *SignLen, unsigned char *SignData, unsigned char* ReturnInfo);

	//导出EPP键盘签名和指定密钥签名的公钥数据
	int WINAPI SUNSON_ExportEPPsignPublicKey(int keyID, int SignKeyID, unsigned char SignAlgorithm, int *PublicKeyLen, unsigned char* PublicKey,int *EppPrivatekeySignLen, unsigned char *EppPrivatekeySignData, int *SignLen, unsigned char *SignData, unsigned char* ReturnInfo);

	//设置RKL hash算法 0:NID_MD5, 1:NID_sha1, 2:NID_sha224, 3:NID_sha256
	int WINAPI SUNSON_SetHashMode(int nMode, unsigned char* ReturnInfo);


	//恢复保护密钥(GD客户定制)
	int WINAPI SUNSON_RestoreInitKey();

};