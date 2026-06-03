#include "XmlParser.h"
#include <sstream>

XmlParser::ResponseData XmlParser::ParseResponse(const std::string& xmlResponse)
{
	ResponseData data;
	CMarkup xml;

	if (!xml.SetDoc(xmlResponse))
	{
		data.success = false;
		data.errorMsg = "XML 解析失败";
		return data;
	}

	// 查找 <response> 或 <result> 根元素
	if (xml.FindElem("response") || xml.FindElem("result"))
	{
		xml.IntoElem();

		// 解析所有子元素
		while (xml.FindElem())
		{
			std::string tagName = xml.GetTagName();
			std::string tagValue = xml.GetData();

			if (tagName == "ERROR")
			{
				data.errorCode = tagValue;
			}
			else if (tagName == "ERRORMSG")
			{
				data.errorMsg = tagValue;
			}
			else if (tagName == "r")
			{
				data.rEncrypted = tagValue;
			}
			else
			{
				data.fields[tagName] = tagValue;
			}
		}

		data.success = (data.errorCode == "0" || data.errorCode.empty());
	}

	return data;
}

std::map<std::string, std::string> XmlParser::ParseRContent(const std::string& rContent)
{
	std::map<std::string, std::string> fields;
	CMarkup xml;

	if (!xml.SetDoc(rContent))
	{
		return fields;
	}

	// 可能有根元素，也可能没有
	if (xml.FindElem())
	{
		xml.IntoElem();
	}

	// 解析所有元素
	while (xml.FindElem())
	{
		std::string tagName = xml.GetTagName();
		std::string tagValue = xml.GetData();
		fields[tagName] = tagValue;
	}

	return fields;
}

std::string XmlParser::GetTagValue(const std::string& xml, const std::string& tagName)
{
	CMarkup xmlDoc;

	if (!xmlDoc.SetDoc(xml))
	{
		return "";
	}

	if (xmlDoc.FindElem(tagName.c_str()))
	{
		return xmlDoc.GetData();
	}

	return "";
}

std::string XmlParser::FormatXml(const std::string& xml)
{
	CMarkup xmlDoc;

	if (!xmlDoc.SetDoc(xml))
	{
		return xml;
	}

	// CMarkup 自动格式化
	return xmlDoc.GetDoc();
}