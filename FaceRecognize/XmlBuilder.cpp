#include "XmlBuilder.h"
#include <time.h>
#include <sstream>
#include <iomanip>
#include "Markup.h"

std::string XmlBuilder::GetDateTime()
{
	time_t now = time(0);
	struct tm ltm;
	localtime_s(&ltm, &now);

	std::stringstream ss;
	ss << std::setfill('0')
		<< std::setw(4) << (ltm.tm_year + 1900)
		<< std::setw(2) << (ltm.tm_mon + 1)
		<< std::setw(2) << ltm.tm_mday
		<< std::setw(2) << ltm.tm_hour
		<< std::setw(2) << ltm.tm_min
		<< std::setw(2) << ltm.tm_sec;

	return ss.str();
}

std::string XmlBuilder::BuildRequestXml(
	const std::string& funCode,
	const std::string& user,
	const std::string& mEncrypted)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");

	// <request>
	xml.AddElem("request");
	xml.IntoElem();

	// <head>
	xml.AddElem("head");
	xml.IntoElem();

	std::string datetime = GetDateTime();

	xml.AddElem("fun", funCode);
	xml.AddElem("ver", "1.0");
	xml.AddElem("sender", "001");
	xml.AddElem("reciver", "002");
	xml.AddElem("datetime", datetime);
	xml.AddElem("operatorName", "");
	xml.AddElem("transId", "");

	xml.OutOfElem(); // </head>

	// <body>
	xml.AddElem("body");
	xml.IntoElem();

	xml.AddElem("USER", user);
	xml.AddElem("m", mEncrypted);

	xml.OutOfElem(); // </body>
	xml.OutOfElem(); // </request>

	return xml.GetDoc();
}

std::string XmlBuilder::BuildMContent(
	const std::string& id,
	const std::string& password,
	const std::string& additionalParams)
{
	CMarkup xml;

	xml.AddElem("ID", id);
	xml.AddElem("PASS", password);

	// 如果有额外参数，直接追加
	if (!additionalParams.empty())
	{
		xml.SetDoc(xml.GetDoc() + additionalParams);
	}

	return xml.GetDoc();
}

std::string XmlBuilder::BuildAuthQueryM(
	const std::string& password,
	const std::string& idCard)
{
	CMarkup xml;

	xml.AddElem("ID", "认证查询");
	xml.AddElem("PASS", password);
	xml.AddElem("AAC002", idCard);

	return CMarkup::AToUTF8(xml.GetDoc());
}

// 构建认证结果的 <m> 内容
std::string XmlBuilder::BuildAuthQueryResultsM(
	const std::string& password,
	const std::string& idCard)
{
	CMarkup xml;

	xml.AddElem("ID", "认证结果");
	xml.AddElem("PASS", password);
	xml.AddElem("AAC002", idCard);
	return CMarkup::AToUTF8(xml.GetDoc());
}

std::string XmlBuilder::BuildAuthenticationM(
	const std::string& password,
	const std::string& idCard,
	const std::string& typeId,
	const std::string& devID,
	const std::string& areaID,
	const std::string& sign,
	const std::string& photoBase64,
	const std::string& photoType)
{
	CMarkup xml;
	/*
	<m>
		<ID>认证</ID>
		<PASS>41990001A</PASS>
		<AAC002>111111198101011110</AAC002>
		<TYPEID>A00001</TYPEID>
		<DEVICEID>A0000100001</DEVICEID>
		<FEATURES1></FEATURES1>
		<REMARK1></REMARK1>
		<AUTHEN>authen</AUTHEN>
		< CBDID > </ CBDID >
		< RZYW > </ RZYW >
		< XZRZ> </XZRZ>
		<SIGN>sign</SIGN>
	</m>
	*/
	xml.AddElem("ID", "认证");
	xml.AddElem("PASS", password);
	xml.AddElem("AAC002", idCard);
	xml.AddElem("TYPEID", typeId);
	xml.AddElem("DEVICEID", devID);
	xml.AddElem("FEATURES1", "");
	xml.AddElem("REMARK1", "");
	xml.AddElem("AUTHEN", photoBase64);
	xml.AddElem("CBDID", areaID);
	xml.AddElem("RZYW", "09");
	xml.AddElem("XZRZ", "");
	xml.AddElem("SIGN", sign);
	xml.AddElem("PHOTOTYPE", photoType);

	return CMarkup::AToUTF8(xml.GetDoc());
}

std::string XmlBuilder::BuildBusinessNameM(const std::string& password)
{
	CMarkup xml;
	
	xml.AddElem("ID", "业务名称");
	xml.AddElem("PASS", password);

	return CMarkup::AToUTF8(xml.GetDoc());
}