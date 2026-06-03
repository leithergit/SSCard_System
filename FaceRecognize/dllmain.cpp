#pragma execution_character_set("utf-8")

#define GLOG_NO_ABBREVIATED_SEVERITIES
#define GOOGLE_STRIP_LOG 1

#include "pch.h"
#include "glog/logging.h"
#include "soapScrzServiceSoapBindingProxy.h"
#include "ScrzServiceSoapBinding.nsmap"
#include "soapH.h"
#include "AESCrypto.h"
#include "XmlBuilder.h"
#include "XmlParser.h"
#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include "FaceRecognize.h"

#ifdef _DEBUG
#pragma comment(lib, "./glog/glogd.lib")
#else
#pragma comment(lib, "./glog/glog.lib")
#endif

using namespace std;
string g_strServerUrl = "http://10.120.84.4:7001/hnScrzService/services/ScrzService?wsdl";
string g_strUser = "";
string g_strPass = "";
string g_strAesKey = "";

// 将字节数组转换为16进制字符串
std::string bytesToHexString(const unsigned char* bytes, int length) {
	std::stringstream ss;
	ss << std::hex << std::uppercase;
	for (int i = 0; i < length; i++) {
		ss.width(2);
		ss.fill('0');
		ss << static_cast<int>(bytes[i]);
	}
	return ss.str();
}

// 将16进制字符串转换为字节数组
std::vector<unsigned char> hexStringToBytes(const std::string& hexStr) {
	std::vector<unsigned char> bytes;
	for (size_t i = 0; i < hexStr.length(); i += 2) {
		std::string byteString = hexStr.substr(i, 2);
		unsigned char byte = static_cast<unsigned char>(strtol(byteString.c_str(), NULL, 16));
		bytes.push_back(byte);
	}
	return bytes;
}

std::string extractXmlTag(const std::string& xml, const std::string& tagName) {
	std::string startTag = "<" + tagName + ">";
	std::string endTag = "</" + tagName + ">";

	size_t startPos = xml.find(startTag);
	size_t endPos = xml.find(endTag);

	if (startPos != std::string::npos && endPos != std::string::npos) {
		startPos += startTag.length();
		return xml.substr(startPos, endPos - startPos);
	}

	return "";
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}




// 去除字符串首尾空白字符
std::string Trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		return "";
	}
	size_t last = str.find_last_not_of(" \t\r\n");
	return str.substr(first, (last - first + 1));
}


std::vector<BusinessItem> ParseBusinessNamesFromString(const std::string& data) {
	std::vector<BusinessItem> businessList;
	std::istringstream iss(data);
	std::string line;

	// 跳过标题行
	if (std::getline(iss, line)) {
		// 标题行：序号,业务名称
	}

	// 逐行解析
	while (std::getline(iss, line)) {
		line = Trim(line);
		if (line.empty()) {
			continue;
		}

		// 查找逗号分隔符
		size_t commaPos = line.find(',');
		if (commaPos != std::string::npos) {
			std::string idStr = Trim(line.substr(0, commaPos));
			std::string name = Trim(line.substr(commaPos + 1));

			if (!idStr.empty() && !name.empty()) {
				try {
					int id = std::stoi(idStr);
					businessList.push_back(BusinessItem(id, name));
				}
				catch (const std::exception& e) {
					LOG(ERROR) << "解析行失败: " << line << ", 错误: " << e.what() << std::endl;
				}
			}
		}
	}

	return businessList;
}

std::vector<AuthResult> ParseAuthResults(const std::string& csvData) {
	std::vector<AuthResult> results;
	std::istringstream iss(csvData);
	std::string line;
	bool firstLine = true;

	while (std::getline(iss, line)) {
		if (firstLine) {
			firstLine = false;
			continue;  // 跳过标题行
		}

		if (Trim(line).empty()) continue;

		// 按逗号分割
		std::vector<std::string> fields;
		std::stringstream ss(line);
		std::string field;

		while (std::getline(ss, field, ',')) {
			fields.push_back(Trim(field));
		}

		if (fields.size() >= 7) {
			results.push_back(AuthResult(
				std::stoi(fields[0]),  // id
				fields[1],              // idCard
				fields[2],              // name
				fields[3],              // authTime
				fields[4],              // authMethod
				fields[5],              // authArea
				fields[6]               // authBusiness
			));
		}
	}

	return results;
}

bool IsDigit(char c) {
	return c >= '0' && c <= '9';
}

void Initialize(
	string& strServerUrl,
	string& strUser,
	string& strPass,
	string& strAesKey)
{
	g_strServerUrl = strServerUrl;
	g_strUser = strUser;
	g_strPass = strPass;
	g_strAesKey = strAesKey;
}

int FaceAuthenticate(
	string& strFaceData,
	string& strCardID,
	string& strTypeID,
	string& strDevID,
	string& strCBDID,
	string& strSign,
	string strFaceDataType)
{
	ScrzServiceSoapBindingProxy proxy;
	proxy.soap_endpoint = g_strServerUrl.c_str();
	try
	{
		AESCrypto Aes;
		string mPlain = XmlBuilder::BuildAuthenticationM(
			g_strPass,
			strCardID,
			strTypeID,
			strDevID,
			strCBDID,
			strSign,
			strFaceData,
			strFaceDataType);
		cout << mPlain << endl;
		string mEncryptedHex = Aes.Encrypt(g_strAesKey, mPlain);
		string requestXml = XmlBuilder::BuildRequestXml(
			"FaceAuthenticate",
			g_strUser,
			mEncryptedHex  // 使用Hex编码的密文
		);
		_ns1__allJK request;
		_ns1__allJKResponse response;
		request.xml = requestXml;
		int ret = proxy.allJK(&request, response);
		if (ret == SOAP_OK && response.allJKReturn.size())
		{
			try
			{
				string result = response.allJKReturn;
				LOG(INFO) << result;
				string strError = extractXmlTag(result, "ERROR");
				LOG(INFO) << "Error = " << strError;
				if (strError.size())
				{
					bool bDigit = true;
					for (auto var : strError)
						if (!IsDigit(var))
						{
							bDigit = false;
							break;
						}
					if (!bDigit)
						return -1;
					
					int nError = atoi(strError.c_str());
					return nError;
				}
				else
					return -1;
			}
			catch (const exception& e)
			{
				LOG(INFO) << "Exception:" << e.what();
				return -1;
			}
		}
		else
		{
			proxy.soap_stream_fault(cerr);
			return -1;
		}
	}
	catch (const exception& e)
	{
		LOG(INFO) << "Exception:" << e.what();
		return -1;
	}
}

int QueryAuthenticateStatus(
	string& szCardID,
	AuthType& nAuthType,
	string& strAuthTime)
{
	ScrzServiceSoapBindingProxy proxy;
	proxy.soap_endpoint = g_strServerUrl.c_str();

	try
	{
		AESCrypto Aes;
		string mPlain = XmlBuilder::BuildAuthQueryM(g_strPass, szCardID);

		string mEncryptedHex = Aes.Encrypt(g_strAesKey, mPlain);

		string requestXml = XmlBuilder::BuildRequestXml(
			"QueryAuthenticateStatus",
			g_strUser,
			mEncryptedHex  // 使用Hex编码的密文
		);

		_ns1__allJK request;
		_ns1__allJKResponse response;

		request.xml = requestXml;

		int ret = proxy.allJK(&request, response);

		if (ret == SOAP_OK && response.allJKReturn.size())
		{
			try
			{
				/*
					<ERROR>00/失败原因</ERROR>
					<r>
						<当前时间>20180527095926</当前时间>
						<社会保障号码>111111198101011110</社会保障号码>
						<姓名>姓名1</姓名>
						<认证时间>20180527095705</认证时间>
						<认证方式>01_指纹认证</认证方式>
					</r>
				*/
				string result = response.allJKReturn;
				LOG(INFO) << "Auth Query Response: " << result;
				string strError = extractXmlTag(result, "ERROR");
				cout << "Error = " << strError << endl;
				if (strError.size())
				{
					bool bDigit = true;
					for (auto var : strError)
						if (!IsDigit(var))
						{
							bDigit = false;
							break;
						}
					if (!bDigit)
						return -1;

					int nError = atoi(strError.c_str());
					if (!nError)
					{
						string strEncrypt = extractXmlTag(result, "r");
						if (strEncrypt.empty())
						{
							return -1;
						}
						string strDecrypted = Aes.Decrypt(g_strAesKey, strEncrypt);
						LOG(INFO) << "Auth Query Decrypted: " << strDecrypted;
						strAuthTime = extractXmlTag(strDecrypted, "认证时间");
						string strAuthMethod = extractXmlTag(strDecrypted, "认证方式");
						if (strAuthMethod.empty())
						{
							nAuthType = AUTH_TYPE_NONE;
							return 0;
						}

						auto ExtractNum = [](const std::string& str)->string
							{
								std::string result;
								for (char c : str) {
									if (std::isdigit(static_cast<unsigned char>(c))) {
										result += c;
									}
									else if (!result.empty()) {
										break;  // 找到第一组数字后停止
									}
								}
								return result;
							};
						string strAuthType = ExtractNum(strAuthMethod);

						if (strAuthType == "01")
						{
							nAuthType = AUTH_TYPE_FINGERPRINT; // 指纹认证成功
						}
						else if (strAuthType == "02")
						{
							nAuthType = AUTH_TYPE_VEIN;			// 静脉脉认证
						}
						else if (strAuthType == "03")
						{
							nAuthType = AUTH_TYPE_FACE;			// 人脸认证
						}
						else
						{
							nAuthType = AUTH_TYPE_NONE; // 未知认证方式
						}
					}
					return nError;
				}
				else
					return -1;
			}
			catch (const exception& e)
			{
				LOG(INFO) << "Exception:" << e.what();
				return -1;
			}
		}
		else
		{
			proxy.soap_stream_fault(cerr);
			return -1;
		}
	}
	catch (const exception& e)
	{
		LOG(INFO) << "Exception:" << e.what();
		return -1;
	}

	return -1;
}


int QueryAuthenticateResults(
	string& szCardID,
	vector<AuthResult> &vecResults)
{
	ScrzServiceSoapBindingProxy proxy;
	proxy.soap_endpoint = g_strServerUrl.c_str();

	try
	{
		AESCrypto Aes;
		string mPlain = XmlBuilder::BuildAuthQueryResultsM(g_strPass, szCardID);

		string mEncryptedHex = Aes.Encrypt(g_strAesKey, mPlain);

		string requestXml = XmlBuilder::BuildRequestXml(
			"QueryAuthenticateResults",
			g_strUser,
			mEncryptedHex  // 使用Hex编码的密文
		);

		_ns1__allJK request;
		_ns1__allJKResponse response;

		request.xml = requestXml;

		int ret = proxy.allJK(&request, response);

		if (ret == SOAP_OK && response.allJKReturn.size())
		{
			try
			{
				/*
					<ERROR>00/失败原因</ERROR>
					<r>
						<当前时间>20180527095926</当前时间>
						<社会保障号码>111111198101011110</社会保障号码>
						<姓名>姓名1</姓名>
						<认证时间>20180527095705</认证时间>
						<认证方式>01_指纹认证</认证方式>
					</r>
				*/
				string result = response.allJKReturn;
				LOG(INFO) << "Auth Query Response: " << result;
				string strError = extractXmlTag(result, "ERROR");

				if (strError.size())
				{
					bool bDigit = true;
					for (auto var : strError)
						if (!IsDigit(var))
						{
							bDigit = false;
							break;
						}
					if (!bDigit)
						return -1;

					int nError = atoi(strError.c_str());
					if (!nError)
					{
						string strEncrypt = extractXmlTag(result, "r");
						if (strEncrypt.empty())
						{
							return -1;
						}
						string strDecrypted = Aes.Decrypt(g_strAesKey, strEncrypt);
						LOG(INFO) << strDecrypted;
						vecResults = ParseAuthResults(strDecrypted);
					}
					return nError;
				}
				else
					return -1;
			}
			catch (const exception& e)
			{
				LOG(INFO) << "Exception:" << e.what();
				return -1;
			}
		}
		else
		{
			proxy.soap_stream_fault(cerr);
			return -1;
		}
	}
	catch (const exception& e)
	{
		LOG(INFO) << "Exception:" << e.what();
		return -1;
	}

	return -1;
}

int QueryBusinessName(vector<BusinessItem>& vecBussiness)
{
	try
	{
		ScrzServiceSoapBindingProxy proxy;
		proxy.soap_endpoint = g_strServerUrl.c_str();
		AESCrypto Aes;
		string mPlain = XmlBuilder::BuildBusinessNameM(g_strPass);

		string mEncryptedHex = Aes.Encrypt(g_strAesKey, mPlain);

		string requestXml = XmlBuilder::BuildRequestXml(
			"GET_BUSINESS_NAME",
			g_strUser,
			mEncryptedHex  // 使用Hex编码的密文
		);

		// 步骤4: 调用服务
		_ns1__allJK request;
		_ns1__allJKResponse response;

		request.xml = requestXml;

		int ret = proxy.allJK(&request, response);

		if (ret == SOAP_OK && response.allJKReturn.size())
		{
			try
			{
				string result = response.allJKReturn;
				LOG(INFO) << "Auth Query Response: " << result;

				string strError = extractXmlTag(result, "ERROR");
				string strEncrypt = extractXmlTag(result, "r");
				string strDecrypted = Aes.Decrypt(g_strAesKey, strEncrypt);
				vecBussiness = ParseBusinessNamesFromString(strDecrypted);

			}
			catch (const exception& e)
			{
				LOG(INFO) << "Exception:" << e.what();
				return false;
			}
		}
		else
		{
			proxy.soap_stream_fault(cerr);
			return false;
		}
	}
	catch (const exception& e)
	{
		LOG(INFO) << "Exception:" << e.what();
		return false;
	}

	return false;
}