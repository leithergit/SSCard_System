
extern "C"
{
	//�򿪴���
	int  WINAPI	SUNSON_OpenCom(int nPort, long nBaudrate);
	//�رմ���
	int WINAPI	SUNSON_CloseCom(void);
	//��ȡ���µİ�����ֵ
	int WINAPI  SUNSON_ScanKeyPress(unsigned char *ucValue);  //ɨ�贮�ڻ�ȡ��ֵ
	int WINAPI SUNSON_GetKeyValue(unsigned char *ucKeyValue); //����ָ���ȡ��ֵ
	//��ȡ����ִ�к�Ĵ�����Ϣ��ɹ���־��Ϣ
	int WINAPI  SUNSON_GetOperationResult(unsigned char *ucResultInfo);
	//B.1ȡEPP���̰汾��
	int WINAPI	SUNSON_GetVersionNo(unsigned char*VersionNo);
	//B.2ϵͳ��λ
	int WINAPI	SUNSON_ResetEpp(unsigned char *ReturnInfo);
	//B.3���ð�������(���Ρ������ֹ)
	int WINAPI	SUNSON_SetKeyMark(DWORD ActiveFunc,DWORD ActiveFDK,DWORD EndFunc,DWORD EndFDK,unsigned char*ReturnInfo);
	//B.4��ȡ��ԿУ��ֵ
	int WINAPI	SUNSON_GetCheckValue(unsigned char ucCheckMode,unsigned char *CheckValue);
	//B.5�����û���Կ
	int WINAPI	SUNSON_LoadUserKey(int ucKeyId,int ucDecryptKeyId,unsigned char  KeyAttribute,unsigned char ucKeyLen,unsigned char *KeyValue,unsigned char *ReturnInfo);
	//B.6ɾ��������Կ
	int WINAPI SUNSON_DeleteAllKeys(unsigned char *ReturnInfo);  
	//B.7ɾ��ָ���û���Կ
	int WINAPI	SUNSON_DelSpecialUserKey(int UserKeyId,unsigned char *ReturnInfo);
	//B.8������ʹ��
	int WINAPI	SUNSON_SetBuzzerEnabled(unsigned char ucBuzzerStatus,unsigned char *ReturnInfo);
	//B.9 �趨��ʼ����
	int WINAPI  SUNSON_SetStartValue(unsigned char *StartValue,unsigned char *ReturnInfo);
	int WINAPI  SUNSON_SetStartValueEx(int nDataLen,unsigned char *StartValue,unsigned char *ReturnInfo);
	//B.10��������
	int WINAPI	SUNSON_DataCompute(int KeyId,unsigned char JM_mode,unsigned char SF_mode,unsigned char padchar,int datalen,unsigned char *data,unsigned char *DataResult);
	//B.11ȡPIN����
	int WINAPI	SUNSON_GetPin(unsigned char ucPinMinLen,unsigned char ucPinMaxLen,unsigned char AutoReturnFlag,unsigned char *ReturnInfo);
	//B.12Pinblock����
	int WINAPI	SUNSON_GetPinBlock(int UserKeyId,int JM_mode,unsigned char padchar,unsigned char ucCardLen,unsigned char*ucCardNumber,unsigned char*PinBlockResult);
	//B.13MAC����
	int WINAPI	SUNSON_MakeMac(int UserKeyId,int Mac_mode,int nMacDataLen,unsigned char *ucMacData,unsigned char *MacResult);
	//B.14ʹ��EPP�������
	int WINAPI	SUNSON_UseEppPlainTextMode(unsigned char PlaintextLength,unsigned char AutoEnd,unsigned char *ReturnInfo);
	//B.15�ر�EPP����  
	int WINAPI	SUNSON_CloseEppPlainTextMode(unsigned char *ReturnInfo);
	//B.16ȡ�����
	int WINAPI  SUNSON_GetRandomData(unsigned char ucRandomDataType,unsigned char ucDataLength,unsigned char *ReturnInfo);

	//B.17���ð汾��
	int WINAPI	SUNSON_SetVersionNo(unsigned char*VersionNo,unsigned char *ReturnInfo);
	//UBC�㷨
	int WINAPI SUNSON_MakeUBCMac(int UserKeyId,int nMacLen,unsigned char* macdata,unsigned char *ReturnInfo);

	//��ȡ����״̬ 
	int WINAPI	SUNSON_GetEppStatus(unsigned char*EppStatus);
	//���ÿ���ʱ��
	int WINAPI	SUNSON_SetLockKeyTimeOut(unsigned char TimeOut,unsigned char *ReturnInfo);

	//��/�رտ����Ի�   0x00 �رչ̼��Ի�(��100N֧��)
	//                    0x01 �����̼��Ի�(��100N֧��)
	//                    0x02 �ر���Կ�Ի�
	//                    0x04 ������Կ�Ի�
	int WINAPI	SUNSON_EnableMoveDestruct(unsigned char mode, unsigned char *ReturnInfo);

	//�������
	//���ù��ܷ���ģʽ
	int WINAPI SUNSON_SetEPPSMMode(unsigned char ucSMMode);
	//��ȡ����ģʽ
	int WINAPI SUNSON_GetEPPSMMode(unsigned char *ucEPPMode);
	//����SM2��Կ
	int WINAPI SUNSON_GenerateSM2Key(int ucKeyId,int KeyAtt,unsigned char *ReturnInfo);
	//����SM2��Կ
	int WINAPI SUNSON_LoadSM2Key(int ucKeyId,int KeyAtt,int nKeylen,unsigned char *KeyValue,unsigned char *ReturnInfo);
	//����SM2��Կ
	int WINAPI SUNSON_ExportSM2PublicKey(int ucKeyId,unsigned char *ReturnInfo);
	//Hash ID��ʼ��
	int WINAPI SUNSON_InitHashID(int ucKeyId,unsigned char UserIDLen,unsigned char *UserID,unsigned char *ReturnInfo);
	//SM2ǩ��
	int WINAPI SUNSON_GetSM2Signature(int ucKeyId,unsigned char EDataLen,unsigned char *EData,unsigned char *ReturnInfo);
	//SM2��ǩ
	int WINAPI SUNSON_VerifySM2Signature(int ucKeyId,unsigned char EDataLen,unsigned char *EData,unsigned char SignatureLen,unsigned char *Signature,unsigned char *ReturnInfo);
	//SM2��������
	int WINAPI SUNSON_SM2DataCompute(int ucKeyId,unsigned char ucKeyType,int nDatalen,unsigned char *data,unsigned char *DataResult);
	//SM3��ϣ����
	int WINAPI SUNSON_SM3HashOperate(unsigned char ucSM3HashType,int nDatalen,unsigned char *data,unsigned char *DataResult);
	//ʹ�ܹ��ܳ�������  
	int WINAPI SUNSON_EnableLoadSMFirmware(void);
	//SM4 MAC����
	int WINAPI SUNSON_SM4MakeMac(int UserKeyId,int Mac_mode,int nMacDataLen,unsigned char *ucMacData,unsigned char *MacResult);
	//SM4��������
	int WINAPI SUNSON_SM4DataCompute(int KeyId,unsigned char JM_mode,int SF_mode,unsigned char padchar,int datalen,unsigned char *data,unsigned char *DataResult);
	//SM4 PINBLOCK
	int WINAPI SUNSON_SM4GetPinBlock(int UserKeyId,int JM_mode,unsigned char padchar,unsigned char ucCardLen,unsigned char*ucCardNumber,unsigned char*PinBlockResult);

	//����SM4��Կ��ͨ��SM2����
	int WINAPI SUNSON_ImportSM4KeyUseSM2key(int nSM2keyID, int nSM4keyID, int nKeyAttr, int nKCVmode, int nKeyLen, unsigned char* KeyValue, unsigned char* ReturnInfo);

	//�����ܷ��ظ�������Կ 00 �����ظ����أ� 01 �������ظ�����
	int WINAPI SUNSON_EnableLoadSameKey(unsigned char Mode, unsigned char *ReturnInfo);

	//��ȡָ����Կ��KCV
	int WINAPI SUNSON_GetKCV(int keyId, unsigned char ucCheckMode,unsigned char *CheckValue);

	//��ȡ��Կ����ֵ
	int WINAPI	SUNSON_GetKeyAttInfo(int UserKeyId,unsigned char *KeyAttInfo);

	//����LED�ƿ���(�ͻ�����) 1, ��LED �� 0 �ر�LED
	int WINAPI  SUNSON_EnableLED(unsigned char flag);

	//���ò����� mode 0x30 2400:
		          //  0x31 4800:
	              //  0x32 9600
	              //  0x33 19200
	              //  0x34 38400
	              //  0x35 57600
	              //  0x36 115200
	int WINAPI  SUNSON_SetBaudRate(unsigned char ucBaudRateMode,unsigned char *ReturnInfo);

	//����usb����ϵͳ����  0x01�ر�USBϵͳ����
    //                     0x02����USBϵͳ����                      
	int WINAPI  SUNSON_EnableUSBplaintext(unsigned char mode);

	//������־
	int WINAPI SUNSON_EnableLog(bool bOpen);

	//RSA
	//����RSA��Կ
	int WINAPI	SUNSON_ExportRSAKey(unsigned char ucKeyId, unsigned char *ucData);
	//����RSA��Կ
	int WINAPI	SUNSON_LoadRSAKey(unsigned char ucKeyId,unsigned char nSignatureId, int nKeylen, unsigned char *KeyValue, int nSignatureLen, unsigned char* SignatureData, unsigned char * ReturnInfo);
	//RSAǩ��
	int WINAPI SUNSON_GetRSASignature(unsigned char ucKeyId, unsigned char Padding,unsigned char HashId,int DataLen,
										unsigned char *ucData,unsigned char *DataResult);
	//RSA��ǩ
	int WINAPI SUNSON_VerifyRSASignature(unsigned char ucKeyId, unsigned char Padding,unsigned char HashId,int SignLen,
											unsigned char *SignData,int DataLen,unsigned char *ucData,unsigned char *ReturnInfo);

	//RSA����ǩ��DER��ʽ�����ּ���֧�֣�
	int WINAPI SUNSON_ExportRSASignDer(unsigned char ucKeyId, unsigned char SignkeyId, unsigned long SignAlg, unsigned char *ucData);

	//������ǩ����RSAKEY KeyType = 0x01 ��DER˽Կ
	//                   KeyType = 0x02 ��DER��Կ
	int WINAPI SUNSON_ImportNoSignRSAKey(unsigned char ucKeyId, unsigned char KeyType, int DataLen,unsigned char *ucData,unsigned char *ReturnInfo);

	//RKL �ӿ�	
	//����RSA��Կ nKeylen 1024,2048 exp 3, 17, 65537
	int WINAPI  SUNSON_GenerateRSAKey(unsigned char ucKeyId, long nKeylen,long Eval, long Dwuse, unsigned char* ReturnInfo);
	//����RSA DerKey mode:0x01 DER��ʽ, 0x02:256�ֽ�Nֵ
	int WINAPI SUNSON_ExportRSADerKey(unsigned char ucKeyId, unsigned char mode, int *nkeyderLen, unsigned char *ucKeyDer, int *nSignLen, unsigned char *ucSignData, unsigned char* ReturnInfo);
	//����RSA DerKey
	int WINAPI SUNSON_ImportRSADerKey(unsigned char ucKeyId, unsigned char SignkeyId, int DerKeyLen, unsigned char *DerKey, long Dwuse, 
										long SignAlg, long CheckMode, int SignLen, unsigned char *SignDat, unsigned char *ucResult);
	//����ǩ��ֵ
	int WINAPI SUNSON_ImportSignValue(unsigned char nKeyId, int SignLen, unsigned char * SignData, unsigned char *ReturnInfo);
	//����RSAǩ����DES��Կ
	int WINAPI SUNSON_ImportRSADesKey(unsigned char nDESKeyId, unsigned char nRSAKeyId, unsigned char SignKeyId, long EncipherAlg, long SignAlg, long Dwuse,
										int LpxValLen, unsigned char *LpxVal, int LpxSignLen, unsigned char *LpxSign, unsigned char *ucResult);

	//����EPPID��ǩ��ֵ signAlg ǩ���㷨
	int WINAPI SUNSON_ImportEppidAndSignData(unsigned char SignAlg, int nEppidLen, unsigned char *EPPID, int SignLen, unsigned char *SignData, unsigned char* ReturnInfo);

	//����EPPID��ǩ��ֵ
	int WINAPI SUNSON_ExportEppidAndSignData(unsigned char *SignAlg, int *nEppidLen, unsigned char *EPPID, int *SignLen, unsigned char *SignData, unsigned char* ReturnInfo);

	//RSA����
	int WINAPI	SUNSON_RSACompute(int KeyId,unsigned char keyClass, unsigned char PaddingMode,
										unsigned char HashMode,int datalen,unsigned char *data,unsigned char *DataResult);

	//����EPP����ǩ����ָ����Կǩ����EPPID
	int WINAPI SUNSON_ExportEPPsignEPPid(int SignKeyID, unsigned char SignAlgorithm, int *nEppidLen, unsigned char *EPPID, int *SignLen, unsigned char *SignData, unsigned char* ReturnInfo);

	//����EPP����ǩ����ָ����Կǩ���Ĺ�Կ����
	int WINAPI SUNSON_ExportEPPsignPublicKey(int keyID, int SignKeyID, unsigned char SignAlgorithm, int *PublicKeyLen, unsigned char* PublicKey,int *EppPrivatekeySignLen, unsigned char *EppPrivatekeySignData, int *SignLen, unsigned char *SignData, unsigned char* ReturnInfo);

	//����RKL hash�㷨 0:NID_MD5, 1:NID_sha1, 2:NID_sha224, 3:NID_sha256
	int WINAPI SUNSON_SetHashMode(int nMode, unsigned char* ReturnInfo);


	//�ָ�������Կ(GD�ͻ�����)
	int WINAPI SUNSON_RestoreInitKey();

};