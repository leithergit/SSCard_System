﻿#pragma execution_character_set("utf-8")
#include "Payment.h"

#include <atlenc.h>

#define PhotoBufferSize		256*1024
char* g_szPhotoBuffer = new char[PhotoBufferSize];

#include "QREncode/qrencode.h"
#include "Gloabal.h"
#include "../update/Update.h"


bool TestDigit(char ch)
{
	if (ch < '0' || ch > '9')
		return false;
	else
		return true;
}

void  SplitString(const char* szStr, char* szDigit, char* szText)
{
	char* p = (char*)szStr;
	do
	{
		if (!TestDigit(*p))
		{
			break;
		}
		else
			*szDigit++ = *p;
	} while (*p++);
	strcpy(szText, p);
}

int  QREnncodeImage(const QString& s, int bulk, QImage& QRImage)
{
	QRcode* qr = QRcode_encodeString(s.toUtf8(), 1, QR_ECLEVEL_Q, QR_MODE_KANJI, 1);
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
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	strcpy((char*)pSSCardInfo->strCardID, (const char*)pIDCard->szIdentity);
	strcpy((char*)pSSCardInfo->strName, (const char*)pIDCard->szName);
	char szStatus[1024] = { 0 };
	int nResult = queryPayment(*pSSCardInfo, (char*)szStatus);
	if (QFailed(nResult))
	{
		FailureMessage("查询付费状态", pSSCardInfo, szStatus, strMessage);
		gError() << gQStr(strMessage);
		QString strInfo = QString("queryPayment Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	QString strInfo = QString("queryPayment Succeed:%1.").arg(szStatus);
	gInfo() << gQStr(strInfo);

	if (strcmp(_strupr(szStatus), "OK") == 0)
	{
		nStatus = Pay_Succeed;
	}
	else
		nStatus = Pay_Not;
	return 0;
}


void	GBKStringtoUrlString(const char* szGBKString, string& strUrlString)
{
	strUrlString = "";
	QString strUtf8String = QString::fromLocal8Bit(szGBKString);
	string strStdUtf8String = strUtf8String.toStdString();
	for (auto var : strStdUtf8String)
	{
		char szHex[4] = { 0 };
		sprintf(szHex, "%%%02X", (unsigned char)var);
		strUrlString += szHex;
	}
}

int     RequestPaymentUrl(QString& strPaymentUrl, QString& strPayCode, QString& strMessage)
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
	strUrl += PayOption.strPayUrl;				//strUrl += "?";
	strUrl += PayOption.strFieldName;			strUrl += "=";
	string strNameUrl;
	GBKStringtoUrlString((const char*)pIDCard->szName, strNameUrl);
	strUrl += strNameUrl;						strUrl += "&";
	strUrl += PayOption.strFieldMobile;			strUrl += "=";
	strUrl += g_pDataCenter->strMobilePhone;	strUrl += "&";
	strUrl += PayOption.strFieldCardID;			strUrl += "=";
	strUrl += (char*)pIDCard->szIdentity;		strUrl += "&";
	strUrl += PayOption.strFiledamount;			strUrl += "=";
	strUrl += PayOption.strAmount;
	gInfo() << "Payment request url = " << strUrl.c_str();

	string strRespond, strMessage1;
	int nRetry = 0;
	do
	{
		if (SendHttpRequest("GET", strUrl, strRespond, strMessage1) == 0)
		{
			gInfo() << strRespond;
			QString strRes = strRespond.c_str();
			//QStringList strHttpRes = strRes.split("\r\n", Qt::SkipEmptyParts);
			QJsonParseError jsonParseError;
			QJsonDocument jsonDocument(QJsonDocument::fromJson(strRes.toLatin1(), &jsonParseError));
			if (QJsonParseError::NoError != jsonParseError.error)
			{
				strMessage = QString("JsonParseError: %1").arg(jsonParseError.errorString());
				gInfo() << gQStr(strMessage);
				return -1;
			}
			QJsonObject rootObject = jsonDocument.object();
			if (!rootObject.keys().contains("msg") ||
				!rootObject.keys().contains("code") ||
				!rootObject.keys().contains("payCode"))
			{
				gInfo() << "There is no  No target value in the payment respond!";
				return -1;
			}
			QJsonValue jsCode = rootObject.value("code");
			QJsonValue jsMsg = rootObject.value("msg");
			QJsonValue jsPayCode = rootObject.value("payCode");
			int nCode = jsCode.toInt();
			if (nCode != 0)
			{
				strMessage = QString("查询结果,code = %1").arg(nCode);
				gInfo() << gQStr(strMessage);
				return -1;
			}
			strPayCode = jsPayCode.toString();
			strPaymentUrl = jsMsg.toString();
			gInfo() << gQStr(strPaymentUrl);
			return 0;
		}
		else
		{
			strMessage = strMessage1.c_str();
			gError() << gQStr(strMessage);
			nRetry++;
		}
	} while (nRetry < 3);
	return -1;
}

int     RequestPaymentUrl2(QString& strPaymentUrl, QString& strPayCode, QString& strTransTime, QString& strMessage)
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
	// GET http://ip:port/paymentbusiness/getPayCode?payerName={name}&payerTel={tel}&payerCard={cardID}&amount={amount}&orgCode={orgCode}&socialCard={socialCard}
	PaymentOpt& PayOption = g_pDataCenter->GetSysConfigure()->PaymentConfig;
	RegionInfo& RegInfo = g_pDataCenter->GetSysConfigure()->Region;
	//std::string szUrl = "http://10.126.131.203:8080/paymentbusiness/getPayCode?payerName=胡杰&payerTel=13637281234&payerCard=410700199907110037&amount=16";
	std::string strUrl = "http://";
	strUrl += PayOption.strHost;				strUrl += ":";
	strUrl += PayOption.strPort;
	strUrl += "/paymentbusiness/getPayCode?payerName=";
	string strNameUrl;
	GBKStringtoUrlString((const char*)pIDCard->szName, strNameUrl);
	strUrl += strNameUrl;						strUrl += "&";
	strUrl += "payerTel=";
	strUrl += g_pDataCenter->strMobilePhone;	strUrl += "&";
	strUrl += "payerCard=";
	strUrl += (char*)pIDCard->szIdentity;		strUrl += "&";
	strUrl += "amount=";
	strUrl += PayOption.strAmount;				strUrl += "&";
	strUrl += "orgCode=";
	strUrl += RegInfo.strAgency;				strUrl += "&";
	strUrl +="socialCard=";	
	strUrl += pSSCardInfo->strCardNum;			strUrl += "&";
	strUrl += "uniqueId=";
	strUrl += RegInfo.strUniqueID;
	gInfo() << "Payment request url = " << strUrl.c_str();

	string strRespond, strMessage1;
	int nRetry = 0;
	do
	{
		if (SendHttpRequest("GET", strUrl, strRespond, strMessage1) == 0)
		{
			gInfo() << strRespond;
			QString strRes = strRespond.c_str();
			//QStringList strHttpRes = strRes.split("\r\n", Qt::SkipEmptyParts);
			QJsonParseError jsonParseError;
			QJsonDocument jsonDocument(QJsonDocument::fromJson(strRes.toLatin1(), &jsonParseError));
			if (QJsonParseError::NoError != jsonParseError.error)
			{
				strMessage = QString("JsonParseError: %1").arg(jsonParseError.errorString());
				gInfo() << gQStr(strMessage);
				return -1;
			}
			QJsonObject rootObject = jsonDocument.object();
			if (!rootObject.keys().contains("msg") ||
				!rootObject.keys().contains("code"))
			{
				gInfo() << "There is no msg or code field in the respond!";
				return -1;
			}
			QString strCode = rootObject.value("code").toString();
            QString strMsg = rootObject.value("msg").toString();
			gInfo() << "Payment message:" << strMsg.toStdString();
			int nCode = strCode.toInt();
			if (nCode == -1)
			{
				strMessage = strMsg;
				gInfo() << "returned Payment message:" << strMessage.toStdString();
				return -1;
			}
			if (!rootObject.keys().contains("payCode") ||
				!rootObject.keys().contains("transTime"))
			{
				gInfo() << "There is no payCode or transTime field in the respond!";
				return -1;
			}
			QJsonValue jsPayCode = rootObject.value("payCode");
			QJsonValue jsTransTime = rootObject.value("transTime");
			
			if (nCode != 0)
			{
				strMessage = QString("查询结果,code = %1").arg(nCode);
				gInfo() << gQStr(strMessage);
				return -1;
			}
			strPayCode = jsPayCode.toString();
			strPaymentUrl = strMsg;
			strTransTime = jsTransTime.toString();
			gInfo() << gQStr(strPaymentUrl);
			return 0;
		}
		else
		{
			strMessage = strMessage1.c_str();
			gError() << gQStr(strMessage);
			nRetry++;
		}
	} while (nRetry < 3);
	return -1;
}

// 查询支付结果
// nStatus:0 - 待支付 1 - 待确认 2 - 支付完成 3 - 支付失败 4 - 订单不存在 5 - 订单取消
int  queryPayResult(string& strPayCode, QString& strMessage, PayResult& nStatus)
{
	// http://ip:port/paymentbusiness/pollForPayResult?payCode=code
	PaymentOpt& PayOption = g_pDataCenter->GetSysConfigure()->PaymentConfig;
	//std::string szUrl = "http://10.126.131.203:8080/paymentbusiness/getPayCode?payerName=胡杰&payerTel=13637281234&payerCard=410700199907110037&amount=16";
	// http://10.125.40.244:8080/paymentbusiness/pollForPayResult?payCode=4100002110005553036m
	std::string strUrl = "http://";
	strUrl += PayOption.strHost;				strUrl += ":";
	strUrl += PayOption.strPort;				strUrl += "/";
	strUrl += PayOption.strPayResultUrl;		//strUrl += "?";
	strUrl += strPayCode;

	gInfo() << "PayResult request url = " << strUrl.c_str();

	string strRespond, strMessage1;
	if (QSucceed(SendHttpRequest("GET", strUrl, strRespond, strMessage1)))
	{
		gInfo() << strRespond;
		QString strHttpRes = strRespond.c_str();
		//QStringList strHttpRes = strRes.split("\r\n", Qt::SkipEmptyParts);
		QJsonParseError jsonParseError;
		QJsonDocument jsonDocument(QJsonDocument::fromJson(strHttpRes.toLatin1(), &jsonParseError));
		if (QJsonParseError::NoError != jsonParseError.error)
		{
			strMessage = QString("JsonParseError: %1").arg(jsonParseError.errorString());
			gInfo() << gQStr(strMessage);
			return -1;
		}
		QJsonObject rootObject = jsonDocument.object();
		if (!rootObject.keys().contains("msg") ||
			!rootObject.keys().contains("code"))
		{
			gInfo() << "There is no  No target value in the PayResult respond!";
			return -1;
		}
		nStatus = (PayResult)rootObject.value("code").toInt();
		strMessage = rootObject.value("msg").toString();
		nStatus = (PayResult)strMessage.toInt();
		gInfo() << "code=" << (int)nStatus << gQStr(strMessage);
		return 0;
	}
	else
	{
		strMessage = strMessage1.c_str();
		gError() << gQStr(strMessage);
		return -1;
	}
}

string string2Url(string strUrl)
{
	string strReturn;
	char szBuffer[16] = { 0 };
	for (auto var : strUrl)
	{
		if (('0' <= var && var <= '9') ||
			('a' <= var && var <= 'z') ||
			('A' <= var && var <= 'Z') ||
			(var == '/' || var == '.'))
			strReturn += var;
		else
		{
			sprintf_s(szBuffer, 16, "%02X", (unsigned char)var);
			strReturn += '%';
			strReturn += szBuffer;
			ZeroMemory(szBuffer, 16);
		}
	}
	return strReturn;
}

int  queryPayResult2(string& strPayCode, string& strTransTime, QString& strMessage, PayResult& nStatus)
{
	// http://ip:port/paymentbusiness/pollForPayResult?payCode=code
	PaymentOpt& PayOption = g_pDataCenter->GetSysConfigure()->PaymentConfig;
	//std::string szUrl = "http://10.126.131.203:8080/paymentbusiness/getPayCode?payerName=胡杰&payerTel=13637281234&payerCard=410700199907110037&amount=16";
	// http://10.125.40.244:8080/paymentbusiness/pollForPayResult?payCode=4100002110005553036m
	std::string strUrl = "http://";
	strUrl += PayOption.strHost;				strUrl += ":";
	strUrl += PayOption.strPort;
	strUrl += "/paymentbusiness/queryPayResult?payCode=";
	strUrl += strPayCode;
	strUrl += "&transTime=";
	
	strUrl += string2Url(strTransTime);

	gInfo() << "PayResult request url = " << strUrl.c_str();
	// 以下为测试用url已付费
	// strUrl = "http://10.126.131.203:8080/paymentbusiness/queryPayResult?payCode=41000022100088855563&transTime=2022%2D08%2D01%2018%3A06%3A59";

	string strRespond, strMessage1;
	// http://10.126.131.203:8080/paymentbusiness/queryPayResult?payCode=4100002210008802544h&transTime=2022-07-25 15:23:39
	if (QSucceed(SendHttpRequest("GET", strUrl, strRespond, strMessage1)))
	{
		gInfo() << strRespond;
		QString strHttpRes = strRespond.c_str();
		//QStringList strHttpRes = strRes.split("\r\n", Qt::SkipEmptyParts);
		QJsonParseError jsonParseError;
		QJsonDocument jsonDocument(QJsonDocument::fromJson(strHttpRes.toLatin1(), &jsonParseError));
		if (QJsonParseError::NoError != jsonParseError.error)
		{
			strMessage = QString("JsonParseError: %1").arg(jsonParseError.errorString());
			gInfo() << gQStr(strMessage);
			return -1;
		}
		QJsonObject rootObject = jsonDocument.object();
		if (!rootObject.keys().contains("msg") ||
			!rootObject.keys().contains("code") ||
			!rootObject.keys().contains("status"))
		{
			gInfo() << "There is no  No target value in the PayResult respond!";
			return -1;
		}
		int nCode = rootObject.value("code").toInt();
		if (nCode)
		{
			gError() << "pay code:" << nCode << ",返回代码非0";
			return -1;
		}
		strMessage = rootObject.value("msg").toString();
		
		jsonDocument = QJsonDocument::fromJson(strMessage.toLatin1(), &jsonParseError);
		rootObject = jsonDocument.object();
		strPayCode = rootObject.value("payCode").toString().toStdString();
		strTransTime = rootObject.value("transtime").toString().toStdString();
		QString strStatus = rootObject.value("stats").toString();
		nStatus = (PayResult)strStatus.toUInt();

		gInfo() << gQStr(strMessage);
		return 0;
	}
	else
	{
		strMessage = strMessage1.c_str();
		gError() << gQStr(strMessage);
		return -1;
	}
}

// nStatus = 0,成功，否则失败
int  ApplyNewCard(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	char szStatus[1024] = { 0 };
	int nResult = applyNewCard(*pSSCardInfo, (char*)szStatus);
	if (QFailed(nResult))
	{
		FailureMessage("申请新制卡", pSSCardInfo, szStatus, strMessage);
		gError() << gQStr(strMessage);
		return -1;
	}
	gInfo() << "applyNewCard:" << szStatus;
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
	{
		strMessage = QString::fromLocal8Bit(szText);
		if (strMessage == "已提交过办卡申请")
		{
			nStatus = 0;
			return 0;
		}
	}

	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

int  ApplyCardReplacement(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	char szStatus[1024] = { 0 };
	int nResult = applyCardReplacement(*pSSCardInfo, (char*)szStatus);
	if (QFailed(nResult))
	{
		FailureMessage("申请补换卡", pSSCardInfo, szStatus, strMessage);
		gError() << gQStr(strMessage);
		return -1;
	}
	gInfo() << "applyCardReplacement:" << szStatus;
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

// 目前在河南无权限 
int  CancelCardReplacement(QString& strMessage, int& nStatus)
{
	return 0;
	// 	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	// 	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	// 	SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	// 	char szStatus[1024] = { 0 };
	// 	int nResult = cancelCardReplacement(Region.strCMAccount.c_str(),
	// 		Region.strCMPassword.c_str(),
	// 		Region.strArea.c_str(),
	// 		(const char*)pIDCard->szIdentity,
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

int  ResgisterPayment(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	char szStatus[1024] = { 0 };
	int nResult = registerPayment(*pSSCardInfo, (char*)szStatus);
	if (QFailed(nResult))
	{
		FailureMessage("交费登记", pSSCardInfo, szStatus, strMessage);
		gError() << gQStr(strMessage);
		QString strInfo = QString("registerPayment Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}
	gInfo() << "registerPayment:" << szStatus;
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
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
	//	(const char*)pIDCard->szIdentity,
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
int  MarkCard(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	char szStatus[1024] = { 0 };
	QString strInfo = QString("Try to markCard(%1,%2)").arg((const char*)pSSCardInfo->strName).arg((char*)pSSCardInfo->strCardID);
	gInfo() << gQStr(strInfo);
	int nResult = markCard(*pSSCardInfo, (char*)szStatus);
	if (QFailed(nResult))
	{
		FailureMessage("即制卡标注", pSSCardInfo, szStatus, strMessage);
		gError() << gQStr(strMessage);
		return -1;
	}
	gInfo() << "markCard:" << szStatus;
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

int     CancelMarkCard(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	char szStatus[1024] = { 0 };
	int nResult = cancelMarkCard(*pSSCardInfo, (char*)szStatus);
	if (QFailed(nResult))
	{
		FailureMessage("即制卡撤销标注", pSSCardInfo, szStatus, strMessage);
		gError() << gQStr(strMessage);
		return -1;
	}
	gInfo() << "cancelMarkCard:" << szStatus;
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

void SaveCardDataField(const char* szField, char* szValue, QString strINIFile)
{
	QSettings CardIni(strINIFile, QSettings::IniFormat);
	CardIni.beginGroup("CardData");
	CardIni.setValue(szField, szValue);
	CardIni.endGroup();
}

int GetCardDataField(const char* szField, char* pValue, QString strINIFile)
{
	QFileInfo fi(strINIFile);
	if (!fi.isFile())
		return -1;

	QSettings CardIni(strINIFile, QSettings::IniFormat);
	CardIni.beginGroup("CardData");
	strcpy(pValue, CardIni.value(szField).toString().toStdString().c_str());
	CardIni.endGroup();
	if (strlen(pValue) == 0)
		return -1;
	return 0;
}

int GetCardDataFieldp(const char* szField, string& strValue, QString strINIFile)
{
	QFileInfo fi(strINIFile);
	if (!fi.isFile())
		return -1;

	QSettings CardIni(strINIFile, QSettings::IniFormat);
	CardIni.beginGroup("CardData");
	strValue = CardIni.value(szField).toString().toStdString();
	CardIni.endGroup();
	if (!strValue.size())
		return -1;
	return 0;
}

/*
int SaveCardData(SSCardInfoPtr& pSSCardInfoOut, QString strINIFile)
{
#define AddCardFiled(x,s)	x.setValue(#s,pSSCardInfoOut->s);
	QSettings CardIni(strINIFile, QSettings::IniFormat);
	CardIni.beginGroup("CardData");
	AddCardFiled(CardIni, strOrganID);
	AddCardFiled(CardIni, strCardNum);
	AddCardFiled(CardIni, strCardID);
	AddCardFiled(CardIni, strBirthday);
	AddCardFiled(CardIni, strName);
	AddCardFiled(CardIni, strSex);
	AddCardFiled(CardIni, strNation);
	AddCardFiled(CardIni, strMobile);
	AddCardFiled(CardIni, strAddress);
	AddCardFiled(CardIni, strPostalCode);
	AddCardFiled(CardIni, strEmail);
	AddCardFiled(CardIni, strCommunity);
	AddCardFiled(CardIni, strCompanyName);
	AddCardFiled(CardIni, strGuardianName);
	AddCardFiled(CardIni, strTransType);
	AddCardFiled(CardIni, strCity);
	AddCardFiled(CardIni, strSSQX);
	AddCardFiled(CardIni, strBankCode);
	AddCardFiled(CardIni, strCityAccTime);
	AddCardFiled(CardIni, strCityInfo);
	AddCardFiled(CardIni, strCardStatus);
	AddCardFiled(CardIni, strCard);
	AddCardFiled(CardIni, strReleaseDate);
	AddCardFiled(CardIni, strValidDate);
	AddCardFiled(CardIni, strPhoto);
	AddCardFiled(CardIni, strIdentifyNum);
	AddCardFiled(CardIni, strCardATR);
	AddCardFiled(CardIni, strBankNum);
	AddCardFiled(CardIni, strPCH);

	CardIni.endGroup();
	return 0;
}
*/

/*
int LoadSSCardData(SSCardInfoPtr& pSSCardInfoOut, QString strINIFile)
{
	QFileInfo fi(strINIFile);
	if (!fi.isFile())
	{
		return -1;
	}
#define GetCardField(x,c,f)		strcpy(c->f, x.value(#f).toString().toStdString().c_str());
	QSettings CardIni(strINIFile, QSettings::IniFormat);
	strcpy(pSSCardInfoOut->strCardNum, CardIni.value("").toString().toStdString().c_str());
	CardIni.beginGroup("CardData");
	GetCardField(CardIni, pSSCardInfoOut, strCardNum);
	GetCardField(CardIni, pSSCardInfoOut, strCardID);
	GetCardField(CardIni, pSSCardInfoOut, strBirthday);
	GetCardField(CardIni, pSSCardInfoOut, strName);
	GetCardField(CardIni, pSSCardInfoOut, strSex);
	GetCardField(CardIni, pSSCardInfoOut, strNation);
	GetCardField(CardIni, pSSCardInfoOut, strMobile);
	GetCardField(CardIni, pSSCardInfoOut, strAddress);
	GetCardField(CardIni, pSSCardInfoOut, strPostalCode);
	GetCardField(CardIni, pSSCardInfoOut, strEmail);
	GetCardField(CardIni, pSSCardInfoOut, strCommunity);
	GetCardField(CardIni, pSSCardInfoOut, strCompanyName);
	GetCardField(CardIni, pSSCardInfoOut, strGuardianName);
	GetCardField(CardIni, pSSCardInfoOut, strTransType);
	GetCardField(CardIni, pSSCardInfoOut, strCity);
	GetCardField(CardIni, pSSCardInfoOut, strSSQX);
	GetCardField(CardIni, pSSCardInfoOut, strBankCode);
	GetCardField(CardIni, pSSCardInfoOut, strCityAccTime);
	GetCardField(CardIni, pSSCardInfoOut, strCityInfo);
	GetCardField(CardIni, pSSCardInfoOut, strCardStatus);
	GetCardField(CardIni, pSSCardInfoOut, strCard);
	GetCardField(CardIni, pSSCardInfoOut, strReleaseDate);
	GetCardField(CardIni, pSSCardInfoOut, strValidDate);

	g_pDataCenter->strPhotoBase64 = CardIni.value("strPhoto").toString().toStdString();
	pSSCardInfoOut->strPhoto = (char*)g_pDataCenter->strPhotoBase64.c_str();
	GetCardField(CardIni, pSSCardInfoOut, strIdentifyNum);
	GetCardField(CardIni, pSSCardInfoOut, strCardATR);
	GetCardField(CardIni, pSSCardInfoOut, strBankNum);
	GetCardField(CardIni, pSSCardInfoOut, strPCH);
	CardIni.endGroup();
	return 0;
}
*/
int  GetImageStorePath(string& strFilePath, int nType)
{
	QString strStorePath = QCoreApplication::applicationDirPath();
	strStorePath += "/IDImage/";
	strStorePath += QDateTime::currentDateTime().toString("yyyyMMdd/");
	QFileInfo fi(strStorePath);
	if (!fi.isDir())
	{// 当天目录不存在？则创建目录
		QDir storeDir;
		if (!storeDir.mkpath(strStorePath))
		{
			char szError[1024] = { 0 };
			_strerror_s(szError, 1024, nullptr);
			QString Info = QString("创建身份证照片保存目录'%1'失败:%2").arg(strStorePath, szError);
			gInfo() << Info.toLocal8Bit().data();
			return -1;
		}
	}
	QString strTempPath;
	if (nType == 0)
		strTempPath = strStorePath + QString("ID_%1.jpg").arg((const char*)g_pDataCenter->GetIDCardInfo()->szIdentity);
	else
		strTempPath = strStorePath + QString("ID_%1_%2.jpg").arg((const char*)g_pDataCenter->GetIDCardInfo()->szIdentity).arg((const char*)g_pDataCenter->GetSSCardInfo()->strCardNum);

	strFilePath = strTempPath.toStdString();
	return 0;
}

int SaveSSCardPhoto(QString strMessage, const char* szPhotoBase64)
{
	if (!szPhotoBase64)
	{
		return -1;
	}
	QFileInfo fis(g_pDataCenter->strSSCardPhotoFile.c_str());
	if (fis.isFile())
	{
		gInfo() << "已经保存过社保卡照片";
		return 0;
	}
	ZeroMemory(g_szPhotoBuffer, PhotoBufferSize);
	int nPhotoSize = PhotoBufferSize;
	Base64Decode(szPhotoBase64, strlen(szPhotoBase64), (BYTE*)g_szPhotoBuffer, &nPhotoSize);
	QImage photo = QImage::fromData((const uchar*)g_szPhotoBuffer, nPhotoSize);
	if (photo.isNull())
		return -1;

	string strPhotoPath;
	GetImageStorePath(strPhotoPath, 1);
	gInfo() << "Try Save SSCard Photo:" << strPhotoPath;
	photo.save(strPhotoPath.c_str(), "JPG", 90);
	QFileInfo fi(strPhotoPath.c_str());
	if (!fi.isFile())
		return -1;
	g_pDataCenter->strSSCardPhotoFile = strPhotoPath.c_str();

	return 0;
}

// nStatus 返回0为成功，其它都为失败
int  GetCardData(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo, bool bSkipPreStep)
{
	bool bLoaded = false;
	QString strDebugCardData = QCoreApplication::applicationDirPath();
	strDebugCardData += "/Debug";
	QFileInfo fdir(strDebugCardData);
	if (fdir.exists())
	{
		if (!fdir.isDir())
		{
			QFile f(strDebugCardData);
			f.remove();
		}
	}
	else
	{
		QDir dir;
		dir.mkdir(strDebugCardData);
	}
	
	strDebugCardData += QString("/TestPersonData_%1.json").arg(pSSCardInfo->strCardID);
	string strStdDebugCardData = strDebugCardData.toLocal8Bit().data();
	if (g_pDataCenter->bDebug)
	{
		do 
		{
			QFileInfo ffile(strDebugCardData);
			if (ffile.isFile())
			{
	#pragma Warning("使用预存制卡数据")
				SSCardInfoPtr pSSCardTemp = make_shared<SSCardInfo>();
				auto optJson = g_pDataCenter->OpenProgress(strStdDebugCardData);
				if (!optJson)
				{
					gError() << "Failed in open progress file " << strStdDebugCardData;
					break;
				}
				if (g_pDataCenter->GetProgress(pSSCardTemp, optJson.value()))
				{
					gError() << "Failed in GetProgress from file " << strStdDebugCardData;
					break;
				}
				//LoadSSCardData(pSSCardTemp, strDebugCardData);
				strcpy(pSSCardInfo->strPCH, pSSCardTemp->strPCH);
				strcpy(pSSCardInfo->strCardNum, pSSCardTemp->strCardNum);		// 新的卡号
				strcpy(pSSCardInfo->strNation, pSSCardTemp->strNation);
				strcpy(pSSCardInfo->strSex, pSSCardTemp->strSex);
				strcpy(pSSCardInfo->strBirthday, pSSCardTemp->strBirthday);
				strcpy(pSSCardInfo->strReleaseDate, pSSCardTemp->strReleaseDate);
				strcpy(pSSCardInfo->strValidDate, pSSCardTemp->strValidDate);
				pSSCardInfo->strPhoto = pSSCardTemp->strPhoto;
				if (strlen(pSSCardInfo->strPhoto))
				{
					SaveSSCardPhoto(strMessage, pSSCardInfo->strPhoto);
				}

				bLoaded = true;
			}
		} while (0);
	}
	if (!bLoaded)
	{
		char szStatus[1014] = { 0 };
		int nResult = -1;
		string strFnName = "";
		do
		{
			strFnName = "即制卡人数";
			if (QFailed(nResult = getJZKRS(*pSSCardInfo, szStatus)))
				break;

			strFnName = "即制卡人员";
			if (QFailed(nResult = getJZKRY(*pSSCardInfo, szStatus)))
				break;
			using LPCHAR = char*;
			strFnName = "获取批次号";
			if (!g_pDataCenter->GetProgressField("PCH",(char*)pSSCardInfo->strPCH) || 
				strlen(pSSCardInfo->strPCH) == 0)
			{
				if (QFailed(nResult = getHQPCH(*pSSCardInfo, szStatus)))
					break;
				g_pDataCenter->SetProgressField("PCH", pSSCardInfo->strPCH);
			}
			else
			{
				gInfo() << GBKStr(strFnName.c_str()) << GBKStr("使用已有批次号:") << pSSCardInfo->strPCH;
				nResult = 0;
			}

			strFnName = "即制卡批次";
			if (!g_pDataCenter->GetProgressField("CardNum", (char*)pSSCardInfo->strCardNum))
			{
				if (QFailed(nResult = getJZKPC(*pSSCardInfo, szStatus)))
				{
					strMessage = QString::fromLocal8Bit(szStatus);
					if (!strMessage.contains("已经有批次号") || !strMessage.contains("批次号已经被使用"))
						break;
				}
				g_pDataCenter->SetProgressField("CardNum", pSSCardInfo->strCardNum);
			}
			else
			{
				gInfo() << GBKStr(strFnName.c_str()) << GBKStr("使用已有新卡号:") << pSSCardInfo->strCardNum;
				nResult = 0;
			}

			strFnName = "获取制卡数据";
			string strPhoto;
			if (!g_pDataCenter->GetProgressField("Photo", strPhoto))
			{
				// 可返回照片信息
				nResult = getCardData(*pSSCardInfo, szStatus);
				if (QFailed(nResult))
				{
					if (strcmp(szStatus, "08") == 0)
					{
						FailureMessage(strFnName.c_str(), pSSCardInfo, szStatus, strMessage);
						gError() << gQStr(strMessage);
					}
					else
					{
						strMessage = QString::fromLocal8Bit(szStatus);
						QString strInfo = QString("getCardData Failed:%1.").arg(nResult);
						gInfo() << gQStr(strInfo);
					}
					return -1;
				}
				char szDigit[16] = { 0 }, szText[1024] = { 0 };
				SplitString(szStatus, szDigit, szText);
				if (strlen(szText))
					strMessage = QString::fromLocal8Bit(szText);
				if (strlen(szDigit))
				{
					nStatus = strtolong(szDigit, 10);
					if (nStatus == 0)
					{
						g_pDataCenter->SetProgress(pSSCardInfo);
						SaveSSCardPhoto(strMessage, pSSCardInfo->strPhoto);
					}
					return 0;
				}
				else
					break;
			}
			else
			{
				/*SSCardInfoPtr pSSCardTemp = make_shared<SSCardInfo>();
				auto optJson = g_pDataCenter->OpenProgress(strStdDebugCardData);
				if (!optJson)
				{
					gError() << "Failed in open progress file " << strStdDebugCardData;
					return -1;
				}
				if (g_pDataCenter->GetProgress(pSSCardTemp, optJson.value()))
				{
					gError() << "Failed in GetProgress from file " << strStdDebugCardData;
					return -1;
				}*/
				//LoadSSCardData(pSSCardTemp, strCardDataPath);
				g_pDataCenter->GetProgressField("NationCode", (char *)pSSCardInfo->strNation);
				g_pDataCenter->GetProgressField("Sex", (char*)pSSCardInfo->strSex);
				g_pDataCenter->GetProgressField("Birthday", (char*)pSSCardInfo->strBirthday);
				g_pDataCenter->GetProgressField("ReleaseDate", (char*)pSSCardInfo->strReleaseDate);
				g_pDataCenter->GetProgressField("ValidDate", (char*)pSSCardInfo->strValidDate);

				g_pDataCenter->strPhotoBase64 = strPhoto;
				pSSCardInfo->strPhoto = (char*)strPhoto.c_str();
				SaveSSCardPhoto(strMessage, pSSCardInfo->strPhoto);
				gInfo() << GBKStr(strFnName.c_str()) << GBKStr("使用已有照片数据.");
				nResult = 0;
			}
		} while (0);
		if (nResult == 8)
		{
			FailureMessage(strFnName.c_str(), pSSCardInfo, szStatus, strMessage);
		}
		else
			strMessage = QString::fromLocal8Bit(szStatus);
		return nResult;
	}
	//#ifdef _DEBUG
	//	if (ffile.isFile())
	//	{
	//#pragma Warning("使用预存制卡数据")
	//		LoadCardData(pSSCardInfoOut, strAppPath);
	//		strcpy(pSSCardInfo->strPCH, pSSCardInfoOut->strPCH);
	//		strcpy(pSSCardInfo->strCardNum, pSSCardInfoOut->strCardNum);		// 新的卡号
	//		strcpy(pSSCardInfo->strNation, pSSCardInfoOut->strNation);
	//		strcpy(pSSCardInfo->strSex, pSSCardInfoOut->strSex);
	//		strcpy(pSSCardInfo->strBirthday, pSSCardInfoOut->strBirthday);
	//		strcpy(pSSCardInfo->strReleaseDate, pSSCardInfoOut->strReleaseDate);
	//		strcpy(pSSCardInfo->strValidDate, pSSCardInfoOut->strValidDate);
	//		pSSCardInfoIn->strPhoto = pSSCardInfoOut->strPhoto;
	//		SaveSSCardPhoto(strMessage, pSSCardInfo->strPhoto);
	//	}
	//	else
	//	{
	//		char szStatus[1024] = { 0 };
	//		int nResult = getCardData(Region.strCMAccount.c_str(),
	//			Region.strCMPassword.c_str(),
	//			*pSSCardInfoIn,
	//			*pSSCardInfoOut,
	//			szStatus);
	//		if (QFailed(nResult))
	//		{
	//			strMessage = "获取三代社保卡数据失败!";
	//			QString strInfo = QString("getCardData Failed:%1.").arg(nResult);
	//			gInfo() << gQStr(strInfo);
	//			return -1;
	//		}
	//
	//		if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
	//			(szStatus[1] >= '0' && szStatus[1] <= '9'))
	//		{
	//			nStatus = strtolong(szStatus, 10, 2);
	//			if (nStatus == 0)
	//			{
	//				strcpy(pSSCardInfo->strPCH, pSSCardInfoOut->strPCH);
	//				strcpy(pSSCardInfo->strCardNum, pSSCardInfoOut->strCardNum);		// 新的卡号
	//				strcpy(pSSCardInfo->strNation, pSSCardInfoOut->strNation);
	//				strcpy(pSSCardInfo->strSex, pSSCardInfoOut->strSex);
	//				strcpy(pSSCardInfo->strBirthday, pSSCardInfoOut->strBirthday);
	//				strcpy(pSSCardInfo->strReleaseDate, pSSCardInfoOut->strReleaseDate);
	//				strcpy(pSSCardInfo->strValidDate, pSSCardInfoOut->strValidDate);
	//				pSSCardInfoIn->strPhoto = pSSCardInfoOut->strPhoto;
	//				SaveCardData(pSSCardInfoIn, strAppPath);
	//				SaveSSCardPhoto(strMessage, pSSCardInfoIn->strPhoto);
	//			}
	//			return 0;
	//		}
	//		else
	//		{
	//			strMessage = QString::fromLocal8Bit(szStatus);
	//			return -1;
	//		}
	//	}
	//#else
	//#endif
	return 0;
}

int  ReturnCardData(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo, bool bFailed)
{
	char szStatus[1024] = { 0 };
	if (bFailed)
	{
		strcpy(pSSCardInfo->strFailReason, "Withdraw");
		strcpy(pSSCardInfo->strFailType, QString("制卡").toLocal8Bit().toStdString().c_str());
	}

	int nResult = returnCardData(*pSSCardInfo, szStatus);
	if (QFailed(nResult))
	{
		FailureMessage("数据回盘", pSSCardInfo, szStatus, strMessage);
		gInfo() << gQStr(strMessage);
		return -1;
	}
	gInfo() << "returnCardData:" << szStatus;
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

int  EnalbeCard(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	char szStatus[1024] = { 0 };
	int nResult = enableCard(*pSSCardInfo, szStatus);
	if (QFailed(nResult))
	{
		FailureMessage("领卡启用", pSSCardInfo, szStatus, strMessage);
		gInfo() << gQStr(strMessage);
		return -1;
	}
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

void RemoveCardData(SSCardInfoPtr& pSSCardInfo)
{
	try
	{
		if (!pSSCardInfo)
			return;

		QString strCardDataPath = QCoreApplication::applicationDirPath();
		strCardDataPath += "/Debug";

		QFileInfo fdir(strCardDataPath);
		if (fdir.exists())
		{
			if (!fdir.isDir())
			{
				QFile f(strCardDataPath);
				f.remove();
			}
		}
		else
		{
			QDir dir;
			dir.mkdir(strCardDataPath);
		}
		QString strFinishPath = strCardDataPath + "/Finished";
		QFileInfo ffdir(strFinishPath);
		if (ffdir.exists())
		{
			if (!ffdir.isDir())
			{
				QFile f(strFinishPath);
				f.remove();
			}
		}
		else
		{
			QDir dir;
			dir.mkdir(strFinishPath);
		}

		strCardDataPath += QString("/Carddata_%1.ini").arg(pSSCardInfo->strCardID);
		strFinishPath += QString("/Carddata_%1.ini").arg(pSSCardInfo->strCardID);
		if (!fs::exists(strCardDataPath.toStdString()))
			return;

		if (fs::exists(strFinishPath.toStdString()))
			fs::remove(strFinishPath.toStdString());

		string strBaseFile = strFinishPath.toLocal8Bit().data();
		fs::copy_file(strCardDataPath.toStdString(), strFinishPath.toStdString());
		fs::remove(strCardDataPath.toStdString());
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return;
	}
}

int GetCA(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo, const char* QMGY, const char* szAlgorithm, CAInfo& caInfo)
{
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfoIn = g_pDataCenter->GetSSCardInfo();
	int nResult = -1;
	char szStatus[1024] = { 0 };
	if (QFailed(nResult = getCA(Region.strCM_CA_Account.c_str(),
		Region.strCM_CA_Password.c_str(),
		Region.strCityCode.c_str(),
		pSSCardInfo->strCardID,
		pSSCardInfo->strCardNum,
		QMGY,
		pSSCardInfo->strName,
		szAlgorithm,
		caInfo,
		szStatus)))
	{
		FailureMessage("获取CA信息", pSSCardInfo, szStatus, strMessage);
		gInfo() << gQStr(strMessage);
		return nResult;
	}
	gInfo() << "getCA:" << szStatus;
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

int QueryCardProgress(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	int nResult = -1;
	char szStatus[1024] = { 0 };
	if (QFailed(nResult = queryCardProgress(*pSSCardInfo, szStatus)))
	{
		QString strStatus = QString::fromLocal8Bit(szStatus);
		if (strStatus == "身份证号不存在")
			return 0;
		FailureMessage("查询制卡进度", pSSCardInfo, szStatus, strMessage);

		//strMessage = "获取制卡进度信息失败!";
		//QString strInfo = QString("enableCard Failed:%1.").arg(nResult);
		gInfo() << gQStr(strMessage);
		return nResult;
	}
	gInfo() << "queryCardProgress:" << szStatus;
	char szDigit[16] = { 0 }, szText[1024] = { 0 };
	SplitString(szStatus, szDigit, szText);
	if (strlen(szText))
		strMessage = QString::fromLocal8Bit(szText);
	if (strlen(szDigit))
	{
		nStatus = strtolong(szDigit, 10);
		return 0;
	}
	else
		return -1;
}

int LoadTestData(string& strName, string& strCardID, string& strMobile)
{
	if (g_pDataCenter->bDebug)
	{
		QString strAppPath = QCoreApplication::applicationDirPath();
		strAppPath += "/Debug/PersonData.ini";
		QFileInfo fi(strAppPath);
		if (!fi.isFile())
			return -1;
		QSettings PersonSetting(strAppPath, QSettings::IniFormat);
		PersonSetting.beginGroup("Person");
		strName = UTF8_GBK(PersonSetting.value("Name").toString().toStdString().c_str());
		strCardID = UTF8_GBK(PersonSetting.value("CardID").toString().toStdString().c_str());
		strMobile = UTF8_GBK(PersonSetting.value("Mobile").toString().toStdString().c_str());
		PersonSetting.endGroup();

		gInfo() << "Name = " << strName << "\tCardID = " << strCardID << "\tMobile = " << strMobile;
		TraceMsgA("Name = %s\tCardID = %s\tMobile = %s.\n", strName.c_str(), strCardID.c_str(), strMobile.c_str());
	}

	return 0;
}

int LoadTestIDData(IDCardInfoPtr& pIDCard, SSCardInfoPtr& pSSCardInfo)
{
	if (g_pDataCenter->bDebug)
	{
		QString strAppPath = QCoreApplication::applicationDirPath();
		strAppPath += "/Debug/PersonData.ini";
		QFileInfo fi(strAppPath);
		if (!fi.isFile())
			return -1;
		if (!pIDCard)
			return -1;

		QSettings PersonSetting(strAppPath, QSettings::IniFormat);
		PersonSetting.beginGroup("Person");
		strcpy((char*)pIDCard->szName, UTF8_GBK(PersonSetting.value("Name").toString().toStdString().c_str()).c_str());
		strcpy((char*)pIDCard->szIdentity, UTF8_GBK(PersonSetting.value("CardID").toString().toStdString().c_str()).c_str());
		strcpy((char*)pIDCard->szNationalty, UTF8_GBK(PersonSetting.value("Nationalty").toString().toStdString().c_str()).c_str());
		strcpy((char*)pIDCard->szNationaltyCode, UTF8_GBK(PersonSetting.value("NationCode").toString().toStdString().c_str()).c_str());
		strcpy((char*)pIDCard->szGender, UTF8_GBK(PersonSetting.value("Gender").toString().toStdString().c_str()).c_str());
		strcpy((char*)pIDCard->szBirthday, UTF8_GBK(PersonSetting.value("Birthday").toString().toStdString().c_str()).c_str());
		strcpy((char*)pIDCard->szExpirationDate1, UTF8_GBK(PersonSetting.value("Issuedate").toString().toStdString().c_str()).c_str());
		strcpy((char*)pIDCard->szExpirationDate2, UTF8_GBK(PersonSetting.value("ExpireDate").toString().toStdString().c_str()).c_str());
		strcpy((char*)pIDCard->szAddress, UTF8_GBK(PersonSetting.value("Address").toString().toStdString().c_str()).c_str());
		strcpy((char*)pSSCardInfo->strName, (char*)pIDCard->szName);
		//strcpy((char*)pSSCardInfo->strIDCardIssuedDate, (char*)pIDCard->szExpirationDate1);
		
		strcpy((char*)pSSCardInfo->strCardID, (char*)pIDCard->szIdentity);
		strcpy((char*)pSSCardInfo->strSex, (char*)pIDCard->szGender);
		strcpy((char*)pSSCardInfo->strBirthday, (char*)pIDCard->szBirthday);
		strcpy((char*)pSSCardInfo->strAddress, (char*)pIDCard->szAddress);
		strcpy((char*)pSSCardInfo->strCardATR, PersonSetting.value("CardATR").toString().toStdString().c_str());
		strcpy((char*)pSSCardInfo->strIdentifyNum, PersonSetting.value("CardIdentityNum").toString().toStdString().c_str());
		strcpy((char*)pSSCardInfo->strBankNum, PersonSetting.value("BankNum").toString().toStdString().c_str());
		strcpy((char*)pSSCardInfo->strCardNum, PersonSetting.value("CardNum").toString().toStdString().c_str());
		strcpy((char*)pSSCardInfo->strMobile, PersonSetting.value("Mobile").toString().toStdString().c_str());
		strcpy((char*)pSSCardInfo->strBankCode, PersonSetting.value("BankCode").toString().toStdString().c_str());

		PersonSetting.endGroup();

	}
	return 0;
}
