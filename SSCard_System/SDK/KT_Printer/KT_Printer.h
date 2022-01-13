/**
 * @file        KT_Printer.h
 * @author      Pointer
 * @version     10/26/2021
 */
#pragma once
#ifndef _KT_PRINT_H_
#define _KT_PRINT_H_
#include <windows.h>
#define DLL_PUBLIC __declspec(dllexport)

enum PrinterType
{
	PRINTER_MIN = 1,
	EVOLIS_KC200 = 1,
	EVOLIS_ZENIUS,
	EVOLIS_AVANSIA,
	HITI_CS200,
	HITI_CS220,
	HITI_CS290,
	ENTRUCT_EM1,
	ENTRUCT_EM2,
	ENTRUCT_CD809,
	PRINTER_MAX = ENTRUCT_CD809
};


struct _PRINTERBOX
{
	int Type;			//�������� 0:δ֪��1�������䣬2��������
	int BoxNumber;		//�����
	int BoxStatus;		//����״̬,0������; 1������; 2:�޿�; 3����(����)��4������
};

struct BOXINFO
{
	int nCount;					//�����ܸ���(���п���)
	_PRINTERBOX* BoxList;		//������Ϣ�ṹ��ָ��
};

struct PRINTERSTATUS
{
	WORD fwDevice;			//��ӡ��״̬, 0-Ready��1-Busy��2-Offline��3-ErrMachine��4-Printing
	WORD fwMedia;			//����״̬��0-�޿���1-�����ſڣ�2-�����ڲ���3-�����ϵ�λ��4-����բ���⣻5-�¿���6-��Ƭδ֪������Ӳ�����Է���,����֧�����޿���⣩
	WORD fwToner;			//ƽӡɫ��״̬,0-FLLL;1-LOW;2-OUT;3-NOTSUPP;4-UNKNOW
};

class KT_Printer
{
public:
	KT_Printer() {};
	virtual ~KT_Printer() {};

public:
	/**
	 * @brief ��ʼ����ӡ��
	 * @param[in] PrinterType ��ӡ������   Ex: Evolis kc200
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_Init(PrinterType type,char* resCode) = 0;
	/**
	 * @brief �򿪴�ӡ��
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_Open(char* resCode) = 0;
	/**
	 * @brief �رմ�ӡ��
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_Close(char* resCode) = 0;
	/**
	 * @brief ��ӡ����λ
	 * @param[in] Action 1-��λ���������豸�ڣ�2-��λ�˿�����ӡ��ȡ��λ�ã�3-��λ�̿�����ӡ��������(��Ӳ��֧����ʵ��)
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_Reset(int Action,char* resCode) = 0;
	/**
	 * @brief ��ȡ��̬��汾��Ϣ
	 * @param[out] VersionInfo ��̬��汾��Ϣ
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_GetVersion(char* VersionInfo,char* resCode) = 0;
	/**
	 * @brief ��ӡ������
	 * @prarm[in] BoxNo �����
	 * @param[in] CardPos ����λ�� 1-����λ��2-�Ӵ�ICλ;3-�ǽ�ICλ;4-��ӡλ
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_Dispense(int BoxNo,int CardPos,char* resCode) = 0;
	/**
	 * @brief �˿���������
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_Eject(char* resCode) = 0;
	/**
	 * @brief ��ӡ�����տ�
	 * @param BoxNo ���տ�����
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_Retract(int BoxNo,char* resCode) = 0;
	/**
	 * @brief ��ȡ����״̬
	 * @param[out] lpBoxInfo ���ؿ���Ľṹ��,������
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_BoxStatus(BOXINFO &lpBoxInfo,char* resCode) = 0;
	/**
	 * @brief ��ȡ��ӡ��״̬
	 * @param[out] lpStatus ���ش�ӡ��״̬�Ľṹ��,������
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_Status(PRINTERSTATUS& lpStatus,char* resCode) = 0;
	/**
	 * @brief ��ӡ��ʼ��
	 * @param[in] SetParam ��ӡ������ ��ΪNULL
	 * @param[out] resCode ����4λ������ֵ,�ɹ����� "0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_InitPrint(char* SetParam,char* resCode) = 0;
	/**
	 * @brief ��ʼ��ͼƬ��Ϣ,���ж���,��ε���
	 *@param[in] ImgPath ͼƬȫ·��
	 * @param[in] Angle ͼƬ˳ʱ����ת�Ƕ�
	 * @param[in] Xpos ��ӡX����
	 * @param[in] Ypos ��ӡY����
	 * @param[in] ImgHeight ��ӡ�߶�
	 * @param[in] ImgWidth ��ӡ���
	 * @param[out] resCodeʧ��ʱ����4λ�����룬�ɹ�ʱ����"0000"
	 * @return 0:�ɹ� 1:ʧ��
	 */
	virtual int Printer_AddImage(char* ImgPath, int Angle, float Xpos, float Ypos, float ImgHeight, float ImgWidth, char* resCode) = 0;
	/**
	 * @brief  Ԥ��ӡ����,���ж�������,��ε���
	 * @param[in] Text ��ӡ������
	 * @param[in] Angle ˳ʱ����ת�Ƕ�,���ڴ�ӡˮƽ����ֱ����
	 * @param[in] Xpos ��ӡX����
	 * @param[in] Ypos ��ӡY����
	 * @param[in] FontName ��ӡ����,����"����"
	 * @param[in] FontSize �����С
	 * @param[in] FontStyle ������1-���棻2-���壻4-б�壻8-����
	 * @param[in] TextColor ��ӡ������ɫ,RGB��ɫֵ
	 * @param[out] resRcCode ʧ��ʱ����4λ�����룬�ɹ�ʱ����"0000"
	 * @return 0���ɹ���1��ʧ��
	 */
	virtual int Printer_AddText(char* Text, int Angle, float Xpos, float Ypos, char* FontName, int FontSize, int FontStyle, int TextColor, char* resRcCode) = 0;
	/**
	 * @brief  �ύ����,��ʼ��ӡ
	 * @param[out] resCode ʧ��ʱ����4λ�����룬�ɹ�ʱ����"0000"
	 * @return 0���ɹ���1��ʧ��
	 */
	virtual int Printer_StartPrint(char* resCode) = 0;
	/**
	 * @brief  ��ת��Ƭ
	 * @param[in] nAngle ��ת�Ƕ�,һ��ֻ֧��180�ȷ�ת��Ƭ
	 * @param[out] resCode ʧ��ʱ����4λ�����룬�ɹ�ʱ����"0000"
	 * @return 0���ɹ���1��ʧ��
	 */
	virtual int Printer_Rotate(int Angle,char* resCode) = 0;
	/**
	 * @brief  �ر�����
	 * @param[out] resCode ʧ��ʱ����4λ�����룬�ɹ�ʱ����"0000"
	 * @return 0���ɹ���1��ʧ��
	 */
	virtual int Printer_CloseSleepMode(char* resCode) { return 0;}
};

#endif