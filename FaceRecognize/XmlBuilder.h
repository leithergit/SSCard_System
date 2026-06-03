#pragma once
#include "Markup.h"
#include <string>

class XmlBuilder
{
public:
	// 构建请求 XML
	static std::string BuildRequestXml(
		const std::string& funCode,
		const std::string& user,
		const std::string& mEncrypted);

	// 构建 <m> 部分的明文（用于加密）
	static std::string BuildMContent(
		const std::string& id,
		const std::string& password,
		const std::string& additionalParams = "");

	// 构建认证查询的 <m> 内容
	static std::string BuildAuthQueryM(
		const std::string& password,
		const std::string& idCard);

	// 构建认证结果的 <m> 内容
	static std::string BuildAuthQueryResultsM(
		const std::string& password,
		const std::string& idCard);

	static std::string BuildAuthenticationM(
		const std::string& password,
		const std::string& idCard,
		const std::string& typeId,
		const std::string& devID,
		const std::string& areaID,
		const std::string& sign,
		const std::string& photoBase64,
		const std::string& photoType);

	// 构建业务名称查询的 <m> 内容
	static std::string BuildBusinessNameM(const std::string& password);

private:
	static std::string GetDateTime();
};