#pragma once
#include <string>
using namespace std;

enum class ServiceType
{
	Service_NewCard,
	Service_ReplaceCard,
	Service_RegisterLost
};

enum class PayResult
{
	Pay_Unsupport = -1,		// 所有不需要支付的场景都返回-1
	WaitforPay = 0,
	WairforConfirm,
	PaySucceed,
	PayFailed,
	InvalidOrder,
	OrderCanceled
};

enum class CardStatus
{
	Card_Release = 0,			// 已经放号
	Card_Normal,				// 正常
	Card_RegisterLost,			// 挂失
	Card_RegisgerTempLost,		// 临时挂失
	Card_CanceledOnRegisterLost,// 挂失后注销
	Card_Canceled,				// 注销
	Card_DeActived,				// 未启用
	Card_Making,				// 制卡中	河南等地使用
	Card_Unknow = 255
};

class SSCardService
{
protected:
	ServiceType nServiceType;
public:
	SSCardService()
	{}

	//SSCardService(/*ServiceType nSvrType*/)/* :		nServiceType(nSvrType)*/
	//{
	//}

	virtual int SetSerivdeType(ServiceType nSvrType) = 0;

	virtual int Initialize(string& szJson, string& szOut) = 0;

	virtual ~SSCardService()
	{
	}

	// 查询卡状态
	virtual int QueryCardStatus(string& strJsonIn, string& strJsonOut) = 0;

	// 查询卡信息
	virtual int QueryCardInfo(string& strJsonIn, string& strJsonOut) = 0;

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

	virtual int LoadCardData(string& strINIFile) = 0;

	virtual int SaveCardData(string& strINIFile) = 0;

	virtual int QueryPayResult(string& strJsonIn, string& strJsonOut) = 0;

	virtual int SetExtraInterface(const string& strCommand, string& strInput, string& strOutput) = 0;

	virtual int GetCA(string& strJsonIn, string& strJsonou) = 0;
};
