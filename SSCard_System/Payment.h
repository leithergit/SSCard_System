#ifndef PAYMENT_H
#define PAYMENT_H
#include <QString>
#include <QImage>
#include "Gloabal.h"

#define OverSocketFailedCont   -2
#define Pay_Succeed             1         // 已经支付
#define Pay_Not                 2         // 尚未支付
#define Failed_QREnocode        3         // 生成支付二维码失败


#define FailureMessage(strFuncName,pSSCardInfo,strStatus,strMessage) {\
if (strcmp((const char*)strStatus, "08") == 0)\
	strMessage = QString("%1失败:人社服务器没有响应,可能网络异常或人社服务器故障\n姓名:%2\t卡号:%3\t").arg(strFuncName).arg(QString::fromLocal8Bit(pSSCardInfo->strName)).arg(pSSCardInfo->strCardNum);\
else\
	strMessage = QString("%1失败:\n姓名:%2\t卡号:%3\t%4").arg(strFuncName).arg(QString::fromLocal8Bit(pSSCardInfo->strName)).arg(pSSCardInfo->strCardNum).arg(QString::fromLocal8Bit((char *)strStatus)); }\

void  SplitString(const char* szStr, char* szDigit, char* szText);

int  QREnncodeImage(const QString& s, int bulk, QImage& QRImage);

// 已经完成
int  QueryPayment(QString& strMessage, int& strPayStatus);

//int  ReqestPaymentQR(QString &strMessage,QImage &Image);

int  RequestPaymentUrl(QString& strPaymentUrl, QString& strPayCode, QString& strMessage);

int  RequestPaymentUrl2(QString& strPaymentUrl, QString& strPayCode, QString& strTransTime, QString& strMessage);

// 0-待支付 1-待确认 2-支付完成 3-支付失败 4-订单不存在 5-订单取消
int  queryPayResult(string& strPayCode, QString& strMessage, PayResult& nStatus);

int  queryPayResult2(string& strPayCode, string& strTransTime, QString& strMessage, PayResult& nStatus);

// nStatus = 0,成功，1 已缴费
int  ResgisterPayment(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);

// nStatus = 0,成功，否则失败
int  ApplyNewCard(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);

// nStatus = 0,成功，否则失败
int  ApplyCardReplacement(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);

// nStatus = 0,成功，否则失败
int  CancelCardReplacement(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);

// nStatus = 0，撤销成功
int  CancelPayment(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);

// nStatus = 0，标注成功
int  MarkCard(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);

// nStatus = 0 成功
int  CancelMarkCard(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);

void RemoveCardData(SSCardInfoPtr& pSSCardInfo);

int  GetCardData(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo, bool bSkipPreStep = false);

int  ReturnCardData(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo, bool Failed = false);

// nStatus = 0 成功
int  EnalbeCard(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);

int GetCA(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo, const char* QMGY, const char* szAlgorithm, CAInfo& caInfo);

int QueryCardProgress(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo);

// nType，0为保存身份证照片，1为保存社保卡照片
int GetImageStorePath(string& strFilePath, int nType = 0);

int LoadTestData(string& strName, string& strCardID, string& strMobile);

int LoadSSCardData(SSCardInfoPtr& pSSCardInfoOut, QString strINIFile);

int SaveSSCardPhoto(QString strMessage, const char* szPhotoBase64);

int LoadTestIDData(IDCardInfoPtr& pIDCard, SSCardInfoPtr& pSSCardInfo);

#endif // PAYMENT_H
