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
int  QueryPayment(QString& strMessage, int& nPayStatus);

//int  ReqestPaymentQR(QString &strMessage,QImage &Image);

int  RequestPaymentUrl(QString& strMessage, QString& strPaymentUrl);

// nStatus = 0,成功，否则失败
int  ApplyCardReplacement(QString& strMessage, int& nStatus);

// nStatus = 0,成功，否则失败
int  CancelCardReplacement(QString& strMessage, int& nStatus);

// nStatus = 0,成功，1 已缴费
int  ResgisterPayment(QString& strMessage, int& nStatus);

// nStatus = 0，撤销成功
int  CancelPayment(QString& strMessage, int& nStatus);

// nStatus = 0，标注成功
int  MarkCard(QString& strMessage, int& nStatus);

// nStatus = 0 成功
int  CancelMarkCard(QString& strMessage, int& nStatus);

int  GetCardData(QString& strMessage, SSCardInfoPtr& pSSCardInfo);

int  ReturnCardData(QString& strMessage, SSCardInfoPtr& pSSCardInfo);

// nStatus = 0 成功
int  EnalbeCard(QString& strMessage, int& nStatus);

int  GetCA(QString& strMessage, QString& strCA);


#endif // PAYMENT_H
