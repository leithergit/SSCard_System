#ifndef PAYMENT_H
#define PAYMENT_H
#include <QString>
#include <QImage>

int  QREnncodeImage(const QString& s, int bulk,QImage &QRImage);

int  QueryPayResult(QString &strMessage);

int  ReqestPaymentQR(QString &strMessage,QImage &Image);

int  GetPaymentUrl(QString &strMessage,QString &strPaymentUrl);

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
