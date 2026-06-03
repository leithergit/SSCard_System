#pragma once
#include "Markup.h"
#include <string>
#include <map>

class XmlParser
{
public:
	// 解析响应 XML
	struct ResponseData
	{
		std::string errorCode;
		std::string errorMsg;
		std::string rEncrypted;  // 加密的 <r> 内容
		std::string rDecrypted;  // 解密的 <r> 内容
		std::map<std::string, std::string> fields; // 其他字段
		bool success;

		ResponseData() : success(false) {}
	};

	// 解析响应 XML
	static ResponseData ParseResponse(const std::string& xmlResponse);

	// 解析解密后的 <r> 内容
	static std::map<std::string, std::string> ParseRContent(const std::string& rContent);

	// 获取指定标签的值
	static std::string GetTagValue(const std::string& xml, const std::string& tagName);

	// 格式化 XML（美化输出）
	static std::string FormatXml(const std::string& xml);
};