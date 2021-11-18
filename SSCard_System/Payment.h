#ifndef PAYMENT_H
#define PAYMENT_H
#include <QString>
#include <QImage>

#define OverSocketFailedCont   -2
#define Pay_Succeed             1         // 已经支付
#define Pay_Not                 2         // 沿未支付
#define Failed_QREnocode        3         // 生成支付二维码失败

int  QREnncodeImage(const QString& s, int bulk,QImage &QRImage);

int  QueryPayment(QString &strMessage/*,QString &strPaymentUrl*/);

//int  ReqestPaymentQR(QString &strMessage,QImage &Image);

int  RequestPaymentUrl(QString &strMessage,QString &strPaymentUrl);

int  ApplyCardReplacement(QString &strMessage);

int  CancelCardReplacement(QString &strMessage);

int  ResgisterPayment(QString &strMessage);

int  MarkCard(QString &strMessage);

int  CancelMarkCard(QString &strMessage);

int  GetCardData(QString &strMessage);

int  returnCardData(QString &strMessage);

int  enalbeCard(QString &strMessage);

int  GetCA(QString &strMessage,QString &strCA);

#endif // PAYMENT_H
