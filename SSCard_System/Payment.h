#ifndef PAYMENT_H
#define PAYMENT_H
#include <QString>
#include <QImage>
#include "Gloabal.h"

#define OverSocketFailedCont   -2
#define Pay_Succeed             1         // 已经支付
#define Pay_Not                 2         // 尚未支付
#define Failed_QREnocode        3         // 生成支付二维码失败


int  QREnncodeImage(const QString& s, int bulk, QImage& QRImage);

// 已经完成
int  QueryPayment(QString& strMessage, int& strPayStatus);

//int  ReqestPaymentQR(QString &strMessage,QImage &Image);

int  RequestPaymentUrl(QString& strPaymentUrl, QString& strPayCode, QString& strMessage);

// 0-待支付 1-待确认 2-支付完成 3-支付失败 4-订单不存在 5-订单取消
int  queryPayResult(string& strPayCode, QString& strMessage, PayResult& nStatus);

//// nStatus = 0,成功，1 已缴费
//int  ResgisterPayment(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);
//
//// nStatus = 0,成功，否则失败
//int  ApplyCardReplacement(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);
//
//// nStatus = 0,成功，否则失败
//int  CancelCardReplacement(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);
//
//// nStatus = 0，撤销成功
//int  CancelPayment(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);
//
//// nStatus = 0，标注成功
//int  MarkCard(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);
//
//// nStatus = 0 成功
//int  CancelMarkCard(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);
//
//int  GetCardData(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);
//
//int  ReturnCardData(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);
//
//// nStatus = 0 成功
//int  EnalbeCard(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);
//
//int GetCA(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo, const char* QMGY, const char* szAlgorithm, CAInfo& caInfo);
//
//int QueryCardProgress(QString& strMessage, int& nStatus, SSCardBaseInfoPtr& pSSCardInfo);

// nType，0为保存身份证照片，1为保存社保卡照片,2为原始base64数据,3为保存监护人照片

int GetImageStorePath(string& strFilePath, PhotoType nType = PhotoType::Photo_CardID, char* szCardID = nullptr);

int LoadTestIDData(IDCardInfoPtr& pIDCard, SSCardBaseInfoPtr& pSSCardInfo);

int LoadIniCardData(SSCardBaseInfoPtr& pSSCardInfoOut, QString strINIFile);

int LoadJsonCardData(SSCardBaseInfoPtr& pSSCardInfoOut, string strJsonFile);

int SaveSSCardPhoto(QString strMessage, const char* szPhotoBase64, char* szCardID = nullptr);

int SaveSSCardPhotoBase64(QString strMessage, const char* szPhotoBase64);


#endif // PAYMENT_H
