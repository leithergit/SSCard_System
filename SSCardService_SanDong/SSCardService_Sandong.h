#pragma once
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include "../SSCardService/SSCardService.h"
#include "../SDK/SSCardinfo_Binzhou/SSCardInfo_BinZhou.h"
#include "../SDK/glog/logging.h"
#include "Utility.h"
#include "CJsonObject.hpp"
#include "Markup.h"
//#include <QSettings>
//#include <QFileInfo>
//#include <QDebug>
#include <map>
#include <vector>
#include <filesystem>
using namespace std::filesystem;
namespace fs = std::filesystem;

//宏定义
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)")"
#define Warning(strMessage) message( __LOC__ " Warning: " strMessage)

extern map<string, string>g_mapNationnaltyCode;
#define  QFailed(x)		(x != 0)
#define  QSucceed(x)	(x == 0)

#define  X_UNUSED(x)	(void )x;

#ifdef _DEBUG
#pragma comment(lib,"glogd")
#else
#pragma comment(lib,"glog")
#endif

#define gInfo()      LOG(INFO)
#define gError()     LOG(ERROR)
#define gWarning()   LOG(WARNING)

using namespace std;
using namespace neb;
class SSCardService_BinZhou :
	public SSCardService
{
private:
	shared_ptr<SD_SSCardInfo> pCardInfo;
	std::string strOperator;
	string strCurService = "";
	string strJsonFile;
	string strPathFinished;
	bool	bAuthorize = false;		// 是否启用制卡校验
	bool	bOverBank = false;		// 是否为跨行操作
public:

	SSCardService_BinZhou()
	{
		strOperator = "金乔炜煜移动制卡机";
	}

	bool LoadProgress(CJsonObject& json)
	{
		try
		{
			gInfo() << "Try to load file:" << strJsonFile;
			ifstream ifs(strJsonFile, ios::in | ios::binary);
			stringstream ss;
			ss << ifs.rdbuf();
			return json.Parse(ss.str());
		}
		catch (std::exception& e)
		{
			gError() << "Catch a exception:" << e.what();
			return false;
		}
	}

	bool SaveProgress(CJsonObject& json)
	{
		try
		{
			ofstream ifs(strJsonFile, ios::out | ios::binary);
			ifs << json.ToFormattedString();
			return true;
		}
		catch (std::exception& e)
		{
			gError() << "Catch a exception:" << e.what();
			return false;
		}
	}

	bool IsFinishedProgress()
	{
		CJsonObject MainProgress;
		std::vector<string> vecKey;

		if (nServiceType == ServiceType::Service_NewCard)
		{
			if (!LoadProgress(MainProgress))
				return false;
			vecKey = std::move(vector<string>(
				{
					"chkCanCardSq",
					"saveCardSqList",
					//"queryCardZksqList",
					"saveCardOpen",
					"WriteCard",
					"PrintCard",
					"saveCardCompleted",
					"saveCardActive",
					"saveCardJrzhActive"
				}));
		}
		else if (nServiceType == ServiceType::Service_ReplaceCard)
		{
			if (!LoadProgress(MainProgress))
				return false;
			vecKey = std::move(vector<string>(
				{
					"saveCardGs",
					"chkCanCardBh",
					"queryCardInfoBySfzhm",
					"saveCardBhList",
					//"queryCardZksqList",
					"saveCardOpen",
					"WriteCard",
					"PrintCard",
					"saveCardCompleted",
					"saveCardActive",
					"saveCardJrzhActive"
				}));
		}
		else
			return true;

		if (!MainProgress.KeyExist(strCurService))
		{
			gError() << "Key " << strCurService << " not exist!";
			return false;
		}
		CJsonObject CurProgress = MainProgress[strCurService];
		for (auto var : vecKey)
		{
			if (!CurProgress.KeyExist(var))
				return false;
			CJsonObject jsonProc = CurProgress[var];
			int nCode = -1;
			jsonProc.Get("code", nCode);
			if (nCode)
				return false;
		}
		return true;
	}

	int  CheckOutJson(string& strOutInfo, CJsonObject& tmpJson, string& strMessage, string& strErrcode, int& nErrFlag, string strFunction = "")
	{
		if (!tmpJson.Parse(strOutInfo))
		{
			strMessage = strFunction + "返回结果无效:";
			strMessage += strOutInfo;
		}
		string strErrFlag;
		if (!tmpJson.Get("errflag", strErrFlag))
		{
			strMessage = strFunction + "的返回结果中找不到'errflag'标签!";
			return -1;
		}
		nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
		if (nErrFlag)
		{
			if (!tmpJson.KeyExist("errtext"))
				strMessage = strFunction + "的返回结果中找不到'errtext'标签!";
			else
			{
				tmpJson.Get("errtext", strMessage);
				tmpJson.Get("errcode", strErrcode);
			}
			return -1;
		}
		return 0;
	}

	void UpdateJson(CJsonObject& jsonIn, string strKey, string& strValue)
	{
		if (strValue.empty())
			return;
		if (jsonIn.KeyExist(strKey))
			jsonIn.Replace(strKey, strValue);
		else
			jsonIn.Add(strKey, strValue);
	}

	void UpdateJson(CJsonObject& jsonIn, string strKey, CJsonObject& jsonValue)
	{
		if (jsonIn.KeyExist(strKey))
			jsonIn.Replace(strKey, jsonValue);
		else
			jsonIn.Add(strKey, jsonValue);
	}

	void UpdateJson(CJsonObject& jsonIn, string strKey, int nValue)
	{
		if (jsonIn.KeyExist(strKey))
			jsonIn.Replace(strKey, nValue);
		else
			jsonIn.Add(strKey, nValue);
	}

	bool UpdatePerson()
	{
		CJsonObject MainProgress;
		if (!LoadProgress(MainProgress))
		{
			gError() << "Faled in load progress!";
			return false;
		}
		string strPerson = "Person";
		CJsonObject jsonPerson;
		if (MainProgress.KeyExist(strPerson))
			jsonPerson = MainProgress[strPerson];

		gInfo() << "update persion:" << pCardInfo->strName;
		UpdateJson(jsonPerson, "Name", pCardInfo->strName);
		UpdateJson(jsonPerson, "Identity", pCardInfo->strCardID);
		UpdateJson(jsonPerson, "Gender", pCardInfo->strSex);
		UpdateJson(jsonPerson, "Birthday", pCardInfo->strBirthday);
		UpdateJson(jsonPerson, "Nantion", pCardInfo->strNation);
		UpdateJson(jsonPerson, "PaperIssuedate", pCardInfo->strReleaseDate);
		UpdateJson(jsonPerson, "PaperExpiredate", pCardInfo->strValidDate);
		UpdateJson(jsonPerson, "Mobile", pCardInfo->strMobile);
		UpdateJson(jsonPerson, "Address", pCardInfo->strAdress);
		UpdateJson(jsonPerson, "BankCode", pCardInfo->strBankCode);
		UpdateJson(jsonPerson, "City", pCardInfo->strCity);
		int nAge = GetAge(pCardInfo->strBirthday);
		if (nAge >= 0 && nAge < 16)
		{
			UpdateJson(jsonPerson, "Guardian", pCardInfo->strGuardianName);
			UpdateJson(jsonPerson, "GuardianShip", pCardInfo->strGuardianType);
			UpdateJson(jsonPerson, "GuardianCardID", pCardInfo->strGuardianCardID);
		}

		UpdateJson(MainProgress, strPerson, jsonPerson);

		return SaveProgress(MainProgress);
	}

	virtual bool UpdateStage(string& strStage, int nResult, string strDatagram, bool bFinised = false) override
	{
		CJsonObject MainProgress;
		if (!LoadProgress(MainProgress))
		{
			gError() << "Faled in load progress!";
			return false;
		}
		CJsonObject CurProgress;
		if (MainProgress.KeyExist(strCurService))
			CurProgress = MainProgress[strCurService];

		auto tNow = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		stringstream curdate;

		// date1 = "2022-01-26 09:51:00"
		// date1 << std::put_time(std::localtime(&tNow), "%Y-%m-%d %X");
		curdate << std::put_time(std::localtime(&tNow), "%Y%m%d");
		if (bFinised)
		{
			UpdateJson(CurProgress, "ServiceDate", (string&)curdate.str());
			UpdateJson(CurProgress, "CardNum", pCardInfo->strCardNum);
		}
		CJsonObject jsonProc;
		if (CurProgress.KeyExist(strStage))
			jsonProc = CurProgress[strStage];

		UpdateJson(jsonProc, "Code", nResult);
		UpdateJson(jsonProc, "Datetime", (string&)curdate.str());
		UpdateJson(jsonProc, "datagram", strDatagram);
		UpdateJson(CurProgress, strStage, jsonProc);
		UpdateJson(MainProgress, strCurService, CurProgress);

		if (SaveProgress(MainProgress))
		{
			if (bFinised)
			{
				if (!fs::exists(strJsonFile))	// 最后一步已经完成,文件是否保存无关紧要
					return true;
				try
				{
					int nIndex = 0;
					string strBaseFile = strPathFinished;
					do
					{
						if (fs::exists(strBaseFile))
							strBaseFile = strPathFinished + std::to_string(nIndex++);
						else
							break;
					} while (true);
					fs::copy(strJsonFile, strBaseFile);
					fs::remove(strJsonFile);
				}
				catch (std::exception& e)
				{
					gInfo() << "catch a exception:" << e.what();
				}
			}
			return true;
		}
		else
			return false;
	}

	virtual int GetPersonInfoFromStage(string& strJsonout) override
	{
		CJsonObject MainProgress;
		if (!LoadProgress(MainProgress))
		{
			gError() << "Faled in load progress!";
			return false;
		}
		if (!MainProgress.KeyExist("Person"))
			return false;

		CJsonObject jsonPerson = MainProgress["Person"];
		strJsonout = jsonPerson.ToString();
		gInfo() << " get Person info from progress file:" << strJsonout;
		return true;
	}

	virtual int GetStageStatus(string strKey, int& nResult, string& strDatagram) override
	{
		CJsonObject MainProgress;
		nResult = -1;
		if (!LoadProgress(MainProgress))
		{
			gError() << "Faled in load progress!";
			return false;
		}
		if (!MainProgress.KeyExist(strCurService))
			return false;

		CJsonObject CurProgress = MainProgress[strCurService];

		if (!CurProgress.KeyExist(strKey))
			return false;

		CJsonObject jsonProc = CurProgress[strKey];
		jsonProc.Get("Code", nResult);
		if (QFailed(nResult))
			return false;
		jsonProc.Get("Datagram", strDatagram);
		gInfo() << strKey << " get Datagram from progress file!";
		return true;
	}

	virtual int SetServiceType(ServiceType nSvrType, string& strIdentity) override
	{
		if (nSvrType < ServiceType::Service_NewCard ||
			nSvrType > ServiceType::Service_QueryInformation)
			return -1;
		else
		{
			try
			{
				pCardInfo = make_shared<SD_SSCardInfo>();
				if (!pCardInfo)
				{
					gError() << "Insufficent memory!";
					return -1;
				}
				nServiceType = nSvrType;
				switch (nServiceType)
				{
				case ServiceType::Service_NewCard:
					strCurService = "NewCard";
					break;
				case ServiceType::Service_ReplaceCard:
					strCurService = "UpdateCard";
					break;
				case ServiceType::Service_RegisterLost:
				case ServiceType::Service_QueryInformation:
					strCurService = "";
					break;
				case ServiceType::Service_Unknown:
				default:
					return -1;
					break;
				}
				if (strCurService.size())
				{
					strJsonFile = "./Data/Progress_" + strIdentity + ".json";
					strPathFinished = "./Data/Finished/Progress_" + strIdentity + ".json";
					if (!exists("./Data/Finished"))
						filesystem::create_directories("./Data/Finished");
					if (!fs::exists(strJsonFile))
					{// 生成新的进度控制 文件
						gInfo() << "Create new progress:" << strJsonFile;
						CJsonObject jsonProgress;
						CJsonObject jsonPerson("{}");
						CJsonObject jsonService("{}");
						jsonProgress.Add("Person", jsonPerson);
						jsonProgress.Add(strCurService, jsonService);
						SaveProgress(jsonProgress);
					}
				}

				//if (exists(strJsonFile))
				//{
				//	if (IsFinishedProgress())
				//	{
				//		fs::copy(strJsonFile, strPathFinished);
				//		fs::copy("./Data/ProgressTemplate.json", strJsonFile);
				//		gInfo() << "Start a new progress for :" << strIdentity;
				//	}
				//	else
				//		gInfo() << "continue a exist progress for :" << strIdentity;
				//}
				//else
				//{
				//	fs::copy("./Data/ProgressTemplate.json", strJsonFile);
				//	gInfo() << "Start a new progress for :" << strIdentity;
				//}

				return 0;
			}
			catch (std::exception& e)
			{
				gError() << "Catch a exception:" << e.what();
				return -1;
			}
		}
	}

	virtual int Initialize(string& strInitJson, string& strOutInfo) override
	{
		CJsonObject jsonInit;
		if (!jsonInit.Parse(strInitJson))
		{
			strOutInfo = "卡管服务配置无法识别, 请检查SSCardService.json文件!";
			gError() << strOutInfo;
			return -1;
		}

		string strTemp;
		if (jsonInit.Get("Operator", strTemp))
		{
			// 经验办人
			strOperator = strTemp;
		}
		// 是否开启授权
		if (!jsonInit.Get("EnableAuthorize", bAuthorize))
			bAuthorize = false;

		//Warning("需要确定initCardInfo输入数据的Json定义!");
		return initCardInfo(strInitJson.c_str(), strOutInfo);
	}

	~SSCardService_BinZhou()
	{
	}

	virtual int SaveCardData(string& strCardID, SSCardBaseInfo& SSCardInfo) override
	{
		X_UNUSED(SSCardInfo);
		X_UNUSED(strCardID);
		//#define AddCardFiled(x,s)	x.setValue(#s,CardInfo.s.c_str());
		//
		//		QSettings CardIni(strINIFile.c_str(), QSettings::IniFormat);
		//		CardIni.beginGroup("CardData");
		//
		//		AddCardFiled(CardIni, strCardNum);			//社保卡号
		//		AddCardFiled(CardIni, strCardID);			//身份证号
		//		AddCardFiled(CardIni, strBirthday);			//出生日期
		//		AddCardFiled(CardIni, strName);				//姓名
		//		AddCardFiled(CardIni, strSex);				//性别
		//		AddCardFiled(CardIni, strNation);			//民族
		//		AddCardFiled(CardIni, strMobile);			//手机号
		//		AddCardFiled(CardIni, strAdress);			//通讯地址
		//		AddCardFiled(CardIni, strPostalCode);		//邮政编码
		//		AddCardFiled(CardIni, strEmail);			//电子邮箱
		//		AddCardFiled(CardIni, strGuardianName);		//监护人姓名
		//		AddCardFiled(CardIni, strCity);				//所属城市
		//		AddCardFiled(CardIni, strSSQX);				//所属区县
		//		AddCardFiled(CardIni, strBankCode);			//银行代码
		//		AddCardFiled(CardIni, strReleaseDate);		//发卡日期
		//		AddCardFiled(CardIni, strValidDate);		//有效日期
		//		AddCardFiled(CardIni, strIdentifyNum);		//卡识别码
		//		AddCardFiled(CardIni, strCardATR);			//卡复位信息
		//		AddCardFiled(CardIni, strBankNum);			//银行卡号
		//		AddCardFiled(CardIni, strCardType);			//证件类型
		//		AddCardFiled(CardIni, strOperator);			//经办人
		//		AddCardFiled(CardIni, strOccupType);		//职业类别
		//		AddCardFiled(CardIni, strDealType);			//办卡类型
		//		AddCardFiled(CardIni, strChipNum);			//卡芯片账号
		//		AddCardFiled(CardIni, strMagNum);			//卡磁条账号
		//		AddCardFiled(CardIni, strCardVersion);		//卡规范版本
		//		AddCardFiled(CardIni, strChipType);			//芯片类型
		//		AddCardFiled(CardIni, strCardUnit);			//发证机关
		//		AddCardFiled(CardIni, strBirthPlace);		//出生地
		//		AddCardFiled(CardIni, strPerAddr);			//户籍所在地
		//		AddCardFiled(CardIni, strRegAttr);			//户口性质
		//		AddCardFiled(CardIni, strEducation);		//文化程度
		//		AddCardFiled(CardIni, strMarital);			//婚姻状况
		//		AddCardFiled(CardIni, strContactsName);		//联系人姓名
		//		AddCardFiled(CardIni, strContactsMobile);	//联系人电话
		//		AddCardFiled(CardIni, strReason);			//补换原因
		//		AddCardFiled(CardIni, strPhoto);			//相片
		//
		//		CardIni.endGroup();
		return 0;
	}

	virtual int LoadCardData(string& strCardID, SSCardBaseInfo& SSCardInfo) override
	{
		X_UNUSED(SSCardInfo);
		X_UNUSED(strCardID);
		//		QFileInfo fi(strINIFile.c_str());
		//		if (!fi.isFile())
		//		{
		//			return -1;
		//		}
		//#define GetCardField(x,c,f)		c.f =  x.value(#f).toString().toStdString();
		//		QSettings CardIni(strINIFile.c_str(), QSettings::IniFormat);
		//		CardInfo.strCardNum = "";
		//		CardIni.beginGroup("CardData");
		//
		//		GetCardField(CardIni, CardInfo, strCardNum);		//社保卡号
		//		GetCardField(CardIni, CardInfo, strCardID);			//身份证号
		//		GetCardField(CardIni, CardInfo, strBirthday);		//出生日期
		//		GetCardField(CardIni, CardInfo, strName);			//姓名
		//		GetCardField(CardIni, CardInfo, strSex);			//性别
		//		GetCardField(CardIni, CardInfo, strNation);			//民族
		//		GetCardField(CardIni, CardInfo, strMobile);			//手机号
		//		GetCardField(CardIni, CardInfo, strAdress);			//通讯地址
		//		GetCardField(CardIni, CardInfo, strPostalCode);		//邮政编码
		//		GetCardField(CardIni, CardInfo, strEmail);			//电子邮箱
		//		GetCardField(CardIni, CardInfo, strGuardianName);	//监护人姓名
		//		GetCardField(CardIni, CardInfo, strCity);			//所属城市
		//		GetCardField(CardIni, CardInfo, strSSQX);			//所属区县
		//		GetCardField(CardIni, CardInfo, strBankCode);		//银行代码
		//		GetCardField(CardIni, CardInfo, strReleaseDate);	//发卡日期
		//		GetCardField(CardIni, CardInfo, strValidDate);		//有效日期
		//		GetCardField(CardIni, CardInfo, strIdentifyNum);	//卡识别码
		//		GetCardField(CardIni, CardInfo, strCardATR);		//卡复位信息
		//		GetCardField(CardIni, CardInfo, strBankNum);		//银行卡号
		//		GetCardField(CardIni, CardInfo, strCardType);		//证件类型
		//		GetCardField(CardIni, CardInfo, strOperator);		//经办人
		//		GetCardField(CardIni, CardInfo, strOccupType);		//职业类别
		//		GetCardField(CardIni, CardInfo, strDealType);		//办卡类型
		//		GetCardField(CardIni, CardInfo, strChipNum);		//卡芯片账号
		//		GetCardField(CardIni, CardInfo, strMagNum);			//卡磁条账号
		//		GetCardField(CardIni, CardInfo, strCardVersion);	//卡规范版本
		//		GetCardField(CardIni, CardInfo, strChipType);		//芯片类型
		//		GetCardField(CardIni, CardInfo, strCardUnit);		//发证机关
		//		GetCardField(CardIni, CardInfo, strBirthPlace);		//出生地
		//		GetCardField(CardIni, CardInfo, strPerAddr);		//户籍所在地
		//		GetCardField(CardIni, CardInfo, strRegAttr);		//户口性质
		//		GetCardField(CardIni, CardInfo, strEducation);		//文化程度
		//		GetCardField(CardIni, CardInfo, strMarital);		//婚姻状况
		//		GetCardField(CardIni, CardInfo, strContactsName);	//联系人姓名
		//		GetCardField(CardIni, CardInfo, strContactsMobile);	//联系人电话
		//		GetCardField(CardIni, CardInfo, strReason);			//补换原因
		//		//GetCardField(CardIni, CardInfo,strPhoto);			//相片
		//		QByteArray ba = CardIni.value("strPhoto").toString().toLatin1();
		//		qDebug() << "Load photo,size = " << ba.size();
		//		CardInfo.strPhoto = string(ba.data(), ba.size());
		//		CardIni.endGroup();
		return 0;
	}

	void ProcessErrcode(int nErrCode, string& strMessage)
	{
		switch (nErrCode)
		{
		case 1:
			strMessage = "人员不存在";
			break;
		case 2:
			strMessage = "人员无法核实(姓名不一致)";
			break;
		case 3:
			strMessage = "重复申请";
			break;
		case 4:
			strMessage = "不允许办理(本地有卡、省卡管有卡、死亡、本地有制卡申请)";
			break;
		default:
			strMessage = "未知错误";
			break;
		}
	}

	// 查询卡状态
	// 输入
	// {
	// "CardID":"33333333333333",
	// "Name":"",
	// "City":"",			// 行政区域
	// "BankCode":""
	//}
	// cardID,name,city,bankcode,PaperType
	int QueryNewCardStatus(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			if (jsonIn.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}

			jsonIn.Get("CardID", pCardInfo->strCardID);
			jsonIn.Get("Name", pCardInfo->strName);
			jsonIn.Get("City", pCardInfo->strCity);
			jsonIn.Get("BankCode", pCardInfo->strBankCode);


			pCardInfo->strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (pCardInfo->strCardID.empty()
				|| pCardInfo->strName.empty()
				|| pCardInfo->strCity.empty()
				|| pCardInfo->strBankCode.empty()
				//||CardInfo.strCardType.empty()
				)
			{
				strMessage = "身份证,姓名,城市代码或银行代码不能为空!";
				break;
			}

			UpdatePerson();
			strKey = "chkCanCardSq";
			string strFunction = "申领卡信息校验(chkCanCardSq)";
			if (!GetStageStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			{
				if (QFailed(nSSResult = chkCanCardSq(*pCardInfo, strOutInfo)))
				{
					strMessage = strFunction + "失败:";
					strMessage += std::to_string(nSSResult);
					break;
				}
				CJsonObject tmpJson;
				if (QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction)))
					break;

				UpdateStage(strKey, nErrFlag, tmpJson.ToFormattedString());
			}
			strMessage = "Succeed";
			nResult = 0;
			nErrFlag = 0;

		} while (0);

		CJsonObject jsonOut;
		jsonOut.Add("Result", nErrFlag);
		jsonOut.Add("Message", strMessage);
		jsonOut.Add("errcode", strErrcode);
		strJsonOut = jsonOut.ToString();

		return nResult;
	}

	// 输入
	// {
	// "CardID":"33333333333333",
	// "Name":"",
	// "City":"",			// 行政区域
	// "BankCode":""
	//}
	// ouput :
	/*
		Name
		CardID
		CardNum
		CardStatus
		BankName
		Mobile
	*/
	int QueryOldCardStatus(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			jsonIn.Get("CardID", pCardInfo->strCardID);
			jsonIn.Get("Name", pCardInfo->strName);
			jsonIn.Get("City", pCardInfo->strCity);
			jsonIn.Get("BankCode", pCardInfo->strBankCode);
			pCardInfo->strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (pCardInfo->strCardID.empty() ||
				pCardInfo->strName.empty() ||
				pCardInfo->strCity.empty() ||
				pCardInfo->strBankCode.empty())
			{
				strMessage = "身份证,姓名,城市代码或银行代码不能为空!";
				break;
			}
			UpdatePerson();
			strKey = "chkCanCardBh";
			if (!GetStageStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			{
				string strFunction = "补换卡信息校验(chkCanCardSq)";
				if (QFailed(nSSResult = chkCanCardBh(*pCardInfo, strOutInfo)))
				{
					strMessage = strFunction + "失败:";
					strMessage += strJsonOut;
					break;
				}

				CJsonObject tmpJson;
				if (QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction)))
					break;

				UpdateStage(strKey, nErrFlag, tmpJson.ToFormattedString());
			}

			strMessage = "Succeed";
			nResult = 0;
			nErrFlag = 0;
		} while (0);

		CJsonObject jsonOut;
		jsonOut.Add("Result", nErrFlag);
		jsonOut.Add("Message", strMessage);
		jsonOut.Add("errcode", strErrcode);
		strJsonOut = jsonOut.ToString();

		return nResult;
	}

	int GetAge(string& strBirthday)
	{
		if (strBirthday.empty())
			return -1;

		int nBirthYear, nBirthMonth, nBirthDay;
		int nCount = sscanf_s(strBirthday.c_str(), "%04d%02d%02d", &nBirthYear, &nBirthMonth, &nBirthDay);
		if (nCount != 3)
			return -1;

		time_t tNow = time(nullptr);
		tm* tmNow = localtime(&tNow);
		tmNow->tm_year += 1900;
		tmNow->tm_mon += 1;

		int nAge = tmNow->tm_year - nBirthYear;
		if (tmNow->tm_mon < nBirthMonth)
		{
			nAge--;
		}
		else if (tmNow->tm_mon == nBirthMonth)
		{
			if (tmNow->tm_mday < nBirthDay)
				nAge--;
		}
		return nAge;
	}


	int QueryCardInfo(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		string strCardStatusToQuery = "1";
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			jsonIn.Get("CardID", pCardInfo->strCardID);
			jsonIn.Get("Name", pCardInfo->strName);
			jsonIn.Get("City", pCardInfo->strCity);
			jsonIn.Get("BankCode", pCardInfo->strBankCode);
			jsonIn.Get("StatustoQuery", strCardStatusToQuery);
			pCardInfo->strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (pCardInfo->strCardID.empty() ||
				pCardInfo->strName.empty() ||
				pCardInfo->strCity.empty() ||
				pCardInfo->strBankCode.empty())
			{
				strMessage = "身份证,姓名,城市代码或银行代码不能为空!";
				break;
			}
			CJsonObject outJson;
			UpdatePerson();
			strKey = "queryCardInfoBySfzhm";
			if (//!GetStageStatus(strKey, nResult, strDatagram) ||
				//QFailed(nResult) ||
				nServiceType == ServiceType::Service_RegisterLost ||
				nServiceType == ServiceType::Service_QueryInformation)
			{
				string strFunction = "查询社保卡信息(queryCardInfoBySfzhm)";
				if (QFailed(nSSResult = queryCardInfoBySfzhm(*pCardInfo, strOutInfo, strCardStatusToQuery)))
				{
					strMessage = strFunction + "失败:";
					strMessage += strOutInfo;
					break;
				}
				CJsonObject tmpJson;
				if (QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction)))
					break;

				UpdateStage(strKey, nErrFlag, tmpJson.ToFormattedString());
				outJson.Parse(strOutInfo);
			}
			/*else
				;*/

			CJsonObject ds = outJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardInfoBySfzhm is invalid !";
				break;
			}

			ds.Get("shbzhm", pCardInfo->strCardID);
			ds.Get("kh", pCardInfo->strCardNum);
			ds.Get("xm", pCardInfo->strName);
			ds.Get("kxpzh", pCardInfo->strChipNum);
			ds.Get("kctzh", pCardInfo->strMagNum);
			ds.Get("yhbh", pCardInfo->strBankCode);		// 银行编号
			ds.Get("sjhm", pCardInfo->strMobile);
			ds.Get("yhbh", pCardInfo->strBankCode);
			ds.Get("sjhm", pCardInfo->strMobile);

			string strCardStatus;
			string strBankName;
			ds.Get("kztmc", strCardStatus);

			const char* szStatus[] = {
				"放号",
				"正常",
				"挂失",
				"临时挂失",
				"挂失后注销",
				"注销",
				"未启用"
			};
			CardStatus nCardStatus = CardStatus::Card_Unknow;
			int nIndex = 0;
			for (auto var : szStatus)
			{
				if (strCardStatus == var)
				{
					nCardStatus = (CardStatus)nIndex;
				}
				nIndex++;
			}

			ds.Get("yhmc", strBankName);

			//CardInfo.strst = ds["kzt"].ToString();
			//PersionInfo.strCardStatus = ds["kztmc"] .ToString();	// 卡状态名称
			//PersionInfo.strBankCode = ds["yhmc"] .ToString();		// 银行名称

			outJson.Clear();
			nResult = 0;
			outJson.Add("Result", nResult);
			outJson.Add("Name", pCardInfo->strName);
			outJson.Add("CardID", pCardInfo->strCardID);
			outJson.Add("CardNum", pCardInfo->strCardNum);
			outJson.Add("BankName", strBankName);
			outJson.Add("BankCode", pCardInfo->strBankCode);
			outJson.Add("CardStatus", (int)nCardStatus);
			outJson.Add("Mobile", pCardInfo->strMobile);

			strJsonOut = outJson.ToString();

			nErrFlag = 0;

		} while (0);
		if (nResult)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}
		return nResult;
	}

	/*
	input string
		{
		"CardID":"33333333333333",
		"Name":"",
		"City":"",			// 行政区域
		"BankCode":""
	}

	output string
	{
		"message":""
	}
	*/

	virtual int QueryCardStatus(string& strJsonIn, string& strJsonOut) override
	{
		switch (nServiceType)
		{
		case ServiceType::Service_NewCard:
			return QueryNewCardStatus(strJsonIn, strJsonOut);
			break;
		case ServiceType::Service_ReplaceCard:
			return QueryOldCardStatus(strJsonIn, strJsonOut);
			break;
		case ServiceType::Service_RegisterLost:
		case ServiceType::Service_QueryInformation:
		default:
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", 1);
			jsonOut.Add("Message", "Invalid Service Type!");
			return -1;
			break;
		}
		}
	}

	/*
	input string:
	{
	"IssueDate":"",
	"ExpireDate":"",
	"Mobile":"",
	"Operator":"",
	"OccupType":"",
	"Birthday":"",
	"Gender":"",
	"Nation":"",
	"Address":"",
	"Photo":""
	}
	output string:
	{
	"Name":"",
	"CardID":"",
	"CardNum":"",
	"Mobile":"",
	"BankCode":""
	}
	*/
	int CheckAuthorize(SD_SSCardInfo& pSSCardInfo, string& strMessage)
	{
		if (!bAuthorize)		// 授权尚未启用
			return 0;
		string strFunction = "校验授权信息(checkAuthorizeInfo)";
		string strOutInfo;
		if (QFailed(checkAuthorizeInfo(pSSCardInfo, strOutInfo)))
		{
			strMessage = strFunction + "失败:";
			strMessage += strOutInfo;
			return 1;
		}
		string strErrcode;
		int nErrFlag;
		CJsonObject tmpJson;
		if (QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction)))
			return 1;

		int nAuthorizeFlag = 0;
		if (!tmpJson.Get("sqbz", nAuthorizeFlag))
		{
			strMessage = strFunction + "返回结果找不到\"sqbz\"标签!";
			return 1;
		}
		if (nAuthorizeFlag == 1) // 已授权
			return 0;

		auto tNow = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		stringstream Datetime;

		Datetime << std::put_time(std::localtime(&tNow), "%Y%m%d%H%M%S");
		pSSCardInfo.strAuthorizeTime = Datetime.str();

		strFunction = "生成授权信息(makeAuthorizeInfo)";
		if (QFailed(makeAuthorizeInfo(pSSCardInfo, strOutInfo)))
		{
			strMessage = strFunction + "失败:";
			strMessage += strOutInfo;
			return 1;
		}
		if (QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction)))
			return 1;
		return 0;
	}

	int CommitNewCardInfo(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		//int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			if (jsonIn.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}
			string strGender;
			pCardInfo->strDealType = "0";	// new card
			pCardInfo->strOperator = strOperator;
			pCardInfo->strCardType = "A";
			//CardInfo.strOccupType = "2000000";
			jsonIn.Get("CardID", pCardInfo->strCardID);
			jsonIn.Get("Name", pCardInfo->strName);
			jsonIn.Get("BankCode", pCardInfo->strBankCode);
			jsonIn.Get("City", pCardInfo->strCity);
			jsonIn.Get("Mobile", pCardInfo->strMobile);
			jsonIn.Get("IssueDate", pCardInfo->strReleaseDate);
			jsonIn.Get("ExpireDate", pCardInfo->strValidDate);
			jsonIn.Get("Mobile", pCardInfo->strMobile);
			jsonIn.Get("Birthday", pCardInfo->strBirthday);
			jsonIn.Get("Gender", strGender);
			jsonIn.Get("Nation", pCardInfo->strNation);
			jsonIn.Get("Address", pCardInfo->strAdress);
			jsonIn.Get("Photo", pCardInfo->strPhoto);
			jsonIn.Get("Occupation", pCardInfo->strOccupType);
			//jsonIn.Get("AuthorizeTime", pCardInfo->strAuthorizeTime);
			jsonIn.Get("AuthorizeType", pCardInfo->strAuthorizeType);
			if (bAuthorize)
			{
				CJsonObject jsonAuthrizeData;
				int nArraySize = 0;
				if (!jsonIn.Get("AuthorizeData", jsonAuthrizeData) ||
					!jsonAuthrizeData.IsArray() ||
					!(nArraySize = jsonAuthrizeData.GetArraySize()))
				{
					strMessage = "没有授权数据或授权数据无效!";
					break;
				}
				for (int i = 0; i < nArraySize; i++)
					pCardInfo->listAuthorizeData.emplace_back(jsonAuthrizeData(i));
			}

			if (pCardInfo->strOccupType.empty())
				pCardInfo->strOccupType = "8000000";

			if (strGender == "男")
				pCardInfo->strSex = "1";
			else if (strGender == "女")
				pCardInfo->strSex = "2";
			else
				pCardInfo->strSex = "9";

			if (pCardInfo->strCardID.empty()
				|| pCardInfo->strName.empty()
				|| pCardInfo->strCity.empty()
				|| pCardInfo->strMobile.empty()
				|| pCardInfo->strBirthday.empty()
				|| pCardInfo->strAdress.empty()
				|| pCardInfo->strPhoto.empty()
				|| strGender.empty()
				|| pCardInfo->strNation.empty())
			{
				strMessage = "身份证,姓名,城市代码,银行代码,手机,证件有效期,生日,性别,民族,住址或照片不能为空!";
				break;
			}
			int nAge = GetAge(pCardInfo->strBirthday);
			if (nAge < 0)
			{
				strMessage = "生日不是一个有效的日期!";
				break;
			}
			bool bByGuardian = false;
			if (nAge < 16 || jsonIn.KeyExist("ByGuardian"))
			{
				bByGuardian = true;
				jsonIn.Get("Guardian", pCardInfo->strGuardianName);
				jsonIn.Get("GuardianShip", pCardInfo->strGuardianType);
				jsonIn.Get("GuardianCardID", pCardInfo->strGuardianCardID);
				pCardInfo->strGuardianCardType = "A";
				if (pCardInfo->strGuardianName.empty() ||
					pCardInfo->strGuardianCardID.empty() ||
					pCardInfo->strGuardianType.empty())
				{
					strMessage = "监护人姓名,监护人身份证号码,监护关系不能为空!";
					break;
				}
			}
			else if (nAge >= 16)
			{
				if (pCardInfo->strReleaseDate.empty()
					|| pCardInfo->strValidDate.empty())
				{
					strMessage = "16周岁及以上人员证件有效期不能为空!";
					break;
				}
			}
			/*auto itFind = g_mapNationnaltyCode.find(strNationality);
			if (itFind != g_mapNationnaltyCode.end())
			{
				CardInfo.strNation = itFind->second;
			}
			else
			{
				strMessage = "民族无效!";
				break;
			}*/
			string strErrFlag;
			UpdatePerson();
			CJsonObject tmpJson;
			int nSSResult = -1;
			string strFunction;


			strKey = "saveCardSqList";

			if (!GetStageStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			{
				if (QFailed(CheckAuthorize(*pCardInfo, strMessage)))
					break;
				strFunction = "提交申领名单(saveCardSqList)";
				if (bByGuardian)
				{
					strFunction = "监护提交申领名单(saveCardSqListByGuardian)";
					if (QFailed(nSSResult = saveCardSqListByGuardian(*pCardInfo, strOutInfo)))
					{
						strMessage = strFunction + "失败:";
						strMessage += strOutInfo;
					}
				}
				else if (QFailed(nSSResult = saveCardSqList(*pCardInfo, strOutInfo)))
				{
					strMessage = strFunction + "失败:";
					strMessage += strOutInfo;
					break;
				}
				if (QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction)))
					break;

				UpdateStage(strKey, 0, tmpJson.ToFormattedString());
			}
			//auto tNow = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			//auto tNext = chrono::system_clock::to_time_t(std::chrono::system_clock::now() + std::chrono::hours(24));
			//stringstream date1, date2;
			//// date1 = "2022-01-26 09:51:00"
			//// date1 << std::put_time(std::localtime(&tNow), "%Y-%m-%d %X");
			//date1 << std::put_time(std::localtime(&tNow), "%Y%m%d");
			//// 20210126
			//date2 << std::put_time(std::localtime(&tNext), "%Y%m%d");

			//CardInfo.strReleaseDate = date1.str();
			//CardInfo.strValidDate = date2.str();
			//strKey = "queryCardZksqList";
			////CJsonObject outJson;
			//if (!GetProgressStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			//{
			//	// city,bankcode,validDate,dealType,releaseDate
			//	if (QFailed(nSSResult = queryCardZksqList(CardInfo, strOutInfo)))
			//	{
			//		strMessage = "Faled in queryCardZksqList:";
			//		strMessage += strOutInfo;
			//		break;
			//	}
			//	if (!tmpJson.Parse(strOutInfo))
			//	{
			//		strMessage = "queryCardZksqList output is invalid:";
			//		strMessage += strOutInfo;
			//		break;
			//	}
			//	if (!tmpJson.Get("errflag", strErrFlag))
			//	{
			//		strMessage = "can't locate field 'errflag' in output of queryCardZksqList!";
			//		break;
			//	}
			//	nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			//	if (nErrFlag)
			//	{
			//		if (!tmpJson.KeyExist("errtext"))
			//			strMessage = "can't locate field 'errtext' in output of queryCardZksqList!";
			//		else
			//		{
			//			tmpJson.Get("errtext", strMessage);
			//			tmpJson.Get("errcode", strErrcode);
			//		}
			//		break;
			//	}
			//	UpdateCurProgress(strKey, 0, tmpJson.ToFormattedString());
			//}
			//else
			//	tmpJson.Parse(strDatagram);
			//CJsonObject ds = tmpJson["ds"];
			//if (!ds.KeyExist("xm") ||
			//	!ds.KeyExist("shbzhm"))
			//{
			//	strMessage = "can't locate field 'xm' or 'shbzhm' from output of queryCardZksqList!";
			//	break;
			//}
			//if (ds.IsEmpty())
			//{
			//	strMessage = "ds from output of queryCardZksqList is invalid !";
			//	break;
			//}
			//CardInfo.strCardNum = ds["shbzhm"].ToString();
			//CardInfo.strMobile = ds["sjhm"].ToString();
			//CJsonObject outJson;
			//outJson.Add("Name", CardInfo.strName);
			//outJson.Add("CardID", CardInfo.strCardID);
			//outJson.Add("CardNum", CardInfo.strCardNum);
			//outJson.Add("Mobile", CardInfo.strMobile);
			//outJson.Add("BankCode", tmpJson["yhbh"].ToString());
			//strJsonOut = outJson.ToString();
			nErrFlag = 0;
			nResult = 0;
		} while (0);
		if (nResult)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}
		return nResult;
	}

	// cardID,cardType,name,bankCode,city,mobile,reason,operator,OccupType,birthday,sex,nation,address,photo
	int CommitReplaceCardInfo(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject json(strJsonIn);
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "补换卡信息为空!";
				break;
			}
			string strGender;
			pCardInfo->strDealType = "1";	// replace card
			pCardInfo->strCardType = "A";	// 仅支持身份证//json["PaperType"].ToString();
			json.Get("OverBank", bOverBank);	// 是否为跨行操作

			json.Get("CardID", pCardInfo->strCardID);
			json.Get("Name", pCardInfo->strName);
			json.Get("BankCode", pCardInfo->strBankCode);
			json.Get("City", pCardInfo->strCity);
			json.Get("Mobile", pCardInfo->strMobile);
			pCardInfo->strReason = "Replace Card!";
			pCardInfo->strOperator = strOperator;
			//json.Get("Reason", CardInfo.strReason);
			//json.Get("Operator", CardInfo.strOperator);
			//json.Get("OccupType", CardInfo.strOccupType);
			pCardInfo->strOccupType = "8000000";
			json.Get("IssueDate", pCardInfo->strReleaseDate);
			json.Get("ExpireDate", pCardInfo->strValidDate);
			json.Get("Birthday", pCardInfo->strBirthday);
			json.Get("Gender", strGender);
			json.Get("Nation", pCardInfo->strNation);
			json.Get("Address", pCardInfo->strAdress);
			json.Get("CardNum", pCardInfo->strCardNum);
			json.Get("Photo", pCardInfo->strPhoto);
			json.Get("Occupation", pCardInfo->strOccupType);
			json.Get("AuthorizeType", pCardInfo->strAuthorizeType);
			json.Get("AuthorizeTime", pCardInfo->strAuthorizeTime);
			if (bAuthorize)
			{
				CJsonObject jsonAuthrizeData;
				int nArraySize = 0;
				if (!json.Get("AuthorizeData", jsonAuthrizeData) ||
					!jsonAuthrizeData.IsArray() ||
					!(nArraySize = jsonAuthrizeData.GetArraySize()))
				{
					strMessage = "没有授权数据或授权数据无效!";
					break;
				}
				for (int i = 0; i < nArraySize; i++)
					pCardInfo->listAuthorizeData.emplace_back(jsonAuthrizeData(i));
			}

			if (pCardInfo->strOccupType.empty())
				pCardInfo->strOccupType = "8000000";

			if (pCardInfo->strCardID.empty()
				|| pCardInfo->strName.empty()
				|| pCardInfo->strCity.empty()
				|| pCardInfo->strMobile.empty()
				|| pCardInfo->strBirthday.empty()
				|| pCardInfo->strAdress.empty()
				|| pCardInfo->strPhoto.empty()
				|| strGender.empty()
				|| pCardInfo->strNation.empty())
			{
				strMessage = "身份证,姓名,城市代码,银行代码,手机,证件有效期,生日,性别,民族,住址或照片不能为空!";
				break;
			}
			int nAge = GetAge(pCardInfo->strBirthday);
			if (nAge < 0)
			{
				strMessage = "生日不是一个有效的日期!";
				break;
			}
			bool bByGuardian = false;
			if (nAge < 16 || json.KeyExist("ByGuardian"))
			{
				bByGuardian = true;
				string strGuardianShip;
				json.Get("Guardian", pCardInfo->strGuardianName);
				json.Get("GuardianShip", strGuardianShip);

				char szGuardianShip[64] = { 0 };
				ANSI2UTF8(strGuardianShip.c_str(), szGuardianShip, 64);
				if (strcmp(szGuardianShip, "父子"))
					pCardInfo->strGuardianType = "1";
				else if (strcmp(szGuardianShip, "父女"))
					pCardInfo->strGuardianType = "2";
				else if (strcmp(szGuardianShip, "母子"))
					pCardInfo->strGuardianType = "3";
				else if (strcmp(szGuardianShip, "母女"))
					pCardInfo->strGuardianType = "4";
				else
				{
					strMessage = "监护关系有误,只能是父子,父女,母子,母女中的一种!";
					break;
				}

				json.Get("GuardianCardID", pCardInfo->strGuardianCardID);
				pCardInfo->strGuardianCardType = "A";
				if (pCardInfo->strGuardianName.empty() ||
					pCardInfo->strGuardianCardID.empty() ||
					pCardInfo->strGuardianType.empty())
				{
					strMessage = "监护人姓名,监护人身份证号码,监护关系不能为空!";
					break;
				}
			}
			else if (nAge >= 16)
			{
				if (pCardInfo->strReleaseDate.empty()
					|| pCardInfo->strValidDate.empty())
				{
					strMessage = "16周岁及以上人员证件有效期不能为空!";
					break;
				}
			}

			if (strGender == "男")
				pCardInfo->strSex = "1";
			else if (strGender == "女")
				pCardInfo->strSex = "2";
			else
				pCardInfo->strSex = "9";


			UpdatePerson();
			strKey = "saveCardBhList";
			string strErrFlag;
			CJsonObject tmpJson;
			string strFunction;
			if (!GetStageStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			{
				if (QFailed(CheckAuthorize(*pCardInfo, strMessage)))
					break;

				if (bByGuardian)
				{
					if (QFailed(saveCardBhListByGuardian(*pCardInfo, strOutInfo)))
					{
						strFunction = "监护人代申请补换卡(saveCardBhListByGuardian)";
						strMessage = strFunction + "失败:";
						strMessage += strOutInfo;
						break;
					}
				}
				else if (QFailed(saveCardBhList(*pCardInfo, strOutInfo)))
				{
					strFunction = "申请补换卡(saveCardBhList)";
					strMessage = "失败:";
					strMessage += strOutInfo;
					break;
				}
				if QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction))
					break;

				UpdateStage(strKey, 0, tmpJson.ToFormattedString());
			}
			auto tNow = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			auto tNext = chrono::system_clock::to_time_t(std::chrono::system_clock::now() + std::chrono::hours(24));
			stringstream date1, date2;

			// date1 = "2022-01-26 09:51:00"
			// date1 << std::put_time(std::localtime(&tNow), "%Y-%m-%d %X");
			date1 << std::put_time(std::localtime(&tNow), "%Y%m%d");
			// 20210126
			date2 << std::put_time(std::localtime(&tNext), "%Y%m%d");

			pCardInfo->strReleaseDate = date1.str();
			pCardInfo->strValidDate = date2.str();

			nResult = 0;
			nErrFlag = 0;
		} while (0);
		if (nErrFlag)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}

		return nResult;
	}

	// 提交制卡人信息
	//	"CardID"
	//	"PaperType"
	//	"Name"
	//	"BankCode"
	//	"City"
	//	"Mobile"
	//	"Reason"
	//	"Operator"
	//	"OccupType"
	//	"IssueDate"
	//	"ExpireDate"
	//	"Birthday"
	//	"Gender"
	//	"Nation"
	//	"Address"
	//	"Photo"
	virtual int CommitPersonInfo(string& strJsonIn, string& strJsonOut) override
	{
		switch (nServiceType)
		{
		case ServiceType::Service_NewCard:
			return CommitNewCardInfo(strJsonIn, strJsonOut);
			break;
		case ServiceType::Service_ReplaceCard:
			return CommitReplaceCardInfo(strJsonIn, strJsonOut);
			break;
		case ServiceType::Service_RegisterLost:
		case ServiceType::Service_QueryInformation:
		default:
		{
			CJsonObject jsonOut;
			jsonOut.Add("Message", "Invalid Service Type!");
			return -1;
			break;
		}
		}

		return -1;
	}

	// 预制卡 cardID,cardType,name,bankCode,operator,city
	/*
	Input :none
	ouput:
	{
		message:""
	}
	*/
	virtual int PreMakeCard(string& strJsonIn, string& strJsonOut) override
	{
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		CJsonObject json(strJsonIn);
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}
			string strGender, strNationality, strPhoto;
			pCardInfo->strDealType = "1";	// replace card
			pCardInfo->strCardType = "A";// 仅支持身份证//json["PaperType"].ToString();
			pCardInfo->strOperator = strOperator;
			json.Get("CardID", pCardInfo->strCardID);
			json.Get("Name", pCardInfo->strName);
			json.Get("BankCode", pCardInfo->strBankCode);
			json.Get("City", pCardInfo->strCity);

			if (pCardInfo->strCardID.empty() ||
				pCardInfo->strName.empty() ||
				pCardInfo->strBankCode.empty() ||
				pCardInfo->strCity.empty())
			{
				strMessage = "身份号码,姓名,银行代码或城市代码中存在空字段!";
				break;
			}

			strKey = "saveCardOpen";
			CJsonObject tmpJson;

			if (!GetStageStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			{
				string strFunction = "即制卡开户(saveCardOpen)";
				// cardID,cardType,name,bankCode,operator,city
				if (QFailed(nSSResult = saveCardOpen(*pCardInfo, strOutInfo)))
				{
					strMessage = strFunction + "失败:";
					strMessage += strOutInfo;
					break;
				}

				if QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction))
					break;

				UpdateStage(strKey, 0, tmpJson.ToFormattedString());
				try
				{
					string strOutputFile = "./Debug/Carddata_" + pCardInfo->strCardID + ".json";
					fstream fs(strOutputFile, ios::out);
					fs << strOutInfo;
					fs.close();
				}
				catch (std::exception& e)
				{
					string strException = e.what();
				}
			}
			else
				tmpJson.Parse(strDatagram);

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardZksqList is invalid !";
				break;
			}
			ds.Get("sbkh", pCardInfo->strCardNum);
			ds.Get("zp", pCardInfo->strPhoto);
			ds.Get("mz", pCardInfo->strNation);

			CJsonObject outJson;
			outJson.Add("CardNum", pCardInfo->strCardNum);
			outJson.Add("Photo", pCardInfo->strPhoto);
			UpdatePerson();
			strJsonOut = outJson.ToString();
			nResult = 0;
			nErrFlag = 0;
		} while (0);
		if (nErrFlag)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}

		return nResult;
	}

	// 读卡
	virtual int ReadCard(string& strJsonIn, string& strJsonOut) override
	{
		X_UNUSED(strJsonIn);
		X_UNUSED(strJsonOut);
		return 0;
	}

	// 写卡
	virtual int WriteCard(string& strJsonIn, string& strJsonOut) override
	{
		X_UNUSED(strJsonIn);
		X_UNUSED(strJsonOut);
		return -1;
	}

	// 打印
	// 
	virtual int PrintCard(string& strJsonIn, string& strJsonOut) override
	{
		X_UNUSED(strJsonIn);
		X_UNUSED(strJsonOut);
		return -1;
	}

	// 数据回盘
	// cardID,cardType,name,bankCode,operator,city,cardNum,chipNum,MagNum,ATR,identifyNum,cardVersion,chipType
	/*
	input:none
	ouput:
	{
		message:""
	}
	*/
	virtual int ReturnData(string& strJsonIn, string& strJsonOut) override
	{
		CJsonObject json(strJsonIn);
		int nResult = -1;
		int nSSResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}

			pCardInfo->strDealType = "1";	// replace card
			pCardInfo->strCardType = "A";// 仅支持身份证//json["PaperType"].ToString();
			pCardInfo->strOperator = strOperator;

			json.Get("CardID", pCardInfo->strCardID);
			json.Get("Name", pCardInfo->strName);
			json.Get("BankCode", pCardInfo->strBankCode);
			json.Get("City", pCardInfo->strCity);
			json.Get("CardNum", pCardInfo->strCardNum);
			json.Get("ChipNum", pCardInfo->strChipNum);
			json.Get("MagNum", pCardInfo->strMagNum);
			json.Get("CardATR", pCardInfo->strCardATR);
			json.Get("CardIdentity", pCardInfo->strIdentifyNum);
			json.Get("CardVersion", pCardInfo->strCardVersion);
			json.Get("ChipType", pCardInfo->strChipType);

			if (pCardInfo->strCardID.empty() ||
				pCardInfo->strName.empty() ||
				pCardInfo->strCity.empty() ||
				pCardInfo->strBankCode.empty() ||
				pCardInfo->strCardNum.empty() ||
				pCardInfo->strChipNum.empty() ||
				pCardInfo->strMagNum.empty() ||
				pCardInfo->strCardATR.empty() ||
				pCardInfo->strIdentifyNum.empty() ||
				pCardInfo->strCardVersion.empty() ||
				pCardInfo->strChipType.empty())
			{
				strMessage = "身份证,姓名,银行代码,城市代码,社保卡号,卡芯片号,磁条号,ATR,卡识别码,卡版本或芯片类型不能为空!";
				break;
			}
			//UpdatePerson();
			strKey = "saveCardCompleted";
			CJsonObject tmpJson;

			if (!GetStageStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			{
				string strFunction = "制卡完成(saveCardCompleted)";
				if (QFailed(nSSResult = saveCardCompleted(*pCardInfo, strOutInfo)))
				{
					strMessage = strFunction + "失败:";
					strMessage += strOutInfo;
					break;
				}

				if QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction))
					break;

				UpdateStage(strKey, 0, tmpJson.ToFormattedString(), false);
			}
			nErrFlag = 0;
			nResult = 0;
		} while (0);
		if (nResult)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}
		return nResult;
	}

	// 激活及后续收尾工作
	// cardID,name,cardNum,bankCode,city
	virtual int ActiveCard(string& strJsonIn, string& strJsonOut) override
	{
		CJsonObject json(strJsonIn);
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}

			pCardInfo->strDealType = "1";	// replace card
			pCardInfo->strCardType = "A";// 仅支持身份证//json["PaperType"].ToString();
			pCardInfo->strOperator = strOperator;

			json.Get("CardID", pCardInfo->strCardID);
			json.Get("Name", pCardInfo->strName);
			json.Get("BankCode", pCardInfo->strBankCode);
			json.Get("City", pCardInfo->strCity);
			json.Get("cardNum", pCardInfo->strCardNum);
			json.Get("MagNum", pCardInfo->strMagNum);

			if (pCardInfo->strCardID.empty() ||
				pCardInfo->strName.empty() ||
				pCardInfo->strCity.empty() ||
				pCardInfo->strBankCode.empty() ||
				pCardInfo->strCardNum.empty())
			{
				strMessage = "身份证,姓名,银行代码,城市代码,社保卡号,卡芯片号,磁条号,ATR,卡识别码,卡版本或芯片类型不能为空!";
				break;
			}

			strKey = "saveCardActive";
			CJsonObject tmpJson;
			int nErrFlag = -1;
			string strErrFlag;
			if (!GetStageStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			{
				string strFunction = "激活卡片号(saveCardActive)";
				if (QFailed(nSSResult = saveCardActive(*pCardInfo, strOutInfo)))
				{
					strMessage = strFunction + "失败:";
					strMessage += strOutInfo;
					break;
				}

				if QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction))
					break;

				UpdateStage(strKey, 0, tmpJson.ToFormattedString(), false);
			}

			strKey = "saveCardJrzhActive";
			string strFunction;
			if (!GetStageStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			{
				// cardID,cardType,name,magNum,bankCode,operator,city
				string strFunction = "社保卡金融账号激活(saveCardJrzhActive)";
				if (QFailed(nSSResult = saveCardJrzhActive(*pCardInfo, strOutInfo)))
				{
					strMessage = strFunction + "失败:";
					strMessage += strOutInfo;
					break;
				}
				if QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction))
					break;

				UpdateStage(strKey, 0, tmpJson.ToFormattedString(), true);
			}
			nErrFlag = 0;
			nResult = 0;
		} while (0);
		if (nResult)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}
		return nResult;
	}

	// 临时挂失/解挂
	/*
	Input
	{
	"CardID":"",
	"Name":"",
	"City":"",
	"BankCode":"",
	"PaperType":""
	"Operator":""
	}
	*/
	// 默认挂失，nOperation为1时为解挂
	virtual int RegisterLostTemporary(string& strJsonIn, string& strJsonOut, int nOperation = 0) override
	{
		CJsonObject json;
		string strOutInfo;
		string strMessage;
		int nSSResult = 0;
		int nResult = -1;
		int nErrFlag = -1;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		do
		{
			json.Parse(strJsonIn);
			CJsonObject tmpJson;
			string strFunction = "saveCardLsgs";
			pCardInfo->strOperator = strOperator;
			json.Get("CardID", pCardInfo->strCardID);
			json.Get("Name", pCardInfo->strName);
			json.Get("BankCode", pCardInfo->strBankCode);
			json.Get("City", pCardInfo->strCity);
			json.Get("cardNum", pCardInfo->strCardNum);

			pCardInfo->strOperator = strOperator;
			pCardInfo->strCardType = "A";
			pCardInfo->strOperator = strOperator;
			if (pCardInfo->strCardID.empty() ||
				pCardInfo->strName.empty() ||
				pCardInfo->strCity.empty() ||
				pCardInfo->strBankCode.empty() ||
				pCardInfo->strCardNum.empty())
			{
				strMessage = "身份证,姓名,银行代码,城市代码,社保卡号不能为空!";
				break;
			}
			strFunction = "社保卡临时挂失失败(saveCardLsgs)";
			if (nOperation == 0)
			{
				if (QFailed(nSSResult = saveCardLsgs(*pCardInfo, strOutInfo)))
				{
					strFunction = "社保卡临时挂失失败(saveCardLsgs)";
					strMessage = strFunction + "失败:";
					strMessage += strOutInfo;
					break;
				}
			}
			else if (nOperation == 1)
			{
				if (QFailed(nSSResult = saveCardLsgsjg(*pCardInfo, strOutInfo)))
				{
					strFunction = "社保卡临时挂失解挂(saveCardLsgsjg)";
					strMessage = strFunction + "失败:";
					strMessage += strOutInfo;
					break;
				}
			}
			else
			{
				nResult = -1;
				strMessage = "操作代码无效,只能是挂失(0)或解挂(1)!";
				break;
			}

			if QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction))
				break;

			nResult = 0;
			nErrFlag = 0;
		} while (0);
		if (nResult)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}
		return nResult;
	}

	// 正式挂失/解挂
	// 默认挂失，nOperation为1时为解挂
	// input:
	/*
		"CardID"
		"Name"
		"City"
		"BankCode"
		"CardNum"
		"Operator"
	*/
	virtual int RegisterLost(string& strJsonIn, string& strJsonOut, int nOperation = 0) override
	{
		X_UNUSED(nOperation);
		CJsonObject json(strJsonIn);
		string strOutInfo;
		string strMessage;
		int nSSResult = 0;
		int nResult = -1;
		int nErrFlag = -1;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}
			// cardID,cardType,name,cardNum,city,cardNum,operator,bankcode
			json.Get("CardID", pCardInfo->strCardID);
			json.Get("Name", pCardInfo->strName);
			json.Get("City", pCardInfo->strCity);
			json.Get("BankCode", pCardInfo->strBankCode);
			json.Get("CardNum", pCardInfo->strCardNum);
			pCardInfo->strOperator = strOperator;
			pCardInfo->strCardType = "A";
			pCardInfo->strOperator = strOperator;

			if (pCardInfo->strCardID.empty() ||
				pCardInfo->strName.empty() ||
				pCardInfo->strCity.empty() ||
				pCardInfo->strBankCode.empty() ||
				pCardInfo->strCardNum.empty())
			{
				strMessage = "身份证,姓名,银行代码,城市代码,社保卡号不能为空!";
				break;
			}

			UpdatePerson();
			strKey = "saveCardGs";
			string strFunction;
			if (!GetStageStatus(strKey, nResult, strDatagram) || QFailed(nResult))
			{
				if (QFailed(nSSResult = saveCardGs(*pCardInfo, strOutInfo)))
				{
					strFunction = "社保卡挂失(saveCardGs)";
					strMessage = strFunction + "失败:";
					strMessage += strOutInfo;
					break;
				}
				CJsonObject tmpJson;
				if (QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction)))
					break;

				UpdateStage(strKey, 0, tmpJson.ToFormattedString());
			}
			nResult = 0;
			nErrFlag = 0;
		} while (0);
		if (nResult)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}
		return nResult;
	}

	// 
	virtual int QueryPayResult(string& strJsonIn, string& strJsonOut) override
	{
		X_UNUSED(strJsonIn);
		CJsonObject jsonOut;
		jsonOut.Add("Result", (int)PayResult::Pay_Unsupport);
		jsonOut.Add("Message", "无须支付或没有支付流程!");
		strJsonOut = jsonOut.ToString();
		return 0;
	}

	virtual int SetExtraInterface(const string& strCommand, string& strJsonIn, string& strJsonOut) override
	{
		if (!strCommand.size())
		{
			strJsonOut = { "\"Result\":1,\
							\"Message\":\"Invalid command!\"" };
			return -1;
		}
		else if (strCommand == "QueryPersonInfo")
		{
			return  QueryPersionOnfo(strJsonIn, strJsonOut);
		}
		else if (strCommand == "QueryPersonPhoto")
		{
			return  QueryPersonPhoto(strJsonIn, strJsonOut);
		}
		else if (strCommand == "ModifyPersonInfo")
		{
			return  ModifyPersonInfo(strJsonIn, strJsonOut);
		}

		return -1;
	}

	/*
	input: CardID,Name,City
	Output:CardID,CardNum,Name,Gender,Birthday,Nation,Address,Mobile,OccupType
	*/
	int QueryPersionOnfo(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		//int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode = "0";
		string strKey = "";
		string strDatagram = "";
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			jsonIn.Get("CardID", pCardInfo->strCardID);
			jsonIn.Get("Name", pCardInfo->strName);
			jsonIn.Get("City", pCardInfo->strCity);
			pCardInfo->strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (pCardInfo->strCardID.empty() ||
				pCardInfo->strName.empty() ||
				pCardInfo->strCity.empty())
			{
				strMessage = "身份证,姓名,城市代码!";
				break;
			}
			string strFunction = "查询人员信息(queryPersonInfo)";
			if (QFailed(queryPersonInfo(*pCardInfo, strJsonOut)))
			{
				strMessage = strFunction + "失败";
				strMessage += strJsonOut;
				break;
			}
			CJsonObject tmpJson;
			if (QFailed(CheckOutJson(strJsonOut, tmpJson, strMessage, strErrcode, nErrFlag, strFunction)))
				break;

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryPersonInfo is invalid !";
				break;
			}

			ds.Get("yxzjhm", pCardInfo->strCardID);		//有效证件号码			
			ds.Get("shbzhm", pCardInfo->strCardNum);	//社会保障号码			
			ds.Get("xm", pCardInfo->strName);			//姓名
			ds.Get("xb", pCardInfo->strSex);			//性别
			ds.Get("csrq", pCardInfo->strBirthday);		//出生日期
			ds.Get("mz", pCardInfo->strNation);			//民族
			ds.Get("txdz", pCardInfo->strAdress);		//通讯地址
			ds.Get("sjhm", pCardInfo->strMobile);		//手机号码
			ds.Get("zylb", pCardInfo->strOccupType);	//职业类别
			ds.Get("zjqsrq", pCardInfo->strReleaseDate);
			ds.Get("zjdqrq", pCardInfo->strValidDate);

			CJsonObject jsonOut;
			//jsonOut.Add("CardID", CardInfo.strCardID);			
			//jsonOut.Add("Name", CardInfo.strName);
			jsonOut.Add("Gender", pCardInfo->strSex);
			jsonOut.Add("Birthday", pCardInfo->strBirthday);
			jsonOut.Add("Nation", pCardInfo->strNation);
			jsonOut.Add("Address", pCardInfo->strAdress);
			jsonOut.Add("Mobile", pCardInfo->strMobile);
			jsonOut.Add("CardNum", pCardInfo->strCardNum);
			jsonOut.Add("Occupation", pCardInfo->strOccupType);
			jsonOut.Add("PaperIssuedDate", pCardInfo->strReleaseDate);
			jsonOut.Add("PaperExpireDate", pCardInfo->strValidDate);
			strJsonOut = jsonOut.ToString();
			nErrFlag = 0;
			nResult = 0;
		} while (0);
		if (nResult)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}
		return nResult;
	}
	/*
	input: CardID,Name,City
	Output:CardID,CardNum,Name,Gender,Birthday,Nation,Address,Mobile,OccupType
	*/
	int QueryPersonPhoto(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		//int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strErrcode;
		string strKey = "";
		string strDatagram = "";
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			jsonIn.Get("CardID", pCardInfo->strCardID);
			jsonIn.Get("Name", pCardInfo->strName);
			jsonIn.Get("City", pCardInfo->strCity);
			pCardInfo->strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (pCardInfo->strCardID.empty() ||
				pCardInfo->strName.empty() ||
				pCardInfo->strCity.empty())
			{
				strMessage = "身份证,姓名,城市代码不能为空!";
				break;
			}
			string strFunction = "查询人员照片(queryPerPhoto)";
			if (QFailed(queryPerPhoto(*pCardInfo, strOutInfo)))
			{
				strMessage = strFunction + "失败:";
				strMessage += strOutInfo;
				break;
			}

			CJsonObject tmpJson;
			if (QFailed(CheckOutJson(strOutInfo, tmpJson, strMessage, strErrcode, nErrFlag, strFunction)))
				break;

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryPerPhoto is invalid !";
				break;
			}

			ds.Get("photostr", pCardInfo->strPhoto);
			CJsonObject jsonOut;
			jsonOut.Add("Photo", pCardInfo->strPhoto);
			strJsonOut = jsonOut.ToString();
			nErrFlag = 0;
			nResult = 0;
		} while (0);
		if (nResult)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			jsonOut.Add("errcode", strErrcode);
			strJsonOut = jsonOut.ToString();
		}
		return nResult;
	}

	int ModifyPersonInfo(string& strJsonIn, string& strJsonOut)
	{
		X_UNUSED(strJsonIn);
		X_UNUSED(strJsonOut);
		return -1;
	}

	// CardID,Name,City,CardNum,SignatureKey
	virtual int GetCA(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		int nResult = -1;
		int nErrFlag = -1;
		string strMessage;
		string strOutInfo;
		string strSignatureKey;
		string strAlgorithm = "SM2";
		string strName, strCardID, strCardNum, strCity;
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			jsonIn.Get("CardID", strCardID);
			jsonIn.Get("Name", strName);
			jsonIn.Get("City", strCity);
			jsonIn.Get("CardNum", strCardNum);
			jsonIn.Get("SignatureKey", strSignatureKey);

			if (strCardID.empty() ||
				strName.empty() ||
				strCity.empty() ||
				strCardNum.empty())
			{
				strMessage = "身份证,姓名,城市代码或社保卡号不能为空!";
				break;
			}

			if (QFailed(getCA(strName, strCardID, strCardNum, strSignatureKey, strAlgorithm, strCity, strOutInfo)))
			{
				strMessage = "Failed in getCA!";
				break;
			}
			CJsonObject tmpJson;
			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "Output of getCA is invalid!";
				break;
			}

			string strErrFlag;
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of getCA!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of getCA!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of getCA is invalid !";
				break;
			}

			string strCaOut = ds.ToString();

			if (!strCaOut.size())
			{
				strMessage = "CA out is empty!";
				break;
			}

			strJsonOut = strCaOut;
			pCardInfo->strCardNum = strCardNum;
			pCardInfo->strCardID = strCardID;
			pCardInfo->strName = strName;
			pCardInfo->strCity = strCity;
			nErrFlag = 0;
			nResult = 0;
		} while (0);
		if (nResult)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			strJsonOut = jsonOut.ToString();
		}
		return nResult;
	}
};
