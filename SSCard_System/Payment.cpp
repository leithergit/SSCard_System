#pragma execution_character_set("utf-8")
#include "Payment.h"

#include <atlenc.h>

#define PhotoBufferSize		256*1024
char* g_szPhotoBuffer = new char[PhotoBufferSize];

#include "./SDK/QREncode/qrencode.h"
#include "Gloabal.h"

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
	strUrl += PayOption.strPayUrl;				//strUrl += "?";
	strUrl += PayOption.strFieldName;			strUrl += "=";
	string strNameUrl;
	GBKStringtoUrlString((const char*)pIDCard->szName, strNameUrl);
	strUrl += strNameUrl;						strUrl += "&";
	strUrl += PayOption.strFieldMobile;			strUrl += "=";
	strUrl += g_pDataCenter->strMobilePhone;	strUrl += "&";
	strUrl += PayOption.strFieldCardID;			strUrl += "=";
	strUrl += (char*)pIDCard->szIdentify;		strUrl += "&";
	strUrl += PayOption.strFiledamount;			strUrl += "=";
	strUrl += PayOption.strAmount;
	gInfo() << "Payment request url = " << strUrl.c_str();

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
		gInfo() << gQStr(strPaymentUrl);
		return 0;
	}
	else
	{
		strMessage = strMessage1.c_str();
		gError() << gQStr(strMessage);
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
		strMessage = QString::fromLocal8Bit(szStatus);
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
	//SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
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
		strMessage = QString::fromLocal8Bit(szStatus);
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
	//IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	//SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szStatus[1024] = { 0 };
	QString strInfo = QString("Try to markCard(%1,%2,%3,%4,%5)").arg(Region.strCMAccount.c_str()).arg(Region.strCMPassword.c_str()).arg(Region.strArea.c_str()).arg((const char*)pIDCard->szIdentify).arg((char*)pIDCard->szName);
	gInfo() << gQStr(strInfo);
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
		strMessage = QString::fromLocal8Bit(szStatus);
		return -1;
	}
}

int     CancelMarkCard(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	//SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
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
		strMessage = QString::fromLocal8Bit(szStatus);
		return -1;
	}
}

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
	AddCardFiled(CardIni, strAdress);
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
int LoadCardData(SSCardInfoPtr& pSSCardInfoOut, QString strINIFile)
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
	GetCardField(CardIni, pSSCardInfoOut, strAdress);
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
	QByteArray ba = CardIni.value("strPhoto").toString().toLatin1();
	qDebug() << "Load photo,size = " << ba.size();
	pSSCardInfoOut->strPhoto = new char[ba.size() + 1];
	strcpy_s(pSSCardInfoOut->strPhoto, ba.size() + 1, ba.data());
	GetCardField(CardIni, pSSCardInfoOut, strIdentifyNum);
	GetCardField(CardIni, pSSCardInfoOut, strCardATR);
	GetCardField(CardIni, pSSCardInfoOut, strBankNum);
	GetCardField(CardIni, pSSCardInfoOut, strPCH);
	CardIni.endGroup();
	return 0;
}

int  GetIDImageStorePath(string& strFilePath)
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
	QString strTempPath = strStorePath + QString("ID_%1.jpg").arg((const char*)g_pDataCenter->GetIDCardInfo()->szIdentify);
	strFilePath = strTempPath.toStdString();
	return 0;
}

int SaveSSCardPhoto(QString strMessage, const char* szPhotoBase64)
{
	if (!szPhotoBase64)
	{
		return -1;
	}

	ZeroMemory(g_szPhotoBuffer, PhotoBufferSize);
	int nPhotoSize = PhotoBufferSize;
	Base64Decode(szPhotoBase64, strlen(szPhotoBase64), (BYTE*)g_szPhotoBuffer, &nPhotoSize);
	QImage photo = QImage::fromData((const uchar*)g_szPhotoBuffer, nPhotoSize);
	string strPhotoPath;
	GetIDImageStorePath(strPhotoPath);
	photo.save(strPhotoPath.c_str(), "JPG", 90);
	g_pDataCenter->strSSCardPhotoFile = strPhotoPath.c_str();
	return 0;
}

// nStatus 返回0为成功，其它都为失败
int     GetCardData(QString& strMessage, int& nStatus)
{
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfoIn = g_pDataCenter->GetSSCardInfo();
	SSCardInfoPtr pSSCardInfoOut = make_shared<SSCardInfo>();
	strcpy(pSSCardInfoIn->strName, (char*)pIDCard->szName);
	strcpy(pSSCardInfoIn->strCardID, (char*)pIDCard->szIdentify);
	strcpy(pSSCardInfoIn->strTransType, "5");
	strcpy(pSSCardInfoIn->strCity, Region.strArea.c_str());
	string strCountry = Region.strArea + Region.strCountry;
	strcpy(pSSCardInfoIn->strSSQX, strCountry.c_str());
	strcpy(pSSCardInfoIn->strCard, Region.strCardVendor.c_str());
	strcpy(pSSCardInfoIn->strBankCode, Region.strBankCode.c_str());

	QString strAppPath = QCoreApplication::applicationDirPath();
	strAppPath += "/Debug";
	QFileInfo fdir(strAppPath);
	if (fdir.exists())
	{
		if (!fdir.isDir())
		{
			QFile f(strAppPath);
			f.remove();
		}
	}
	else
	{
		QDir dir;
		dir.mkdir(strAppPath);
	}

	strAppPath += QString("/Carddata_%1.ini").arg(pSSCardInfoIn->strCardID);
	QFileInfo ffile(strAppPath);
	//#ifdef _DEBUG
	//	if (ffile.isFile())
	//	{
	//#pragma Warning("使用预存制卡数据")
	//		LoadCardData(pSSCardInfoOut, strAppPath);
	//		strcpy(pSSCardInfoIn->strPCH, pSSCardInfoOut->strPCH);
	//		strcpy(pSSCardInfoIn->strCardNum, pSSCardInfoOut->strCardNum);		// 新的卡号
	//		strcpy(pSSCardInfoIn->strNation, pSSCardInfoOut->strNation);
	//		strcpy(pSSCardInfoIn->strSex, pSSCardInfoOut->strSex);
	//		strcpy(pSSCardInfoIn->strBirthday, pSSCardInfoOut->strBirthday);
	//		strcpy(pSSCardInfoIn->strReleaseDate, pSSCardInfoOut->strReleaseDate);
	//		strcpy(pSSCardInfoIn->strValidDate, pSSCardInfoOut->strValidDate);
	//		pSSCardInfoIn->strPhoto = pSSCardInfoOut->strPhoto;
	//		SaveSSCardPhoto(strMessage, pSSCardInfoIn->strPhoto);
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
	//				strcpy(pSSCardInfoIn->strPCH, pSSCardInfoOut->strPCH);
	//				strcpy(pSSCardInfoIn->strCardNum, pSSCardInfoOut->strCardNum);		// 新的卡号
	//				strcpy(pSSCardInfoIn->strNation, pSSCardInfoOut->strNation);
	//				strcpy(pSSCardInfoIn->strSex, pSSCardInfoOut->strSex);
	//				strcpy(pSSCardInfoIn->strBirthday, pSSCardInfoOut->strBirthday);
	//				strcpy(pSSCardInfoIn->strReleaseDate, pSSCardInfoOut->strReleaseDate);
	//				strcpy(pSSCardInfoIn->strValidDate, pSSCardInfoOut->strValidDate);
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
	char szStatus[1014] = { 0 };
	int nResult = getCardData(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		*pSSCardInfoIn,
		*pSSCardInfoOut,
		szStatus);
	if (QFailed(nResult))
	{
		strMessage = "获取三代社保卡数据失败!";
		QString strInfo = QString("getCardData Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return -1;
	}

	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		if (nStatus == 0)
		{
			strcpy(pSSCardInfoIn->strPCH, pSSCardInfoOut->strPCH);
			strcpy(pSSCardInfoIn->strCardNum, pSSCardInfoOut->strCardNum);		// 新的卡号
			strcpy(pSSCardInfoIn->strNation, pSSCardInfoOut->strNation);
			strcpy(pSSCardInfoIn->strSex, pSSCardInfoOut->strSex);
			strcpy(pSSCardInfoIn->strBirthday, pSSCardInfoOut->strBirthday);
			strcpy(pSSCardInfoIn->strReleaseDate, pSSCardInfoOut->strReleaseDate);
			strcpy(pSSCardInfoIn->strValidDate, pSSCardInfoOut->strValidDate);
			pSSCardInfoIn->strPhoto = pSSCardInfoOut->strPhoto;
			SaveCardData(pSSCardInfoIn, strAppPath);
			SaveSSCardPhoto(strMessage, pSSCardInfoIn->strPhoto);
		}
		return 0;
	}
	else
	{
		strMessage = QString::fromLocal8Bit(szStatus);
		return -1;
	}
	//#endif

	return 0;
}

int     ReturnCardData(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
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
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		return 0;
	}
	else
	{
		strMessage = QString::fromLocal8Bit(szStatus);
		return -1;
	}
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
		strMessage = QString::fromLocal8Bit(szStatus);
		return -1;
	}
	return 0;
}


int GetCA(QString strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo, const char* QMGY, const char* szAlgorithm, CAInfo& caInfo)
{
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	SSCardInfoPtr pSSCardInfoIn = g_pDataCenter->GetSSCardInfo();
	int nResult = -1;
	char szStatus[1024] = { 0 };
	if (QFailed(nResult = getCA(Region.strCM_CA_Account.c_str(),
		Region.strCM_CA_Password.c_str(),
		Region.strArea.c_str(),
		pSSCardInfo->strCardID,
		pSSCardInfo->strCardNum,
		QMGY,
		pSSCardInfo->strName,
		szAlgorithm,
		caInfo,
		szStatus)))
	{
		strMessage = "获取CA信息失败";
		QString strInfo = QString("enableCard Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return nResult;
	}
	gInfo() << "getCA:" << szStatus;
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		return 0;
	}
	else
	{
		strMessage = QString::fromLocal8Bit(szStatus);
		return -1;
	}
}


int QueryCardProgress(QString strMessage, int& nStatus, IDCardInfoPtr& pIDCard, SSCardInfoPtr& pSSCardInfo)
{
	RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	int nResult = -1;
	char szStatus[1024] = { 0 };
	if (QFailed(nResult = queryCardProgress(Region.strCMAccount.c_str(),
		Region.strCMPassword.c_str(),
		(const char*)pIDCard->szIdentify,
		(const char*)pIDCard->szName, Region.strArea.c_str(), *pSSCardInfo, szStatus)))
	{
		strMessage = "获取制卡进度信息失败!";
		QString strInfo = QString("enableCard Failed:%1.").arg(nResult);
		gInfo() << gQStr(strInfo);
		return nResult;
	}
	gInfo() << "queryCardProgress:" << szStatus;
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
		return 0;
	}
	else
	{
		strMessage = QString::fromLocal8Bit(szStatus);
		return -1;
	}
}

int LoadTestData(QString& strName, QString& strCardID, QString& strMobile)
{
	if (g_pDataCenter->bDebug)
	{
		QString strAppPath = QCoreApplication::applicationDirPath();
		strAppPath += "/Debug/CardData.ini";
		QFileInfo fi(strAppPath);
		if (!fi.isFile())
			return -1;

		strName = "韩晓丽";
		strCardID = "412726198206150140";
		strMobile = "13673888580";
	}

	/*QSettings CardTest(strAppPath);
	QStringList strFieldList = CardTest.allKeys();
	for (auto var : strFieldList)
	{
		qDebug() << CardTest.value(var).toString();;
	}
	CardTest.beginGroup("CardData");
	strName = CardTest.value("Name").toString();

	strCardID = CardTest.value("CardID").toString();
	strMobile = CardTest.value("Mobile").toString();
	qDebug() << "Name = " << strName << "CardID = " << strCardID << "Mobile = " << strMobile;
	CardTest.endGroup();*/
	return 0;
}
