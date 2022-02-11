#pragma once
#include <string>
using namespace std;

enum ServiceType
{
	Service_NewCard,
	Service_ReplaceCard,
	Service_RegisterLost
};

class SSCardService
{
protected:
	ServiceType nServiceType;
public:
	SSCardService() = delete;

	SSCardService(ServiceType nSvrType) :
		nServiceType(nSvrType)
	{
	}
	virtual int Initialize(string& szJson, string& szOut) = 0;

	virtual ~SSCardService()
	{
	}

	// 查询卡状态
	virtual int QueryCardStatus(string& strJsonIn, string& strJsonOut) = 0;

	// 提交制卡人信息
	virtual int CommitPersonInfo(string& strJsonIn, string& strJsonOut) = 0;


	// 预制卡
	virtual int PreMakeCard(string& strJsonIn, string& strJsonOut) = 0;

	// 读卡
	virtual int ReadCard(string& strJsonIn, string& strJsonOut) = 0;

	// 写卡
	virtual int WriteCard(string& strJsonIn, string& strJsonOut) = 0;

	// 打印
	virtual int PrintCard(string& strJsonIn, string& strJsonOut) = 0;

	// 数据回盘
	virtual int ReturnData(string& strJsonIn, string& strJsonOut) = 0;

	// 激活及后续收尾工作
	virtual int ActiveCard(string& strJsonIn, string& strJsonOut) = 0;

	// 临时挂失/解挂
	virtual int RegisterLostTemporary(string& strJsonIn, string& strJsonOut, int nOperation = 0) = 0;

	// 正式挂失/解挂
	virtual int RegisterLost(string& strJsonIn, string& strJsonOut, int nOperation = 0) = 0;


};
