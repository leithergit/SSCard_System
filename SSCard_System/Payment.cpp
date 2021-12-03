#pragma execution_character_set("utf-8")
#include "Payment.h"
#include <QPainter>
#include <QImage>
#include <QRect>
#include "./SDK/QREncode/qrencode.h"
#include "Gloabal.h"

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
int     QueryPayment(QString& strMessage, int& nPayStatus)
{
	Q_UNUSED(strMessage);

	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	char szStatus[1024] = { 0 };
	int nResult = queryPayment(Region.strCMAccount.c_str(), Region.strCMPassword.c_str(), (char*)pIDCard->szIdentify, (char*)pIDCard->szName, (char*)szStatus);
	if (QFailed(nResult))
	{
		strMessage = "查询付费状态失败";
		QString strInfo = QString("queryPayment Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	QString strInfo = QString("queryPayment Succeed:%1.").arg(szStatus);
	gInfo() << gQStr(strInfo);
	if (strcmp(_strupr(szStatus), "OK") == 0)
	{
		nPayStatus = Pay_Succeed;
	}
	else
		nPayStatus = Pay_Not;

	return 0;
}

int     RequestPaymentUrl(QString& strMessage, QString& strPaymentUrl)
{
	Q_UNUSED(strMessage);
#ifdef _DEBUG
	strPaymentUrl = "http://222.143.21.159:9003/FSPayPlatform/payinfo/pay?src=RSTSBK&data=eydwYXlDb2RlJzogJzQxMDAwMDIxMTAwMDUwNzMwNTQ5JywgJ3RyYW5zdGltZSc6ICcyMDIxLTEyLTAzIDE2OjQzOjQxJywgJ25vdGlmeV9VUkwnOiAnaHR0cDovLzIyMi4xNDMuMzQuMTgvSG5QdWJsaWNTZXJ2aWNlQ29uc29sZS9ub250YXgvc3luY1BheVJlc3VsdEpRV1kubXZjJywgJ2RldmljZVR5cGUnOiAnMid9&noise=1638521021270&sign=Z26-1V3Nf7I6O1dDLgMwj08w6nzrCA6I5WF1cZmHrs71ZOL8q5v8DY_DzzLvb823aTHUEmwi7qP5wjPmCCIu8Q";
#endif
	return 0;
}

int     ApplyCardReplacement(QString& strMessage, int& nStatus)
{
	Q_UNUSED(strMessage);
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = applyCardReplacement(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		Region.strArea.c_str(),
		Region.strAgency.c_str(),
		(const char*)pSSCardInfo->strBankCode,
		(const char*)pIDCard->szIdentify,
		(const char*)pSSCardInfo->strCardNum,
		(const char*)pSSCardInfo->strMobile,
		(char*)pIDCard->szName,
		(char*)szStatus);
	if (QFailed(nResult))
	{
		strMessage = "申请补换卡失败";
		QString strInfo = QString("queryPayment Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "applyCardReplacement:" << szStatus;
	nStatus = strtol(szStatus, nullptr, 10);
	return 0;
}

int     CancelCardReplacement(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = cancelCardReplacement(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		Region.strArea.c_str(),
		(const char*)pIDCard->szIdentify,
		(char*)pIDCard->szName,
		(char*)szStatus);
	if (QFailed(nResult))
	{
		strMessage = "撤销申请补换卡失败";
		QString strInfo = QString("queryPayment Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "cancelCardReplacement:" << szStatus;
	nStatus = strtol(szStatus, nullptr, 10);
	return 0;
}

int     ResgisterPayment(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = registerPayment(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		(const char*)pIDCard->szIdentify,
		(char*)pIDCard->szName,
		Region.strArea.c_str(),
		(char*)szStatus);
	if (QFailed(nResult))
	{
		strMessage = "交费登记失败";
		QString strInfo = QString("registerPayment Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "registerPayment:" << szStatus;
	nStatus = strtol(szStatus, nullptr, 10);
	return 0;
}

int  CancelPayment(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = cancelPayment(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		(const char*)pIDCard->szIdentify,
		(char*)pIDCard->szName,
		(char*)szStatus);
	if (QFailed(nResult))
	{
		strMessage = "撤销缴费登记失败";
		QString strInfo = QString("queryPayment Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "cancelCardReplacement:" << szStatus;
	nStatus = strtol(szStatus, nullptr, 10);
	return 0;
}

int  MarkCard(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = markCard(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		Region.strArea.c_str(),
		(const char*)pIDCard->szIdentify,
		(char*)pIDCard->szName,
		(char*)szStatus);
	if (QFailed(nResult))
	{
		strMessage = "即制卡标注失败";
		QString strInfo = QString("markCard Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "markCard:" << szStatus;
	nStatus = strtol(szStatus, nullptr, 10);
	return 0;
}

int     CancelMarkCard(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = cancelMarkCard(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		Region.strArea.c_str(),
		(const char*)pIDCard->szIdentify,
		(char*)pIDCard->szName,
		(char*)szStatus);
	if (QFailed(nResult))
	{
		strMessage = "即制卡撤销标注失败";
		QString strInfo = QString("cancelMarkCard Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "cancelMarkCard:" << szStatus;
	nStatus = strtol(szStatus, nullptr, 10);
	return 0;
}

int     GetCardData(QString& strMessage, SSCardInfoPtr& pSSCardInfoOut)
{
	//IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfoIn = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = getCardData(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		*pSSCardInfoIn,
		*pSSCardInfoOut);
	if (QFailed(nResult))
	{
		strMessage = "获取制卡数据失败";
		QString strInfo = QString("getCardData Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "getCardData:" << szStatus;
	return 0;
}

int     ReturnCardData(QString& strMessage, SSCardInfoPtr& pSSCardInfoOut)
{
	//IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfoIn = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = returnCardData(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		*pSSCardInfoIn,
		*pSSCardInfoOut);
	if (QFailed(nResult))
	{
		strMessage = "制卡回盘失败";
		QString strInfo = QString("returnCardData Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "returnCardData:" << szStatus;
	return 0;
}

int     EnalbeCard(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfoIn = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = enableCard(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		Region.strArea.c_str(),
		(const char*)pIDCard->szIdentify,
		(const char*)pSSCardInfoIn->strCardNum,
		(const char*)pIDCard->szName,
		szStatus);
	if (QFailed(nResult))
	{
		strMessage = "领卡启用失败";
		QString strInfo = QString("enableCard Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "returnCardData:" << szStatus;
	return 0;
}

int     GetCA(QString& strMessage, QString& strCA)
{
	Q_UNUSED(strMessage);
	Q_UNUSED(strCA);
	return 0;
}
