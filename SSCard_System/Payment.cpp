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
int     QueryPayment(QString& strMessage, int& nStatus)
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
	// 	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
	// 		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	// 	{
	// 		nStatus = strtolong(szStatus, 10, 2);
	// 	}
	// 	else
	// 	{
	// 		strMessage = szStatus;
	// 		return -1;
	// 	}
	if (strcmp(_strupr(szStatus), "OK") == 0)
	{
		nStatus = Pay_Succeed;
	}
	else
		nStatus = Pay_Not;
	return 0;
}

int     RequestPaymentUrl(QString& strPaymentUrl, QString& strMessage)
{
	Q_UNUSED(strMessage);
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	if (!pIDCard)
	{
		strMessage = "身份证信息无效!";
		return -1;
	}
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!pSSCardInfo)
	{
		strMessage = "社保卡信息无效!";
		return -1;
	}

	PaymentOpt& PayOption = g_pDataCenter->GetSysConfigure()->PaymentConfig;
	//std::string szUrl = "http://10.126.131.203:8080/paymentbusiness/getPayCode?payerName=胡杰&payerTel=13637281234&payerCard=410700199907110037&amount=16";
	std::string strUrl = "http://";
	strUrl += PayOption.strHost;				strUrl += ":";
	strUrl += PayOption.strPort;				strUrl += "/";
	strUrl += PayOption.strPayUrl;				strUrl += "?";
	strUrl += PayOption.strFieldName;			strUrl += "=";
	strUrl += (const char*)pIDCard->szName;		strUrl += "&";
	strUrl += PayOption.strFieldMobile;			strUrl += "=";
	strUrl += pSSCardInfo->strMobile;			strUrl += "&";
	strUrl += PayOption.strFiledamount;			strUrl += "=";
	strUrl += PayOption.strAmount;
	gInfo() << "Payment request url = " << strUrl;

	string strRespond, strMessage1;
	if (SendHttpRequest(strUrl, strRespond, strMessage1))
	{
		gInfo() << strRespond;
		QString strRes = strRespond.c_str();
		QStringList strHttpRes = strRes.split("\r\n", Qt::SkipEmptyParts);

		QJsonParseError jsonParseError;
		QJsonDocument jsonDocument(QJsonDocument::fromJson(strHttpRes[5].toLatin1(), &jsonParseError));
		if (QJsonParseError::NoError != jsonParseError.error)
		{
			strMessage = QString("JsonParseError: %1").arg(jsonParseError.errorString());
			gInfo() << gQStr(strMessage);
			return -1;
		}
		QJsonObject rootObject = jsonDocument.object();
		if (!rootObject.keys().contains("msg"))
		{
			gInfo() << "There is no  No target value in the payment respond!";
			return -1;
		}

		QJsonValue jsonValue = rootObject.value("msg");
		strPaymentUrl = jsonValue.toString();
		return 0;
	}
	else
	{
		strMessage = strMessage1.c_str();
		return -1;
	}
}

int     ApplyCardReplacement(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = applyCardReplacement(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		Region.strArea.c_str(),
		Region.strAgency.c_str(),
		Region.strBankCode.c_str(),
		(const char*)pIDCard->szIdentify,
		(const char*)pSSCardInfo->strCardNum,
		g_pDataCenter->strMobilePhone.c_str(),
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
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		return 0;
	}
	else
	{
		strMessage = szStatus;
		return -1;
	}
}
// 目前在河南无权限 
int     CancelCardReplacement(QString& strMessage, int& nStatus)
{
	return 0;
	// 	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	// 	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	// 	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	// 	char szStatus[1024] = { 0 };
	// 	int nResult = cancelCardReplacement(Region.strCMAccount.c_str(),
	// 		Region.strCMPassword.c_str(),
	// 		Region.strArea.c_str(),
	// 		(const char*)pIDCard->szIdentify,
	// 		(char*)pIDCard->szName,
	// 		(char*)szStatus);
	// 	if (QFailed(nResult))
	// 	{
	// 		strMessage = "撤销申请补换卡失败";
	// 		QString strInfo = QString("queryPayment Failed:%1.").arg(nResult);
	// 		gInfo() << gQStr(strInfo);
	// 		return -1;
	// 	}
	// 	gInfo() << "cancelCardReplacement:" << szStatus;
	// 	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
	// 		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	// 	{
	// 		nStatus = strtolong(szStatus, 10, 2);
	// 		return 0;
	// 	}
	// 	else
	// 	{
	// 		strMessage = szStatus;
	// 		return -1;
	// 	}
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
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		return 0;
	}
	else
	{
		strMessage = szStatus;
		return -1;
	}
}

// 无须实现
int  CancelPayment(QString& strMessage, int& nStatus)
{
	//IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	//RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	//SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	//char szStatus[1024] = { 0 };
	//int nResult = cancelPayment(Region.strCMAccount.c_str(),
	//	Region.strCMPassword.c_str(),
	//	(const char*)pIDCard->szIdentify,
	//	(char*)pIDCard->szName,
	//	(char*)szStatus);
	//if (QFailed(nResult))
	//{
	//	strMessage = "撤销缴费登记失败";
	//	QString strInfo = QString("queryPayment Failed:%1.").arg(nResult);
	//	gInfo() << gQStr(strInfo);
	//	return -1;
	//}
	//gInfo() << "cancelCardReplacement:" << szStatus;
	//nStatus = strtolong(szStatus, 10, 2);
	return 0;
}

// nStatus = 0	时标注成功
//			 1	之前 已经标注
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
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		return 0;
	}
	else
	{
		strMessage = szStatus;
		return -1;
	}
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
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		return 0;
	}
	else
	{
		strMessage = szStatus;
		return -1;
	}
}

int     GetCardData(QString& strMessage, SSCardInfoPtr& pSSCardInfoOut)
{
	//IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfoIn = g_pDataCenter->GetSSCardInfo();
	strcpy(pSSCardInfoIn->strTransType, "5");
	string strCountry = Region.strArea + Region.strCountry;
	strcpy(pSSCardInfoIn->strSSQX, strCountry.c_str());
	strcpy(pSSCardInfoIn->strCard, Region.strCardVendor.c_str());
	strcpy(pSSCardInfoIn->strBankCode, Region.strBankCode.c_str());
	int nResult = getCardData(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		*pSSCardInfoIn,
		*pSSCardInfoOut);
	if (QFailed(nResult))
	{
		strMessage = "获取三代社保卡数据失败!";
		QString strInfo = QString("getCardData Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}

	return 0;
}

int     ReturnCardData(QString& strMessage, SSCardInfoPtr& pSSCardInfo)
{
	//IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfoIn = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	int nResult = returnCardData(Region.strCMAccount.c_str(), Region.strCMPassword.c_str(), *pSSCardInfo, szStatus);
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
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		return 0;
	}
	else
	{
		strMessage = szStatus;
		return -1;
	}
	return 0;
}
