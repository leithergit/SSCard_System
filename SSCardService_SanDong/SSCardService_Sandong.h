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

	SSCardService_Sandong() = delete;

	SSCardService_Sandong(ServiceType nSvrType)
		:SSCardService(nSvrType)
	{
	}

	virtual int Initialize(string& strInitJson, string& szOutInfo)
	{
		//Warning("需要确定initCardInfo输入数据的Json定义!");
		return initCardInfo(strInitJson.c_str(), szOutInfo);
	}
	~SSCardService_Sandong()
	{

	}

	virtual int SaveCardData(string& strINIFile)
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

	virtual int LoadCardData(string& strINIFile)
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
		int nResult = -1;
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

			TempCardInfo.strCardID = jsonIn["CardID"].ToString();
			TempCardInfo.strName = jsonIn["Name"].ToString();
			TempCardInfo.strCity = jsonIn["City"].ToString();
			TempCardInfo.strBankCode = jsonIn["BankCode"].ToString();
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
			if (QFailed(nResult = chkCanCardSq(TempCardInfo, strOutInfo)))
			{
				strMessage = "Faled in chkCanCardSq:";
				strMessage += std::to_string(nResult);
				break;
			}

			CJsonObject OutJson(strOutInfo);
			if (!OutJson.KeyExist("errflag") ||
				!OutJson.KeyExist("errtext"))
			{
				strMessage = "can't locate errflag or errtext!";
				break;
			}
			int nErrFlag = 1;
			OutJson.Get("errflag", nErrFlag);
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
		CJsonObject jsonOut;
		jsonOut.Add("Message", strMessage);

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
		SSCardNum
		CardStatus
		BankName
		Mobile
	*/
	int QueryOldCardStatus(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo TempCardInfo;
		CJsonObject jsonIn;
		int nResult = -1;
		string strMessage;
		string strOutInfo;
		do
		{
			if (!jsonIn.Parse(strJsonIn))
			{
				strMessage = "strJsonIn Input is invalid!";
				break;
			}
			TempCardInfo.strCardID = jsonIn["CardID"].ToString();
			TempCardInfo.strName = jsonIn["Name"].ToString();
			TempCardInfo.strCity = jsonIn["City"].ToString();
			TempCardInfo.strBankCode = jsonIn["BankCode"].ToString();
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

			if (QFailed(nResult = chkCanCardBh(TempCardInfo, strOutInfo)))
			{
				strMessage = "Faled in chkCanCardSq:";
				strMessage += std::to_string(nResult);
				break;
			}

			CJsonObject outJson(strOutInfo);
			int nErrFlag = 1;

			if (!outJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field errflag from output of chkCanCardBh!";
				break;
			}
			if (nErrFlag)
			{
				if (!outJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' from output of chkCanCardBh!";
				else
				{
					strMessage = outJson["errtext"].ToString();
				}
				break;
			}

			if (QFailed(nResult = queryCardInfoBySfzhm(TempCardInfo, strOutInfo)))
			{
				strMessage = "Failed in queryCardInfoBySfzhm:";
				strMessage += std::to_string(nResult);
				break;
			}

			if (!outJson.Parse(strOutInfo))
			{
				strMessage = "queryCardInfoBySfzhm output invalid:";
				strMessage += strOutInfo;
				break;
			}
			if (!outJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' from output of queryCardInfoBySfzhm!";
				break;
			}

			if (nErrFlag)
			{
				if (!outJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' from output of queryCardInfoBySfzhm!";
				else
				{
					strMessage = outJson["errtext"].ToString();
				}
				break;
			}
			CJsonObject ds = outJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardInfoBySfzhm is invalid !";
				break;
			}

			CardInfo.strCardID = ds["shbzhm"].ToString();
			CardInfo.strCardNum = ds["kh"].ToString();
			CardInfo.strName = ds["xm"].ToString();
			//CardInfo.strst = ds["kzt"].ToString();
			CardInfo.strChipNum = ds["kxpzh"].ToString();
			CardInfo.strMagNum = ds["kctzh"].ToString();
			CardInfo.strBankCode = ds["yhbh"].ToString();		// 银行编号
			//PersionInfo.strCardStatus = ds["kztmc"] .ToString();	// 卡状态名称
			//PersionInfo.strBankCode = ds["yhmc"] .ToString();		// 银行名称
			CardInfo.strMobile = ds["sjhm"].ToString();

			outJson.Clear();
			outJson.Add("Name", CardInfo.strName);
			outJson.Add("CardID", CardInfo.strCardID);
			outJson.Add("SSCardNum", CardInfo.strCardNum);
			outJson.Add("BankName", ds["yhmc"].ToString());
			outJson.Add("SSCardStatus", ds["kztmc"].ToString());
			outJson.Add("Mobile", ds["sjhm"].ToString());
			strJsonOut = outJson.ToString();
			strMessage = "Succeed";
		} while (0);
		CJsonObject jsonOut;
		jsonOut.Add("Message", strMessage);
		strJsonOut = jsonOut.ToString();
		return nResult;
	}
	/*
	input string
	 {
	 "CardID":"33333333333333",
	 "Name":"",
	 "City":"",			// 行政区域
	 "BankCode":""
	 "PaperType":""		// 证件类型，A为身份证
	}

	output string
	{
		"message":""
	}
	*/

	virtual int QueryCardStatus(string& strJsonIn, string& strJsonOut)
	{
		switch (nServiceType)
		{
		case Service_NewCard:
			return QueryNewCardStatus(strJsonIn, strJsonOut);
			break;
		case Service_ReplaceCard:
			return QueryOldCardStatus(strJsonIn, strJsonOut);
			break;
		case Service_RegisterLost:
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
		int nResult = -1;
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
			TempCardInfo.strReleaseDate = jsonIn["IssueDate"].ToString();
			TempCardInfo.strValidDate = jsonIn["ExpireDate"].ToString();
			TempCardInfo.strMobile = jsonIn["Mobile"].ToString();
			TempCardInfo.strOperator = jsonIn["Operator"].ToString();
			TempCardInfo.strOccupType = jsonIn["OccupType"].ToString();
			TempCardInfo.strBirthday = jsonIn["Birthday"].ToString();
			TempCardInfo.strSex = jsonIn["Gender"].ToString();
			TempCardInfo.strNation = jsonIn["Nation"].ToString();
			TempCardInfo.strAdress = jsonIn["Address"].ToString();
			TempCardInfo.strPhoto = jsonIn["Photo"].ToString();

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
			if (QFailed(nResult = saveCardSqList(TempCardInfo, strOutInfo)))
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

			int nErrFlag = -1;
			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' from output of saveCardSqList!";
				break;
			}
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' from output of saveCardSqList!";
				else
				{
					strMessage = tmpJson["errtext"].ToString();
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
			if (QFailed(nResult = queryCardZksqList(TempCardInfo, strOutInfo)))
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

			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryCardZksqList!";
				break;
			}
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryCardZksqList!";
				else
				{
					strMessage = tmpJson["errtext"].ToString();
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
		CJsonObject jsonOut;
		jsonOut.Add("Message", strMessage);
		strJsonOut = jsonOut.ToString();
		return nResult;
	}

	// cardID,cardType,name,bankCode,city,mobile,reason,operator,OccupType,birthday,sex,nation,address,photo
	int CommitReplaceCardInfo(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo TempCardInfo;
		CJsonObject json(strJsonIn);
		int nResult = -1;
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
			TempCardInfo.strCity = json["City"].ToString();
			TempCardInfo.strMobile = json["Mobile"].ToString();
			TempCardInfo.strReason = json["Reason"].ToString();
			TempCardInfo.strOperator = json["Operator"].ToString();
			TempCardInfo.strOccupType = json["OccupType"].ToString();
			TempCardInfo.strReleaseDate = json["IssueDate"].ToString();
			TempCardInfo.strValidDate = json["ExpireDate"].ToString();
			TempCardInfo.strBirthday = json["Birthday"].ToString();
			TempCardInfo.strSex = json["Gender"].ToString();
			TempCardInfo.strNation = json["Nation"].ToString();
			TempCardInfo.strAdress = json["Address"].ToString();
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
			if (QFailed(nResult = saveCardBhList(TempCardInfo, strOutInfo)))
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
			int nErrFlag = -1;

			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryCardZksqList!";
				break;
			}
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryCardZksqList!";
				else
				{
					strMessage = tmpJson["errtext"].ToString();
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
			if (QFailed(nResult = queryCardZksqList(TempCardInfo, strOutInfo)))
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

			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryCardZksqList!";
				break;
			}
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryCardZksqList!";
				else
				{
					strMessage = tmpJson["errtext"].ToString();
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

			CardInfo.strCardNum = ds["shbzhm"].ToString();
			CardInfo.strMobile = ds["sjhm"].ToString();

			// cardID,cardType,name,bankCode,city,cardNum,operator
			if (QFailed(nResult = saveCardBhk(TempCardInfo, strOutInfo)))
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

			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of saveCardBhk!";
				break;
			}
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of saveCardBhk!";
				else
				{
					strMessage = tmpJson["errtext"].ToString();
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
		CJsonObject jsonOut;
		jsonOut.Add("Message", strMessage);
		strJsonOut = jsonOut.ToString();
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
	virtual int CommitPersonInfo(string& strJsonIn, string& strJsonOut)
	{
		switch (nServiceType)
		{
		case Service_NewCard:
			return CommitNewCardInfo(strJsonIn, strJsonOut);
			break;
		case Service_ReplaceCard:
			return CommitReplaceCardInfo(strJsonIn, strJsonOut);
			break;
		case Service_RegisterLost:
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
	virtual int PreMakeCard(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo TempCardInfo;
		CJsonObject jsonIn;
		int nResult = -1;
		string strMessage;
		string strOutInfo;
		do
		{

			// cardID,cardType,name,bankCode,operator,city
			if (QFailed(nResult = saveCardOpen(CardInfo, strOutInfo)))
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
			if (QFailed(nResult = saveCardOpen(CardInfo, strOutInfo)))
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
			int nErrFlag;
			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryCardZksqList!";
				break;
			}
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of queryCardZksqList!";
				else
				{
					strMessage = tmpJson["errtext"].ToString();
				}
				break;
			}

			CJsonObject ds = tmpJson["ds"];
			if (ds.IsEmpty())
			{
				strMessage = "ds from output of queryCardZksqList is invalid !";
				break;
			}
			CardInfo.strCardNum = ds["sbkh"].ToString();
			CardInfo.strPhoto = ds["zp"].ToString();
			nResult = 0;
		} while (0);
		CJsonObject jsonOut;
		jsonOut.Add("Message", strMessage);
		strJsonOut = jsonOut.ToString();
		return nResult;
	}

	// 读卡
	virtual int ReadCard(string& strJsonIn, string& strJsonOut)
	{
		return 0;
	}

	// 写卡
	virtual int WriteCard(string& strJsonIn, string& strJsonOut)
	{
		return -1;
	}

	// 打印
	// 
	virtual int PrintCard(string& strJsonIn, string& strJsonOut)
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
	virtual int ReturnData(string& strJsonIn, string& strJsonOut)
	{
		CJsonObject json(strJsonIn);
		int nResult = -1;
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
			if (QFailed(nResult = saveCardCompleted(CardInfo, strOutInfo)))
			{
				strMessage = "Failed in saveCardCompleted:";
				strMessage += strOutInfo;
				break;
			}


			nResult = 0;
		} while (0);
		CJsonObject jsonOut;
		jsonOut.Add("Message", strMessage);
		strJsonOut = jsonOut.ToString();
		return nResult;
	}

	// 激活及后续收尾工作
	// cardID,cardType,name,cardNum,bankCode,operator,city,magNum
	virtual int ActiveCard(string& strJsonIn, string& strJsonOut)
	{
		SD_SSCardInfo TempCardInfo;
		CJsonObject json(strJsonIn);
		int nResult = -1;
		string strMessage;
		string strOutInfo;
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}

			TempCardInfo.strCardID = json["CardID"].ToString();
			TempCardInfo.strCardType = json["PaperType"].ToString();
			TempCardInfo.strName = json["Name"].ToString();
			TempCardInfo.strBankCode = json["BankCode"].ToString();
			TempCardInfo.strOperator = json["Operator"].ToString();
			TempCardInfo.strCity = json["City"].ToString();
			TempCardInfo.strCardNum = json["cardNum"].ToString();

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

			if (QFailed(nResult = saveCardActive(TempCardInfo, strOutInfo)))
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
			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of saveCardActive!";
				break;
			}
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of saveCardActive!";
				else
				{
					strMessage = tmpJson["errtext"].ToString();
				}
				break;
			}

			// cardID,cardType,name,magNum,bankCode,operator,city
			if (QFailed(nResult = saveCardJrzhActive(TempCardInfo, strOutInfo)))
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
			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of saveCardJrzhActive!";
				break;
			}
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of saveCardJrzhActive!";
				else
				{
					strMessage = tmpJson["errtext"].ToString();
				}
				break;
			}

			CardInfo = TempCardInfo;
			nResult = 0;
		} while (0);
		CJsonObject jsonOut;
		jsonOut.Add("Message", strMessage);
		strJsonOut = jsonOut.ToString();
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
	virtual int RegisterLostTemporary(string& strJsonIn, string& strJsonOut, int nOperation = 0)
	{
		SD_SSCardInfo TempCardInfo;
		CJsonObject json;
		string strOutInfo;
		string strMessage;
		int nResult = 0;
		do
		{
			if (QFailed(nResult = QueryCardStatus(strJsonIn, strJsonOut)))
			{
				break;
			}
			json.Parse(strJsonIn);
			CJsonObject tmpJson;
			string strFunction = "saveCardLsgs";
			CardInfo.strOperator = json["Operator"].ToString();
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
				if (QFailed(nResult = saveCardLsgs(CardInfo, strOutInfo)))
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
				if (QFailed(nResult = saveCardLsgsjg(CardInfo, strOutInfo)))
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
			int nErrFlag;
			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of ";
				strMessage += strFunction;
				strMessage += "!";
				break;
			}
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
					strMessage = tmpJson["errtext"].ToString();
				}
				break;
			}
			nResult = 0;
		} while (0);
		CJsonObject jsonOut;
		jsonOut.Add("Message", strMessage);
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
		"PaperType"
		"CardNum"
		"Operator"
	*/
	virtual int RegisterLost(string& strJsonIn, string& strJsonOut, int nOperation = 0)
	{
		SD_SSCardInfo TempCardInfo;
		CJsonObject json(strJsonIn);
		string strOutInfo;
		string strMessage;
		int nResult = 0;
		do
		{
			if (json.IsEmpty())
			{
				strMessage = "Json Input is empty!";
				break;
			}
			// cardID,cardType,name,cardNum,city,cardNum,operator,bankcode
			TempCardInfo.strCardID = json["CardID"].ToString();
			TempCardInfo.strName = json["Name"].ToString();
			TempCardInfo.strCity = json["City"].ToString();
			TempCardInfo.strBankCode = json["BankCode"].ToString();
			TempCardInfo.strCardType = json["PaperType"].ToString();
			TempCardInfo.strCardNum = json["CardNum"].ToString();
			TempCardInfo.strOperator = json["Operator"].ToString();

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
			if (QFailed(nResult = saveCardGs(TempCardInfo, strOutInfo)))
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
			int nErrFlag = -1;
			if (!tmpJson.Get("errflag", nErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of saveCardBhk!";
				break;
			}
			if (nErrFlag)
			{
				if (!tmpJson.KeyExist("errtext"))
					strMessage = "can't locate field 'errtext' in output of saveCardBhk!";
				else
				{
					strMessage = tmpJson["errtext"].ToString();
				}
				break;
			}
			nResult = 0;
		} while (0);
		CJsonObject jsonOut;
		jsonOut.Add("Message", strMessage);
		return nResult;
	}

	virtual int ExecExtraInterface(const string& strCommand, string& strInput, string& strOutput)
	{
		return -1;
	}
};
