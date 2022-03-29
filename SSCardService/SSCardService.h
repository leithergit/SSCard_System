#pragma once
#include <string>
using namespace std;

enum class ServiceType
{
	Service_Unknown = -1,
	Service_NewCard = 0,
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

struct SSCardBaseInfo
{
	// 持卡人身份信息
	std::string strName;			// 姓名
	std::string strIdentity;		// 身份证号
	std::string strBirthday;		// 出生日期	
	std::string strGender;			// 性别
	std::string strGenderCode; 		// 性别代码
	std::string strNation;			// 民族
	std::string strNationCode;		// 民族代码
	std::string strAddress;			// 通讯地址

	// 社保卡关键信息
	std::string strCardNum;			// 社保卡号
	std::string strPhoto;			// 相片
	std::string strMobile;			// 手机号
	std::string strOccupType;		// 职业类别

	// 社保业务相关					  
	std::string strTransType;		// 申请类型 新制卡，补换卡
	std::string strCardStatus;		// 制卡中(或制卡完成(待分发)待领卡或卡已领取)
	// 山东特有						    
	std::string strPersonType;		// 人员类别
	std::string strCardType;		// 证件类型
	// 河南特有						    
	std::string strPCH;				// 批次号
	std::string strReleaseDate;		// 发卡日期
	std::string strValidDate;		// 有效日期	

	// 银行相关
	std::string strBankCode;		// 银行代码	
	std::string strBankNum;			// 银行卡号
	std::string strOperator;		// 经办人
	std::string strOrganID;			// 经办机构

	// 地市相关						   
	std::string strCity;			// 所属城市
	std::string strSSQX;			// 所属区县

	// 卡信息相关					   
	std::string strCardIdentity;	// 卡识别码
	std::string strCardATR;			// 卡复位信息
	std::string strCardVender;		// 卡商
	std::string strChipNum;			// 卡芯片账号
	std::string strMagNum;			// 卡磁条账号
	std::string strCardVersion;		// 卡规范版本
	std::string strChipType;		// 芯片类型

	// 其它非关键信息
	std::string strCityAccTime;		// 市接收时间
	std::string strCityInfo;		// 领卡地信息
	std::string strFailType;		// 失败类型(银行,制卡)
	std::string strFailReason;		// 失败原因
	std::string strCommunity;		// 所在社区
	std::string strCompanyName;		// 单位名称
	std::string strGuardianName;	// 监护人姓名
	std::string strPostalCode;		// 邮政编码
	std::string strEmail;			// 电子邮箱
	std::string strLocalNum;		// 所在社区或单位,编号
	std::string strClassName;		// 班级名称
	std::string strUniversity;		// 是否大学
	std::string strDepartmentName;	// 所在院系名称
	std::string strReserver[16];	// 保留字段
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

	virtual int SetServiceType(ServiceType nSvrType, string& strIdentity) = 0;

	virtual int Initialize(string& szJson, string& szOut) = 0;

	virtual ~SSCardService()
	{
	}

	virtual bool UpdateStage(string& strKey, int nResult, string strDatagram, bool bFinised = false) = 0;

	virtual int GetStageStatus(string strKey, int& nResult, string& strDatagram) = 0;
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

	virtual int LoadCardData(string& strCardID, SSCardBaseInfo& SSCardInfo) = 0;

	virtual int SaveCardData(string& strCardID, SSCardBaseInfo& SSCardInfo) = 0;

	virtual int QueryPayResult(string& strJsonIn, string& strJsonOut) = 0;

	virtual int SetExtraInterface(const string& strCommand, string& strInput, string& strOutput) = 0;

	virtual int GetCA(string& strJsonIn, string& strJsonout) = 0;

	virtual int GetPersonInfoFromStage(string& strJsonout) = 0;
};
