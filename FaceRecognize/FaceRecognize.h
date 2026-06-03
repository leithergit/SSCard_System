#pragma once
#include <vector>
#include <string>
#include <memory>
using namespace std;

// 业务名称结构体
struct BusinessItem {
	int id;                    // 序号
	std::string name;          // 业务名称
	BusinessItem(int businessId, const std::string& businessName)
		: id(businessId), name(businessName) {
	}
};

struct AuthResult {
	// 序号,社会保障号码,姓名,认证时间,认证方式,认证地区,认证业务
	int id;
	std::string idCard;
	std::string name;
	std::string authTime;
	std::string authMethod;
	std::string authArea;
	std::string authBusiness;
	AuthResult(int resId,
		const std::string& resIdCard,
		const std::string& resName,
		const std::string& resAuthTime,
		const std::string& resAuthMethod,
		const std::string& resAuthArea,
		const std::string& resAuthBusiness)
		: id(resId),
		idCard(resIdCard),
		name(resName),
		authTime(resAuthTime),
		authMethod(resAuthMethod),
		authArea(resAuthArea),
		authBusiness(resAuthBusiness) {
	}
};

enum AuthType
{
	AUTH_TYPE_NONE = -1,
	AUTH_TYPE_FINGERPRINT = 1,
	AUTH_TYPE_VEIN = 2,
	AUTH_TYPE_FACE = 3
};
using BusinessItemPtr = shared_ptr<BusinessItem>;

void Initialize(
	string& strServerUrl,
	string& strUser,
	string& strPass,
	string& strAesKey);

int FaceAuthenticate(
	string& strFaceData,
	string& strCardID,
	string& strTypeID,
	string& strDevID,
	string& strCBDID,
	string& strSign,
	string strFaceDataType = "jpg");

int QueryAuthenticateStatus(
	string& szCardID, 
	AuthType &nAuthType, 
	string& strAuthTime);

int QueryAuthenticateResults(
	string& szCardID,
	vector<AuthResult> &vecResults);

int QueryBusinessName(vector<BusinessItem> &vecBussinessInfo);