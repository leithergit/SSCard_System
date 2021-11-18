#pragma once
#ifndef KT_READER_H
#define KT_READER_H
#include <windows.h>

typedef enum ReaderBrand
{
	IN_VALID = -1,
	DC_READER = 1,	//�¿�������
	MH_READER,		//����������
	HD_READER,		//���������
}RT;

typedef enum CardPowerType
{
	READER_CONTACT = 1,		//�Ӵ�ʽ
	READER_UNCONTACT,		//�ǽ�ʽ
}PowerType;

class KT_Reader
{
public:
	KT_Reader() {};
	virtual ~KT_Reader() {};
public:
	/**
	 * @brief ��������������,�ο�ö��Reader
	 * @param[in] ReaderType ����������
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Reader_Create(RT ReaderType, char* resCode) = 0;
	/**
	 * @brief ��ʼ���������˿�
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Reader_Init(char* resCode) = 0;
	/**
	 * @brief �������˿��˳�
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Reader_Exit(char* resCode) = 0;
	/**
	 * @brief �ϵ����
	 * @param[in] Type �ϵ�����,�Ӵ�or�ǽ�
	 * @param[out] CardAtr ATR����
	 * @param[out] AtrLen  ATR���ݳ���
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Reader_PowerOn(PowerType Type, char* CardAtr,int &AtrLen,char* resCode) = 0;
	/**
	 * @brief �µ����
	 * @param[in] Type �µ�����,�Ӵ�or�ǽ�
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Reader_PowerOff(PowerType Type,char* resCode) = 0;
	/**
	 * @brief ִ���������
	 * @param[in] Cmd �����������
	 * @param[in] CmdLen ���͵������
	 * @param[out] OutData ���ص�����
	 * @param[out] OutLen ���ص����ݳ���
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Reader_APDU(BYTE* Cmd, int CmdLen, char* OutData, int &OutLen, char* resCode) = 0;
	/**
	 * @brief ����������
	 * @param[in] TimeOut ��ʱʱ��
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Reader_Beep(int TimeOut, char* resCode) { return 0;}
	/**
	 * @brief ��ȡ�������汾��Ϣ
	 * @param[out] Version ���صİ汾��Ϣ
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Reader_GetVersion(char* Version, char* resCode) { return 0;}
};

#endif // !KT_READER_H
