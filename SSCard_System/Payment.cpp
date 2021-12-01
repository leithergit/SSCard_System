#include "Payment.h"
#include <QPainter>
#include <QImage>
#include <QRect>
#include "./SDK/QREncode/qrencode.h"

int  QREnncodeImage(const QString& s, int bulk, QImage& QRImage)
{
	QRcode* qr = QRcode_encodeString(s.toUtf8(), 1, QR_ECLEVEL_Q, QR_MODE_8, 0);
	if (qr != nullptr)
	{
		int allBulk = (qr->width) * bulk;
		QRImage = QImage(allBulk, allBulk, QImage::Format_Mono);
		QPainter painter(&QRImage);
		QColor fg("black");
		QColor bg("white");
		painter.setBrush(bg);
		painter.setPen(Qt::NoPen);
		painter.drawRect(0, 0, allBulk, allBulk);
		painter.setBrush(fg);
		for (int y = 0; y < qr->width; y++)
		{
			for (int x = 0; x < qr->width; x++)
			{
				if (qr->data[y * qr->width + x] & 1)
				{
					QRectF r(x * bulk, y * bulk, bulk, bulk);
					painter.drawRects(&r, 1);
				}
			}
		}
		QRcode_free(qr);
		return 0;
	}
	else
		return -1;
}

// 查询支付结果不能返回支付URL，否则未支付前，每次都带会不同的URL，支付结果可能也无法确定
int     QueryPayment(QString& strMessage/*,QString &strPaymentUrl*/)
{
	Q_UNUSED(strMessage);
	return 0;
}

//int     ReqestPaymentQR(QString &strMessage,QImage &Image)
//{
//        return 0;
//}

int     RequestPaymentUrl(QString& strMessage, QString& strPaymentUrl)
{
	Q_UNUSED(strMessage);
#ifdef _DEBUG
	strPaymentUrl = "http://kingaotech.com/";
#endif
	return 0;
}

int     ApplyCardReplacement(QString& strMessage)
{
	Q_UNUSED(strMessage);
	return 0;
}

int     CancelCardReplacement(QString& strMessage)
{
	Q_UNUSED(strMessage);
	return 0;
}

int     ResgisterPayment(QString& strMessage)
{
	Q_UNUSED(strMessage);
	return 0;
}

int     MarkCard(QString& strMessage)
{
	Q_UNUSED(strMessage);
	return 0;
}

int     CancelMarkCard(QString& strMessage)
{
	Q_UNUSED(strMessage);
	return 0;
}

int     GetCardData(QString& strMessage)
{
	Q_UNUSED(strMessage);
	return 0;
}

int     returnCardData(QString& strMessage)
{
	Q_UNUSED(strMessage);
	return 0;
}

int     enalbeCard(QString& strMessage)
{
	Q_UNUSED(strMessage);
	return 0;
}

int     GetCA(QString& strMessage, QString& strCA)
{
	Q_UNUSED(strMessage);
	Q_UNUSED(strCA);
	return 0;
}
