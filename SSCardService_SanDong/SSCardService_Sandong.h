#pragma once
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include "../SSCardService/SSCardService.h"
#include "../SDK/SSCardInfo_Sandong/SD_SSCardInfo.h"
#include "Utility.h"
#include "CJsonObject.hpp"
#include "Markup.h"
#include <QtCore/qglobal.h>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <map>

//宏定义
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)")"
#define Warning(strMessage) message( __LOC__ " Warning: " strMessage)

extern map<string, string>g_mapNationnaltyCode;
#define  QFailed(x)		(x!= 0)

using namespace std;
using namespace neb;
class SSCardService_Sandong :
	public SSCardService
{
private:
	SD_SSCardInfo CardInfo;
public:

	SSCardService_Sandong()
	{
	}

	//SSCardService_Sandong(/*ServiceType nSvrType*/)	/*	:SSCardService(nSvrType)*/
	//{
	//}

	virtual int SetSerivdeType(ServiceType nSvrType) override
	{

		if (nSvrType < ServiceType::Service_NewCard ||
			nSvrType > ServiceType::Service_RegisterLost)
			return -1;
		else
		{
			nServiceType = nSvrType;
			return 0;
		}
	}

	virtual int Initialize(string& strInitJson, string& szOutInfo) override
	{
		//Warning("需要确定initCardInfo输入数据的Json定义!");
		return initCardInfo(strInitJson.c_str(), szOutInfo);
	}
	~SSCardService_Sandong()
	{

	}

	virtual int SaveCardData(string& strINIFile) override
	{
#define AddCardFiled(x,s)	x.setValue(#s,CardInfo.s.c_str());

		QSettings CardIni(strINIFile.c_str(), QSettings::IniFormat);
		CardIni.beginGroup("CardData");

		AddCardFiled(CardIni, strCardNum);			//社保卡号
		AddCardFiled(CardIni, strCardID);			//身份证号
		AddCardFiled(CardIni, strBirthday);			//出生日期
		AddCardFiled(CardIni, strName);				//姓名
		AddCardFiled(CardIni, strSex);				//性别
		AddCardFiled(CardIni, strNation);			//民族
		AddCardFiled(CardIni, strMobile);			//手机号
		AddCardFiled(CardIni, strAdress);			//通讯地址
		AddCardFiled(CardIni, strPostalCode);		//邮政编码
		AddCardFiled(CardIni, strEmail);			//电子邮箱
		AddCardFiled(CardIni, strGuardianName);		//监护人姓名
		AddCardFiled(CardIni, strCity);				//所属城市
		AddCardFiled(CardIni, strSSQX);				//所属区县
		AddCardFiled(CardIni, strBankCode);			//银行代码
		AddCardFiled(CardIni, strReleaseDate);		//发卡日期
		AddCardFiled(CardIni, strValidDate);		//有效日期
		AddCardFiled(CardIni, strIdentifyNum);		//卡识别码
		AddCardFiled(CardIni, strCardATR);			//卡复位信息
		AddCardFiled(CardIni, strBankNum);			//银行卡号
		AddCardFiled(CardIni, strCardType);			//证件类型
		AddCardFiled(CardIni, strOperator);			//经办人
		AddCardFiled(CardIni, strOccupType);		//职业类别
		AddCardFiled(CardIni, strDealType);			//办卡类型
		AddCardFiled(CardIni, strChipNum);			//卡芯片账号
		AddCardFiled(CardIni, strMagNum);			//卡磁条账号
		AddCardFiled(CardIni, strCardVersion);		//卡规范版本
		AddCardFiled(CardIni, strChipType);			//芯片类型
		AddCardFiled(CardIni, strCardUnit);			//发证机关
		AddCardFiled(CardIni, strBirthPlace);		//出生地
		AddCardFiled(CardIni, strPerAddr);			//户籍所在地
		AddCardFiled(CardIni, strRegAttr);			//户口性质
		AddCardFiled(CardIni, strEducation);		//文化程度
		AddCardFiled(CardIni, strMarital);			//婚姻状况
		AddCardFiled(CardIni, strContactsName);		//联系人姓名
		AddCardFiled(CardIni, strContactsMobile);	//联系人电话
		AddCardFiled(CardIni, strReason);			//补换原因
		AddCardFiled(CardIni, strPhoto);			//相片

		CardIni.endGroup();
		return 0;
	}

	virtual int LoadCardData(string& strINIFile) override
	{
		QFileInfo fi(strINIFile.c_str());
		if (!fi.isFile())
		{
			return -1;
		}
#define GetCardField(x,c,f)		c.f =  x.value(#f).toString().toStdString();
		QSettings CardIni(strINIFile.c_str(), QSettings::IniFormat);
		CardInfo.strCardNum = "";
		CardIni.beginGroup("CardData");

		GetCardField(CardIni, CardInfo, strCardNum);		//社保卡号
		GetCardField(CardIni, CardInfo, strCardID);			//身份证号
		GetCardField(CardIni, CardInfo, strBirthday);		//出生日期
		GetCardField(CardIni, CardInfo, strName);			//姓名
		GetCardField(CardIni, CardInfo, strSex);			//性别
		GetCardField(CardIni, CardInfo, strNation);			//民族
		GetCardField(CardIni, CardInfo, strMobile);			//手机号
		GetCardField(CardIni, CardInfo, strAdress);			//通讯地址
		GetCardField(CardIni, CardInfo, strPostalCode);		//邮政编码
		GetCardField(CardIni, CardInfo, strEmail);			//电子邮箱
		GetCardField(CardIni, CardInfo, strGuardianName);	//监护人姓名
		GetCardField(CardIni, CardInfo, strCity);			//所属城市
		GetCardField(CardIni, CardInfo, strSSQX);			//所属区县
		GetCardField(CardIni, CardInfo, strBankCode);		//银行代码
		GetCardField(CardIni, CardInfo, strReleaseDate);	//发卡日期
		GetCardField(CardIni, CardInfo, strValidDate);		//有效日期
		GetCardField(CardIni, CardInfo, strIdentifyNum);	//卡识别码
		GetCardField(CardIni, CardInfo, strCardATR);		//卡复位信息
		GetCardField(CardIni, CardInfo, strBankNum);		//银行卡号
		GetCardField(CardIni, CardInfo, strCardType);		//证件类型
		GetCardField(CardIni, CardInfo, strOperator);		//经办人
		GetCardField(CardIni, CardInfo, strOccupType);		//职业类别
		GetCardField(CardIni, CardInfo, strDealType);		//办卡类型
		GetCardField(CardIni, CardInfo, strChipNum);		//卡芯片账号
		GetCardField(CardIni, CardInfo, strMagNum);			//卡磁条账号
		GetCardField(CardIni, CardInfo, strCardVersion);	//卡规范版本
		GetCardField(CardIni, CardInfo, strChipType);		//芯片类型
		GetCardField(CardIni, CardInfo, strCardUnit);		//发证机关
		GetCardField(CardIni, CardInfo, strBirthPlace);		//出生地
		GetCardField(CardIni, CardInfo, strPerAddr);		//户籍所在地
		GetCardField(CardIni, CardInfo, strRegAttr);		//户口性质
		GetCardField(CardIni, CardInfo, strEducation);		//文化程度
		GetCardField(CardIni, CardInfo, strMarital);		//婚姻状况
		GetCardField(CardIni, CardInfo, strContactsName);	//联系人姓名
		GetCardField(CardIni, CardInfo, strContactsMobile);	//联系人电话
		GetCardField(CardIni, CardInfo, strReason);			//补换原因
		//GetCardField(CardIni, CardInfo,strPhoto);			//相片
		QByteArray ba = CardIni.value("strPhoto").toString().toLatin1();
		qDebug() << "Load photo,size = " << ba.size();
		CardInfo.strPhoto = string(ba.data(), ba.size());
		CardIni.endGroup();
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
		SD_SSCardInfo CardInfo;
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 1;
		string strMessage;
		string strOutInfo;
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

			jsonIn.Get("CardID", CardInfo.strCardID);
			jsonIn.Get("Name", CardInfo.strName);
			jsonIn.Get("City", CardInfo.strCity);
			jsonIn.Get("BankCode", CardInfo.strBankCode);
			CardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (CardInfo.strCardID.empty()
				|| CardInfo.strName.empty()
				|| CardInfo.strCity.empty()
				|| CardInfo.strBankCode.empty()
				//||CardInfo.strCardType.empty()
				)
			{
				strMessage = "身份证,姓名,城市代码或银行代码不能为空!";
				break;
			}

			if (QFailed(nSSResult = chkCanCardSq(CardInfo, strOutInfo)))
			{
				strMessage = "Faled in chkCanCardSq:";
				strMessage += std::to_string(nSSResult);
				break;
			}

			CJsonObject OutJson(strOutInfo);
			if (!OutJson.KeyExist("errflag") ||
				!OutJson.KeyExist("errtext"))
			{
				strMessage = "can't locate errflag or errtext!";
				break;
			}
			string strErrFlag;
			OutJson.Get("errflag", strErrFlag);
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				int nErrCode = -1;
				if (!OutJson.Get("errcode", nErrCode))
				{
					strMessage = "can't locate errcode!";
					break;
				}
				ProcessErrcode(nErrCode, strMessage);
				break;
			}

			strMessage = "Succeed";
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
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			jsonIn.Get("CardID", CardInfo.strCardID);
			jsonIn.Get("Name", CardInfo.strName);
			jsonIn.Get("City", CardInfo.strCity);
			jsonIn.Get("BankCode", CardInfo.strBankCode);
			CardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (CardInfo.strCardID.empty()
				|| CardInfo.strName.empty()
				|| CardInfo.strCity.empty()
				|| CardInfo.strBankCode.empty()
				|| CardInfo.strCardType.empty()
				)
			{
				strMessage = "身份证,姓名,城市代码或银行代码不能为空!";
				break;
			}

			if (QFailed(nSSResult = chkCanCardBh(CardInfo, strOutInfo)))
			{
				strMessage = "Faled in chkCanCardSq:";
				strMessage += std::to_string(nSSResult);
				break;
			}

			CJsonObject outJson(strOutInfo);
			string strErrFlag;

			if (!outJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field errflag from output of chkCanCardBh!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!outJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' from output of chkCanCardBh!";
				else
				{
					outJson.Get("errtext", strMessage);
				}
				break;
			}

			strMessage = "Succeed";
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
	int QueryCardInfo(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			jsonIn.Get("CardID", CardInfo.strCardID);
			jsonIn.Get("Name", CardInfo.strName);
			jsonIn.Get("City", CardInfo.strCity);
			jsonIn.Get("BankCode", CardInfo.strBankCode);
			CardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (QFailed(nSSResult = queryCardInfoBySfzhm(CardInfo, strOutInfo)))
			{
				strMessage = "Failed in queryCardInfoBySfzhm:";
				strMessage += std::to_string(nSSResult);
				break;
			}
			CJsonObject outJson(strOutInfo);
			if (!outJson.Parse(strOutInfo))
			{
				strMessage = "queryCardInfoBySfzhm output invalid:";
				strMessage += strOutInfo;
				break;
			}
			string strErrFlag;
			if (!outJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' from output of queryCardInfoBySfzhm!";
				break;
			}
			int nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!outJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' from output of queryCardInfoBySfzhm!";
				else
				{
					outJson.Get("errtext", strMessage);
				}
				break;
			}
			CJsonObject ds = outJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardInfoBySfzhm is invalid !";
				break;
			}

			ds.Get("shbzhm", CardInfo.strCardID);
			ds.Get("kh", CardInfo.strCardNum);
			ds.Get("xm", CardInfo.strName);
			ds.Get("kxpzh", CardInfo.strChipNum);
			ds.Get("kctzh", CardInfo.strMagNum);
			ds.Get("yhbh", CardInfo.strBankCode);		// 银行编号
			ds.Get("sjhm", CardInfo.strMobile);
			ds.Get("yhbh", CardInfo.strBankCode);
			ds.Get("sjhm", CardInfo.strMobile);
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
			outJson.Add("Name", CardInfo.strName);
			outJson.Add("CardID", CardInfo.strCardID);
			outJson.Add("CardNum", CardInfo.strCardNum);
			outJson.Add("BankName", strBankName);
			outJson.Add("BankCode", CardInfo.strBankCode);
			outJson.Add("CardStatus", (int)nCardStatus);
			outJson.Add("Mobile", CardInfo.strMobile);
			strJsonOut = outJson.ToString();
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
		default:
		{
			CJsonObject jsonOut;
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
	int CommitNewCardInfo(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
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

			CardInfo.strDealType = "0";	// new card
			CardInfo.strOperator = "Administrator";
			CardInfo.strCardType = "A";
			jsonIn.Get("IssueDate", CardInfo.strReleaseDate);
			jsonIn.Get("ExpireDate", CardInfo.strValidDate);
			jsonIn.Get("Mobile", CardInfo.strMobile);
			jsonIn.Get("Operator", CardInfo.strOperator);
			//jsonIn.Get("OccupType", CardInfo.strOccupType);
			jsonIn.Get("Birthday", CardInfo.strBirthday);
			jsonIn.Get("Gender", CardInfo.strSex);
			jsonIn.Get("Nation", CardInfo.strNation);
			jsonIn.Get("Address", CardInfo.strAdress);
			jsonIn.Get("Photo", CardInfo.strPhoto);


			if (CardInfo.strCardID.empty()
				|| CardInfo.strName.empty()
				|| CardInfo.strCity.empty()
				|| CardInfo.strBankCode.empty()
				|| CardInfo.strMobile.empty()
				|| CardInfo.strReleaseDate.empty()
				|| CardInfo.strValidDate.empty()
				|| CardInfo.strBirthday.empty()
				|| CardInfo.strSex.empty()
				|| CardInfo.strNation.empty()
				|| CardInfo.strAdress.empty()
				|| CardInfo.strPhoto.empty())
			{
				strMessage = "身份证,姓名,城市代码,银行代码,手机,证件有效,生日,性别,民族,住址或照片不能为空!";
				break;
			}

			if (QFailed(nSSResult = saveCardSqList(CardInfo, strOutInfo)))
			{
				strMessage = "Failed in saveCardSqList:";
				strMessage += strOutInfo;
				break;
			}

			CJsonObject tmpJson;
			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "queryCardInfoBySfzhm output is invalid:";
				strMessage += strOutInfo;
			}
			string strErrFlag;

			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' from output of saveCardSqList!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' from output of saveCardSqList!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}
			auto tNow = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			auto tNext = chrono::system_clock::to_time_t(std::chrono::system_clock::now() + std::chrono::days(1));
			stringstream date1, date2;

			// date1 = "2022-01-26 09:51:00"
			// date1 << std::put_time(std::localtime(&tNow), "%Y-%m-%d %X");
			date1 << std::put_time(std::localtime(&tNow), "%Y%m%d");
			// 20210126
			date2 << std::put_time(std::localtime(&tNext), "%Y%m%d");

			CardInfo.strReleaseDate = date1.str();
			CardInfo.strValidDate = date2.str();

			// city,bankcode,validDate,dealType,releaseDate
			if (QFailed(nSSResult = queryCardZksqList(CardInfo, strOutInfo)))
			{
				strMessage = "Faled in queryCardInfoBySfzhm:";
				strMessage += strOutInfo;
				break;
			}
			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "queryCardZksqList output is invalid:";
				strMessage += strOutInfo;
				break;
			}
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryCardZksqList!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryCardZksqList!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}

			if (!tmpJson.KeyExist("xm") ||
				!tmpJson.KeyExist("shbzhm"))
			{
				strMessage = "can't locate field 'xm' or 'shbzhm' from output of queryCardZksqList!";
				break;
			}
			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardZksqList is invalid !";
				break;
			}

			CardInfo.strCardNum = ds["shbzhm"].ToString();
			CardInfo.strMobile = ds["sjhm"].ToString();

			CJsonObject outJson;
			outJson.Add("Name", CardInfo.strName);
			outJson.Add("CardID", CardInfo.strCardID);
			outJson.Add("CardNum", CardInfo.strCardNum);
			outJson.Add("Mobile", CardInfo.strMobile);
			outJson.Add("BankCode", tmpJson["yhbh"].ToString());
			strJsonOut = outJson.ToString();

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

	// cardID,cardType,name,bankCode,city,mobile,reason,operator,OccupType,birthday,sex,nation,address,photo
	int CommitReplaceCardInfo(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo CardInfo;
		CJsonObject json(strJsonIn);
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}
			string strGender, strNationality, strPhoto;
			CardInfo.strDealType = "1";	// replace card
			CardInfo.strCardType = "A";// 仅支持身份证//json["PaperType"].ToString();
			json.Get("CardID", CardInfo.strCardID);
			json.Get("Name", CardInfo.strName);
			json.Get("BankCode", CardInfo.strBankCode);
			json.Get("City", CardInfo.strCity);
			json.Get("Mobile", CardInfo.strMobile);
			CardInfo.strReason = "Replace Card!";
			CardInfo.strOperator = "Administrator";
			//json.Get("Reason", CardInfo.strReason);
			//json.Get("Operator", CardInfo.strOperator);
			//json.Get("OccupType", CardInfo.strOccupType);
			json.Get("IssueDate", CardInfo.strReleaseDate);
			json.Get("ExpireDate", CardInfo.strValidDate);
			json.Get("Birthday", CardInfo.strBirthday);
			json.Get("Gender", strGender);
			json.Get("Nation", strNationality);
			json.Get("Address", CardInfo.strAdress);
			json.Get("CardNum", CardInfo.strCardNum);

			if (CardInfo.strCardID.empty() ||
				CardInfo.strName.empty() ||
				CardInfo.strBankCode.empty() ||
				CardInfo.strCity.empty() ||
				CardInfo.strMobile.empty() ||
				CardInfo.strReason.empty() ||
				CardInfo.strOperator.empty() ||
				strGender.empty() ||
				//CardInfo.strOccupType.empty() ||
				CardInfo.strCardType.empty() ||
				CardInfo.strReleaseDate.empty() ||
				CardInfo.strValidDate.empty() ||
				CardInfo.strBirthday.empty() ||
				strNationality.empty() ||
				CardInfo.strAdress.empty())
			{
				strMessage = "身份号码,姓名,银行代码,城市,手机号码,出生日期,性别,民族或住址不能为空!";
				break;
			}
			if (strGender == "男")
				CardInfo.strSex = "1";
			else if (strGender == "女")
				CardInfo.strSex = "2";
			else
				CardInfo.strSex = "9";

			auto itFind = g_mapNationnaltyCode.find(strNationality);
			if (itFind != g_mapNationnaltyCode.end())
			{
				CardInfo.strNation = itFind->second;
			}
			else
			{
				strMessage = "民族无效!";
				break;
			}

			if (QFailed(nSSResult = saveCardBhList(CardInfo, strOutInfo)))
			{
				strMessage = "Failed in saveCardSqList:";
				strMessage += strOutInfo;
				break;
			}

			CJsonObject tmpJson;
			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "saveCardBhList output is invalid:";
				strMessage += strOutInfo;
				break;
			}
			string strErrFlag;
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryCardZksqList!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryCardZksqList!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}
			auto tNow = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			auto tNext = chrono::system_clock::to_time_t(std::chrono::system_clock::now() + std::chrono::days(1));
			stringstream date1, date2;

			// date1 = "2022-01-26 09:51:00"
			// date1 << std::put_time(std::localtime(&tNow), "%Y-%m-%d %X");
			date1 << std::put_time(std::localtime(&tNow), "%Y%m%d");
			// 20210126
			date2 << std::put_time(std::localtime(&tNext), "%Y%m%d");

			CardInfo.strReleaseDate = date1.str();
			CardInfo.strValidDate = date2.str();

			// city,bankcode,validDate,dealType,releaseDate

			if (QFailed(nSSResult = queryCardZksqList(CardInfo, strOutInfo)))
			{
				strMessage = "Faled in queryCardInfoBySfzhm:";
				strMessage += strOutInfo;
				break;
			}
			try
			{
				string strOutputFile = "./Debug/Carddata_" + CardInfo.strCardID + ".json";
				fstream fs(strOutputFile, ios::out);
				fs << strOutInfo;
				fs.close();
			}
			catch (std::exception& e)
			{
			}

			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "queryCardZksqList output is invalid:";
				strMessage += strOutInfo;
			}

			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryCardZksqList!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryCardZksqList!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardZksqList is invalid !";
				break;
			}

			if (!ds.KeyExist("xm") ||
				!ds.KeyExist("shbzhm"))
			{
				strMessage = "can't locate field 'xm' or 'shbzhm' from output of queryCardZksqList!";
				break;
			}

			ds.Get("shbzhm", CardInfo.strCardNum);
			ds.Get("sjhm", CardInfo.strMobile);

			// cardID,cardType,name,bankCode,city,cardNum,operator
			if (QFailed(nSSResult = saveCardBhk(CardInfo, strOutInfo)))
			{
				strMessage = "Faled in saveCardBhk:";
				strMessage += strOutInfo;
				break;
			}

			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "saveCardBhk output is invalid:";
				strMessage += strOutInfo;
			}

			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of saveCardBhk!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of saveCardBhk!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}

			nResult = 0;

		} while (0);
		if (nErrFlag)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
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
		SD_SSCardInfo CardInfo;
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
		CJsonObject json(strJsonIn);
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}
			string strGender, strNationality, strPhoto;
			CardInfo.strDealType = "1";	// replace card
			CardInfo.strCardType = "A";// 仅支持身份证//json["PaperType"].ToString();
			CardInfo.strOperator = "Administrator";
			json.Get("CardID", CardInfo.strCardID);
			json.Get("Name", CardInfo.strName);
			json.Get("BankCode", CardInfo.strBankCode);
			json.Get("City", CardInfo.strCity);

			if (CardInfo.strCardID.empty() ||
				CardInfo.strName.empty() ||
				CardInfo.strBankCode.empty() ||
				CardInfo.strCity.empty())
			{
				strMessage = "身份号码,姓名,银行代码或城市代码中存在空字段!";
				break;
			}

			// cardID,cardType,name,bankCode,operator,city
			if (QFailed(nSSResult = saveCardOpen(CardInfo, strOutInfo)))
			{
				strMessage = "Failed in saveCardSqList:";
				strMessage += strOutInfo;
				break;
			}

			CJsonObject tmpJson;
			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "saveCardOpen output is invalid:";
				strMessage += strOutInfo;
				break;
			}

			string strErrFlag;
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryCardZksqList!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryCardZksqList!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}
			try
			{
				string strOutputFile = "./Debug/Carddata_" + CardInfo.strCardID + ".json";
				fstream fs(strOutputFile, ios::out);
				fs << strOutInfo;
				fs.close();
			}
			catch (std::exception& e)
			{
				string strException = e.what();
			}

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardZksqList is invalid !";
				break;
			}
			ds.Get("sbkh", CardInfo.strCardNum);
			ds.Get("zp", CardInfo.strPhoto);

			CJsonObject outJson;
			outJson.Add("CardNum", CardInfo.strCardNum);
			outJson.Add("Photo", CardInfo.strPhoto);
			strJsonOut = outJson.ToString();
			nResult = 0;

		} while (0);
		if (nErrFlag)
		{
			CJsonObject jsonOut;
			jsonOut.Add("Result", nErrFlag);
			jsonOut.Add("Message", strMessage);
			strJsonOut = jsonOut.ToString();
		}

		return nResult;
	}

	// 读卡
	virtual int ReadCard(string& strJsonIn, string& strJsonOut) override
	{
		return 0;
	}

	// 写卡
	virtual int WriteCard(string& strJsonIn, string& strJsonOut) override
	{
		return -1;
	}

	// 打印
	// 
	virtual int PrintCard(string& strJsonIn, string& strJsonOut) override
	{
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
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}

			CardInfo.strDealType = "1";	// replace card
			CardInfo.strCardType = "A";// 仅支持身份证//json["PaperType"].ToString();
			CardInfo.strOperator = "Administrator";

			json.Get("CardID", CardInfo.strCardID);
			json.Get("Name", CardInfo.strName);
			json.Get("BankCode", CardInfo.strBankCode);
			json.Get("City", CardInfo.strCity);
			json.Get("CardNum", CardInfo.strCardNum);
			json.Get("ChipNum", CardInfo.strChipNum);
			json.Get("MagNum", CardInfo.strMagNum);
			json.Get("CardATR", CardInfo.strCardATR);
			json.Get("CardIdentity", CardInfo.strIdentifyNum);
			json.Get("CardVersion", CardInfo.strCardVersion);
			json.Get("ChipType", CardInfo.strChipType);


			if (CardInfo.strCardID.empty() ||
				CardInfo.strName.empty() ||
				CardInfo.strCity.empty() ||
				CardInfo.strBankCode.empty() ||
				CardInfo.strCardNum.empty() ||
				CardInfo.strChipNum.empty() ||
				CardInfo.strMagNum.empty() ||
				CardInfo.strCardATR.empty() ||
				CardInfo.strIdentifyNum.empty() ||
				CardInfo.strCardVersion.empty() ||
				CardInfo.strChipType.empty())
			{
				strMessage = "身份证,姓名,银行代码,城市代码,社保卡号,卡芯片号,磁条号,ATR,卡识别码,卡版本或芯片类型不能为空!";
				break;
			}
			if (QFailed(nSSResult = saveCardCompleted(CardInfo, strOutInfo)))
			{
				strMessage = "Failed in saveCardCompleted:";
				strMessage += strOutInfo;
				break;
			}
			CJsonObject tmpJson;
			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "saveCardCompleted output is invalid:";
				strMessage += strOutInfo;
				break;
			}
			string strErrFlag;
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of saveCardCompleted!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of saveCardCompleted!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}

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

	// 激活及后续收尾工作
	// cardID,name,cardNum,bankCode,city
	virtual int ActiveCard(string& strJsonIn, string& strJsonOut) override
	{
		SD_SSCardInfo CardInfo;
		CJsonObject json(strJsonIn);
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}

			CardInfo.strDealType = "1";	// replace card
			CardInfo.strCardType = "A";// 仅支持身份证//json["PaperType"].ToString();
			CardInfo.strOperator = "Administrator";

			json.Get("CardID", CardInfo.strCardID);
			json.Get("Name", CardInfo.strName);
			json.Get("BankCode", CardInfo.strBankCode);
			json.Get("City", CardInfo.strCity);
			json.Get("cardNum", CardInfo.strCardNum);
			json.Get("MagNum", CardInfo.strMagNum);

			if (CardInfo.strCardID.empty() ||
				CardInfo.strName.empty() ||
				CardInfo.strCity.empty() ||
				CardInfo.strBankCode.empty() ||
				CardInfo.strCardNum.empty())
			{
				strMessage = "身份证,姓名,银行代码,城市代码,社保卡号,卡芯片号,磁条号,ATR,卡识别码,卡版本或芯片类型不能为空!";
				break;
			}

			if (QFailed(nSSResult = saveCardActive(CardInfo, strOutInfo)))
			{
				strMessage = "Failed in saveCardCompleted:";
				strMessage += strOutInfo;
				break;
			}

			CJsonObject tmpJson;
			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "saveCardActive output is invalid:";
				strMessage += strOutInfo;
				break;
			}
			int nErrFlag;
			string strErrFlag;
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of saveCardActive!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of saveCardActive!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}

			// cardID,cardType,name,magNum,bankCode,operator,city
			if (QFailed(nSSResult = saveCardJrzhActive(CardInfo, strOutInfo)))
			{
				strMessage = "Failed in saveCardJrzhActive:";
				strMessage += strOutInfo;
				break;
			}
			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "saveCardJrzhActive output is invalid:";
				strMessage += strOutInfo;
				break;
			}

			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of saveCardJrzhActive!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of saveCardJrzhActive!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}

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
		SD_SSCardInfo CardInfo;
		CJsonObject json;
		string strOutInfo;
		string strMessage;
		int nSSResult = 0;
		int nResult = -1;
		int nErrFlag = 0;
		do
		{
			if (QFailed(nSSResult = QueryCardStatus(strJsonIn, strJsonOut)))
			{
				break;
			}
			json.Parse(strJsonIn);
			CJsonObject tmpJson;
			string strFunction = "saveCardLsgs";
			json.Get("Operator", CardInfo.strOperator);
			CardInfo.strCardType = "A";
			CardInfo.strOperator = "Administrator";
			if (CardInfo.strCardID.empty() ||
				CardInfo.strName.empty() ||
				CardInfo.strCity.empty() ||
				CardInfo.strBankCode.empty() ||
				CardInfo.strCardNum.empty())
			{
				strMessage = "身份证,姓名,银行代码,城市代码,社保卡号不能为空!";
				break;
			}
			if (nOperation == 0)
			{
				if (QFailed(nSSResult = saveCardLsgs(CardInfo, strOutInfo)))
				{
					strMessage = "Failed saveCardLsgs:";
					strMessage += strOutInfo;
					break;
				}
			}
			else if (nOperation == 1)
			{
				string strFunction = "saveCardLsgsjg";
				if (QFailed(nSSResult = saveCardLsgsjg(CardInfo, strOutInfo)))
				{
					strMessage = "Failed saveCardLsgsjg:";
					strMessage += strOutInfo;
					break;
				}
			}
			else
			{
				nResult = -1;
				strMessage = "Invalid Operation type,should be 0 or 1!";
				break;
			}

			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "saveCardJrzhActive output is invalid:";
				strMessage += strOutInfo;
				break;
			}
			string strErrFlag;

			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of ";
				strMessage += strFunction;
				strMessage += "!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
				{
					strMessage = "can't locate field 'errtext' in output of ";
					strMessage += strFunction;
					strMessage += "!";
				}
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}
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
		SD_SSCardInfo CardInfo;
		CJsonObject json(strJsonIn);
		string strOutInfo;
		string strMessage;
		int nSSResult = 0;
		int nResult = -1;
		int nErrFlag = 0;
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}
			// cardID,cardType,name,cardNum,city,cardNum,operator,bankcode
			json.Get("CardID", CardInfo.strCardID);
			json.Get("Name", CardInfo.strName);
			json.Get("City", CardInfo.strCity);
			json.Get("BankCode", CardInfo.strBankCode);
			json.Get("CardNum", CardInfo.strCardNum);
			json.Get("Operator", CardInfo.strOperator);
			CardInfo.strCardType = "A";
			CardInfo.strOperator = "Administrator";

			if (CardInfo.strCardID.empty() ||
				CardInfo.strName.empty() ||
				CardInfo.strCity.empty() ||
				CardInfo.strBankCode.empty() ||
				CardInfo.strCardNum.empty())
			{
				strMessage = "身份证,姓名,银行代码,城市代码,社保卡号不能为空!";
				break;
			}
			if (QFailed(nSSResult = saveCardGs(CardInfo, strOutInfo)))
			{
				strMessage = "Failed saveCardGs:";
				strMessage += strOutInfo;
				break;
			}
			CJsonObject tmpJson;
			if (!tmpJson.Parse(strOutInfo))
			{
				strMessage = "Output of saveCardGs is invalid!";
				break;
			}

			string strErrFlag;
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of saveCardBhk!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of saveCardBhk!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}
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

	// 
	virtual int QueryPayResult(string& strJsonIn, string& strJsonOut) override
	{
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
		else
			return -1;
	}

	/*
	input: CardID,Name,City
	Output:CardID,CardNum,Name,Gender,Birthday,Nation,Address,Mobile,OccupType
	*/
	int QueryPersionOnfo(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo CardInfo;
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			jsonIn.Get("CardID", CardInfo.strCardID);
			jsonIn.Get("Name", CardInfo.strName);
			jsonIn.Get("City", CardInfo.strCity);
			CardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (CardInfo.strCardID.empty() ||
				CardInfo.strName.empty() ||
				CardInfo.strCity.empty())
			{
				strMessage = "身份证,姓名,城市代码!";
				break;
			}
			if (QFailed(queryPersonInfo(CardInfo, strJsonOut)))
			{
				strMessage = "Failed in queryPersonInfo!";
				break;
			}
			CJsonObject tmpJson;
			if (!tmpJson.Parse(strJsonOut))
			{
				strMessage = "Output of queryPersonInfo is invalid!";
				break;
			}

			string strErrFlag;
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryPersonInfo!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryPersonInfo!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}
			CJsonObject jsonOut;

			tmpJson.Get("yxzjhm", CardInfo.strCardID);		//有效证件号码			
			tmpJson.Get("shbzhm", CardInfo.strCardNum);		//社会保障号码			
			tmpJson.Get("xm", CardInfo.strName);			//姓名
			tmpJson.Get("xb", CardInfo.strSex);				//性别
			tmpJson.Get("csrq", CardInfo.strBirthday);		//出生日期
			tmpJson.Get("mz", CardInfo.strNation);			//民族
			tmpJson.Get("txdz", CardInfo.strAdress);		//通讯地址
			tmpJson.Get("sjhm", CardInfo.strMobile);		//手机号码
			tmpJson.Get("zylb", CardInfo.strOccupType);		//职业类别


			jsonOut.Add("CardID", CardInfo.strCardID);
			jsonOut.Add("CardNum", CardInfo.strCardNum);
			jsonOut.Add("Name", CardInfo.strName);
			jsonOut.Add("Gender", CardInfo.strSex);
			jsonOut.Add("Birthday", CardInfo.strBirthday);
			jsonOut.Add("Nation", CardInfo.strNation);
			jsonOut.Add("Address", CardInfo.strAdress);
			jsonOut.Add("Mobile", CardInfo.strMobile);
			jsonOut.Add("OccupType", CardInfo.strOccupType);

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
	/*
	input: CardID,Name,City
	Output:CardID,CardNum,Name,Gender,Birthday,Nation,Address,Mobile,OccupType
	*/
	int QueryPersonPhoto(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo CardInfo;
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			jsonIn.Get("CardID", CardInfo.strCardID);
			jsonIn.Get("Name", CardInfo.strName);
			jsonIn.Get("City", CardInfo.strCity);
			CardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (CardInfo.strCardID.empty() ||
				CardInfo.strName.empty() ||
				CardInfo.strCity.empty())
			{
				strMessage = "身份证,姓名,城市代码!";
				break;
			}
			if (QFailed(queryPerPhoto(CardInfo, strJsonOut)))
			{
				strMessage = "Failed in queryPerPhoto!";
				break;
			}
			CJsonObject tmpJson;
			if (!tmpJson.Parse(strJsonOut))
			{
				strMessage = "Output of saveCardGs is invalid!";
				break;
			}

			string strErrFlag;
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryPerPhoto!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryPerPhoto!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardInfoBySfzhm is invalid !";
				break;
			}

			ds.Get("photostr", CardInfo.strPhoto);
			CJsonObject jsonOut;
			jsonOut.Add("Photo", CardInfo.strPhoto);

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

	int ModifyPersonInfo(string& strJsonIn, string& strJsonOut)
	{
		return -1;
	}

	// CardID,Name,City,CardNum,SignatureKey
	virtual int GetCA(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 0;
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
				strMessage = "can't locate field 'errflag' in output of queryPerPhoto!";
				break;
			}
			nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryPerPhoto!";
				else
				{
					tmpJson.Get("errtext", strMessage);
				}
				break;
			}

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardInfoBySfzhm is invalid !";
				break;
			}

			string strCaOut = ds.ToString();

			if (!strCaOut.size())
			{
				strMessage = "CA out is empty!";
				break;
			}

			strJsonOut = strCaOut;
			CardInfo.strCardNum = strCardNum;
			CardInfo.strCardID = strCardID;
			CardInfo.strName = strName;
			CardInfo.strCity = strCity;

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
