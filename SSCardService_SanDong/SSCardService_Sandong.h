#pragma once
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "../SSCardService/SSCardService.h"
#include "../SDK/SSCardInfo_Sandong/SD_SSCardInfo.h"
#include "Utility.h"
#include "CJsonObject.hpp"
#include <QtCore/qglobal.h>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>

//宏定义
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)")"
#define Warning(strMessage) message( __LOC__ " Warning: " strMessage)


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
	{}

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
		SD_SSCardInfo TempCardInfo;
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

			jsonIn.Get("CardID", TempCardInfo.strCardID);
			jsonIn.Get("Name", TempCardInfo.strName);
			jsonIn.Get("City", TempCardInfo.strCity);
			jsonIn.Get("BankCode", TempCardInfo.strBankCode);
			TempCardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (TempCardInfo.strCardID.empty()
				|| TempCardInfo.strName.empty()
				|| TempCardInfo.strCity.empty()
				|| TempCardInfo.strBankCode.empty()
				//||TempCardInfo.strCardType.empty()
				)
			{
				strMessage = "Any items of CardID,Name,City,Bankcode can't be empty!";
				break;
			}

			CardInfo = TempCardInfo;
			if (QFailed(nSSResult = chkCanCardSq(TempCardInfo, strOutInfo)))
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
		SSCardNum
		CardStatus
		BankName
		Mobile
	*/
	int QueryOldCardStatus(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo TempCardInfo;
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
			jsonIn.Get("CardID", TempCardInfo.strCardID);
			jsonIn.Get("Name", TempCardInfo.strName);
			jsonIn.Get("City", TempCardInfo.strCity);
			jsonIn.Get("BankCode", TempCardInfo.strBankCode);
			TempCardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (TempCardInfo.strCardID.empty()
				|| TempCardInfo.strName.empty()
				|| TempCardInfo.strCity.empty()
				|| TempCardInfo.strBankCode.empty()
				|| TempCardInfo.strCardType.empty()
				)
			{
				strMessage = "Any items of CardID,Name,City,Bankcode can't be empty!";
				break;
			}

			if (QFailed(nSSResult = chkCanCardBh(TempCardInfo, strOutInfo)))
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
		SSCardNum
		CardStatus
		BankName
		Mobile
	*/
	int QueryCardInfo(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo TempCardInfo;
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
			jsonIn.Get("CardID", TempCardInfo.strCardID);
			jsonIn.Get("Name", TempCardInfo.strName);
			jsonIn.Get("City", TempCardInfo.strCity);
			jsonIn.Get("BankCode", TempCardInfo.strBankCode);
			TempCardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (QFailed(nSSResult = queryCardInfoBySfzhm(TempCardInfo, strOutInfo)))
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
			outJson.Add("SSCardNum", CardInfo.strCardNum);
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

	// 	virtual int QueryCardInfo(string& strJsonIn, string& strJsonOut)
	// 	{
	// 		switch (nServiceType)
	// 		{
	// 		case ServiceType::Service_NewCard:
	// 			return QueryNewCardStatus(strJsonIn, strJsonOut);
	// 			break;
	// 		case ServiceType::Service_ReplaceCard:
	// 			return QueryOldCardStatus(strJsonIn, strJsonOut);
	// 			break;
	// 		case ServiceType::Service_RegisterLost:
	// 		default:
	// 		{
	// 			CJsonObject jsonOut;
	// 			jsonOut.Add("Message", "Invalid Service Type!");
	// 			return -1;
	// 			break;
	// 		}
	// 		}
	// 	}
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
	"SSCardNum":"",
	"Mobile":"",
	"BankCode":""
	}
	*/
	int CommitNewCardInfo(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo TempCardInfo;
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

			TempCardInfo = CardInfo;
			/*TempCardInfo.strName = jsonIn["Name"].ToString();
			TempCardInfo.strCity = jsonIn["City"].ToString();
			TempCardInfo.strBankCode = jsonIn["BankCode"].ToString();*/
			TempCardInfo.strDealType = "0";	// new card
			//TempCardInfo.strCardType = jsonIn["PaperType"].ToString();
			jsonIn.Get("IssueDate", TempCardInfo.strReleaseDate);
			jsonIn.Get("ExpireDate", TempCardInfo.strValidDate);
			jsonIn.Get("Mobile", TempCardInfo.strMobile);
			jsonIn.Get("Operator", TempCardInfo.strOperator);
			jsonIn.Get("OccupType", TempCardInfo.strOccupType);
			jsonIn.Get("Birthday", TempCardInfo.strBirthday);
			jsonIn.Get("Gender", TempCardInfo.strSex);
			jsonIn.Get("Nation", TempCardInfo.strNation);
			jsonIn.Get("Address", TempCardInfo.strAdress);
			jsonIn.Get("Photo", TempCardInfo.strPhoto);

			if (TempCardInfo.strCardID.empty()
				|| TempCardInfo.strName.empty()
				|| TempCardInfo.strCity.empty()
				|| TempCardInfo.strBankCode.empty()
				|| TempCardInfo.strCardType.empty()
				|| TempCardInfo.strMobile.empty()
				|| TempCardInfo.strOperator.empty()
				|| TempCardInfo.strOccupType.empty()
				|| TempCardInfo.strReleaseDate.empty()
				|| TempCardInfo.strValidDate.empty()
				|| TempCardInfo.strBirthday.empty()
				|| TempCardInfo.strSex.empty()
				|| TempCardInfo.strNation.empty()
				|| TempCardInfo.strAdress.empty()
				|| TempCardInfo.strPhoto.empty())
			{
				strMessage = "Any items of cardID,cardType,name,city,bankcode,mobile,operator,OccupType,releaseDate,validDate,Birthday,sex,nation,address photo can't be empty!";
				break;
			}
			CardInfo = TempCardInfo;
			if (QFailed(nSSResult = saveCardSqList(TempCardInfo, strOutInfo)))
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

			TempCardInfo.strReleaseDate = date1.str();
			TempCardInfo.strValidDate = date2.str();

			// city,bankcode,validDate,dealType,releaseDate
			if (QFailed(nSSResult = queryCardZksqList(TempCardInfo, strOutInfo)))
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
			outJson.Add("SSCardNum", CardInfo.strCardNum);
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
		SD_SSCardInfo TempCardInfo;
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
			TempCardInfo.strDealType = "1";	// replace card
			//TempCardInfo.strCardID = json["CardID"].ToString();			
			//TempCardInfo.strName = json["Name"].ToString();
			//TempCardInfo.strBankCode = json["BankCode"].ToString();
			TempCardInfo.strCardType = "A";// 仅支持身份证//json["PaperType"].ToString();
			json.Get("City", TempCardInfo.strCity);
			json.Get("Mobile", TempCardInfo.strMobile);
			json.Get("Reason", TempCardInfo.strReason);
			json.Get("Operator", TempCardInfo.strOperator);
			json.Get("OccupType", TempCardInfo.strOccupType);
			json.Get("IssueDate", TempCardInfo.strReleaseDate);
			json.Get("ExpireDate", TempCardInfo.strValidDate);
			json.Get("Birthday", TempCardInfo.strBirthday);
			json.Get("Gender", TempCardInfo.strSex);
			json.Get("Nation", TempCardInfo.strNation);
			json.Get("Address", TempCardInfo.strAdress);
			//TempCardInfo.strPhoto = json["Photo"].ToString();	// 补办卡不需要身份证

			if (TempCardInfo.strCardID.empty() ||
				TempCardInfo.strName.empty() ||
				TempCardInfo.strBankCode.empty() ||
				TempCardInfo.strCity.empty() ||
				TempCardInfo.strMobile.empty() ||
				TempCardInfo.strReason.empty() ||
				TempCardInfo.strOperator.empty() ||
				TempCardInfo.strOccupType.empty() ||
				TempCardInfo.strCardType.empty() ||
				TempCardInfo.strReleaseDate.empty() ||
				TempCardInfo.strValidDate.empty() ||
				TempCardInfo.strBirthday.empty() ||
				TempCardInfo.strSex.empty() ||
				TempCardInfo.strNation.empty() ||
				TempCardInfo.strAdress.empty()
				/*TempCardInfo.strPhoto.empty()*/)
			{
				strMessage = "Any items of cardID,cardType,name,bankCode,city,mobile,reason,operator,OccupType,birthday,sex,nation,address,photo can't be empty!";
				break;
			}
			CardInfo = TempCardInfo;
			if (QFailed(nSSResult = saveCardBhList(TempCardInfo, strOutInfo)))
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

			TempCardInfo.strReleaseDate = date1.str();
			TempCardInfo.strValidDate = date2.str();

			// city,bankcode,validDate,dealType,releaseDate
			if (QFailed(nSSResult = queryCardZksqList(TempCardInfo, strOutInfo)))
			{
				strMessage = "Faled in queryCardInfoBySfzhm:";
				strMessage += strOutInfo;
				break;
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
			if (QFailed(nSSResult = saveCardBhk(TempCardInfo, strOutInfo)))
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

			CJsonObject outJson;
			outJson.Add("Name", CardInfo.strName);
			outJson.Add("CardID", CardInfo.strCardID);
			outJson.Add("SSCardNum", CardInfo.strCardNum);
			outJson.Add("Mobile", CardInfo.strMobile);
			outJson.Add("BankCode", tmpJson["yhbh"].ToString());
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
		SD_SSCardInfo TempCardInfo;
		CJsonObject jsonIn;
		int nSSResult = -1;
		int nResult = -1;
		int nErrFlag = 0;
		string strMessage;
		string strOutInfo;
		do
		{

			// cardID,cardType,name,bankCode,operator,city
			if (QFailed(nSSResult = saveCardOpen(CardInfo, strOutInfo)))
			{
				strMessage = "Failed in saveCardSqList:";
				strMessage += strOutInfo;
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
			if (QFailed(nSSResult = saveCardOpen(CardInfo, strOutInfo)))
			{
				strMessage = "Faled in saveCardOpen:";
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

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardZksqList is invalid !";
				break;
			}
			ds.Get("sbkh", CardInfo.strCardNum);
			ds.Get("zp", CardInfo.strPhoto);
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

			if (CardInfo.strCardID.empty() ||
				CardInfo.strName.empty() ||
				CardInfo.strCity.empty() ||
				CardInfo.strBankCode.empty() ||
				CardInfo.strCardType.empty() ||
				CardInfo.strOperator.empty() ||
				CardInfo.strCardNum.empty() ||
				CardInfo.strChipNum.empty() ||
				CardInfo.strMagNum.empty() ||
				CardInfo.strCardATR.empty() ||
				CardInfo.strIdentifyNum.empty() ||
				CardInfo.strCardVersion.empty() ||
				CardInfo.strChipType.empty())
			{
				strMessage = "Any items of cardID,cardType,name,bankCode,operator,city,cardNum,chipNum,MagNum,ATR,identifyNum,cardVersion,chipType can't be empty!";
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
	// cardID,cardType,name,cardNum,bankCode,operator,city,magNum
	virtual int ActiveCard(string& strJsonIn, string& strJsonOut) override
	{
		SD_SSCardInfo TempCardInfo;
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

			json.Get("CardID", TempCardInfo.strCardID);
			json.Get("PaperType", TempCardInfo.strCardType);
			json.Get("Name", TempCardInfo.strName);
			json.Get("BankCode", TempCardInfo.strBankCode);
			json.Get("Operator", TempCardInfo.strOperator);
			json.Get("City", TempCardInfo.strCity);
			json.Get("cardNum", TempCardInfo.strCardNum);

			if (TempCardInfo.strCardID.empty() ||
				TempCardInfo.strName.empty() ||
				TempCardInfo.strCity.empty() ||
				TempCardInfo.strBankCode.empty() ||
				TempCardInfo.strCardType.empty() ||
				TempCardInfo.strOperator.empty() ||
				TempCardInfo.strCardNum.empty())
			{
				strMessage = "Any items of cardID,cardType,name,bankCode,operator,city,cardNum,chipNum,MagNum,ATR,identifyNum,cardVersion,chipType can't be empty!";
				break;
			}

			if (QFailed(nSSResult = saveCardActive(TempCardInfo, strOutInfo)))
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
			if (QFailed(nSSResult = saveCardJrzhActive(TempCardInfo, strOutInfo)))
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

			CardInfo = TempCardInfo;
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
		SD_SSCardInfo TempCardInfo;
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
			if (nOperation == 0)
			{
				if (CardInfo.strCardID.empty() ||
					CardInfo.strName.empty() ||
					CardInfo.strCity.empty() ||
					CardInfo.strBankCode.empty() ||
					CardInfo.strCardType.empty() ||
					CardInfo.strCardNum.empty() ||
					CardInfo.strOperator.empty())
				{
					strMessage = "CardID,Name,City,Bankcode,PaperType,CardNum,Operator can't be empty!";
					break;
				}
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
				if (CardInfo.strCardID.empty() ||
					CardInfo.strName.empty() ||
					CardInfo.strCity.empty() ||
					CardInfo.strBankCode.empty() ||
					CardInfo.strCardType.empty() ||
					CardInfo.strCardNum.empty())
				{
					strMessage = "CardID,Name,City,Bankcode,PaperType,CardNum can't be empty!";
					break;
				}
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
		SD_SSCardInfo TempCardInfo;
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
			json.Get("CardID", TempCardInfo.strCardID);
			json.Get("Name", TempCardInfo.strName);
			json.Get("City", TempCardInfo.strCity);
			json.Get("BankCode", TempCardInfo.strBankCode);
			json.Get("CardNum", TempCardInfo.strCardNum);
			json.Get("Operator", TempCardInfo.strOperator);
			TempCardInfo.strCardType = "A";

			if (TempCardInfo.strCardID.empty() ||
				TempCardInfo.strName.empty() ||
				TempCardInfo.strCity.empty() ||
				TempCardInfo.strBankCode.empty() ||
				TempCardInfo.strCardType.empty() ||
				TempCardInfo.strCardNum.empty() ||
				TempCardInfo.strOperator.empty())
			{
				strMessage = "CardID,Name,City,Bankcode,PaperType,CardNum,Operation can't be empty!";
				break;
			}
			if (QFailed(nSSResult = saveCardGs(TempCardInfo, strOutInfo)))
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
		jsonOut.Add("Message", "不支付或没有支付流程!");
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
		SD_SSCardInfo TempCardInfo;
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
			jsonIn.Get("CardID", TempCardInfo.strCardID);
			jsonIn.Get("Name", TempCardInfo.strName);
			jsonIn.Get("City", TempCardInfo.strCity);
			TempCardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (TempCardInfo.strCardID.empty()
				|| TempCardInfo.strName.empty()
				|| TempCardInfo.strCity.empty()
				|| TempCardInfo.strCardType.empty())
			{
				strMessage = "Any items of CardID,Name,City,Bankcode can't be empty!";
				break;
			}
			if (QFailed(queryPersonInfo(TempCardInfo, strJsonOut)))
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

			tmpJson.Get("yxzjhm", TempCardInfo.strCardID);		//有效证件号码			
			tmpJson.Get("shbzhm", TempCardInfo.strCardNum);		//社会保障号码			
			tmpJson.Get("xm", TempCardInfo.strName);		//姓名
			tmpJson.Get("xb", TempCardInfo.strSex);			//性别
			tmpJson.Get("csrq", TempCardInfo.strBirthday);	//出生日期
			tmpJson.Get("mz", TempCardInfo.strNation);		//民族
			tmpJson.Get("txdz", TempCardInfo.strAdress);		//通讯地址
			tmpJson.Get("sjhm", TempCardInfo.strMobile);		//手机号码
			tmpJson.Get("zylb", TempCardInfo.strOccupType);	//职业类别
			// tmpJson.Get("yxzjlx"	,);		//有效证件类型
			// tmpJson.Get("lxdh"	, TempCardInfo.strMobile);		//固定电话或其他联系电话
			// tmpJson.Get("gj"	, TempCardInfo.strco);			//国籍
			// tmpJson.Get("yzbm"	, TempCardInfo.s);		//邮政编码
			// tmpJson.Get("zjqsrq"	, TempCardInfo.strReleaseDate);		//证件有效起始日期
			// tmpJson.Get("zjdqrq"	, TempCardInfo.strValidDate);		//证件有效终止日期
			// tmpJson.Get("fzjg"	,);		//发证机关
			// tmpJson.Get("csd"	,);		//	出生地
			// tmpJson.Get("hjszd"	,);		//户籍所在地
			// tmpJson.Get("hkxz"	,);		//户口性质
			// tmpJson.Get("whcd"	,);		//文化程度
			// tmpJson.Get("hyzk"	,);		//婚姻状况
			// tmpJson.Get("lxrxm"	,);		//联系人姓名
			// tmpJson.Get("lxrdh"	,);		//联系人电话
			// tmpJson.Get("jgbh"	,);		//机构编号
			// tmpJson.Get("jgmc"	,);		//机构名称

			jsonOut.Add("CardID", TempCardInfo.strCardID);
			jsonOut.Add("CardNum", TempCardInfo.strCardNum);
			jsonOut.Add("Name", TempCardInfo.strName);
			jsonOut.Add("Gender", TempCardInfo.strSex);
			jsonOut.Add("Birthday", TempCardInfo.strBirthday);
			jsonOut.Add("Nation", TempCardInfo.strNation);
			jsonOut.Add("Address", TempCardInfo.strAdress);
			jsonOut.Add("Mobile", TempCardInfo.strMobile);
			jsonOut.Add("OccupType", TempCardInfo.strOccupType);

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
		SD_SSCardInfo TempCardInfo;
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
			jsonIn.Get("CardID", TempCardInfo.strCardID);
			jsonIn.Get("Name", TempCardInfo.strName);
			jsonIn.Get("City", TempCardInfo.strCity);
			TempCardInfo.strCardType = "A";// 仅支持身份证//jsonIn["PaperType"].ToString();

			if (TempCardInfo.strCardID.empty()
				|| TempCardInfo.strName.empty()
				|| TempCardInfo.strCity.empty()
				|| TempCardInfo.strCardType.empty())
			{
				strMessage = "Any items of CardID,Name,City,Bankcode can't be empty!";
				break;
			}
			if (QFailed(queryPerPhoto(TempCardInfo, strJsonOut)))
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
			CJsonObject jsonOut;

			tmpJson.Get("photostr", TempCardInfo.strPhoto);		//有效证件号码
			jsonOut.Add("Photo", TempCardInfo.strPhoto);

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
};
