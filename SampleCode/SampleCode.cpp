// SampleCode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include "../utility/Markup.h"
#include "../utility/json/CJsonObject.hpp"
using namespace std;
using namespace neb;
#include<filesystem>
using namespace std::filesystem;

bool IsDigitString(const char* szStr)
{
	int nLen = strlen(szStr);
	for (int i = 0; i < nLen; i++)
	{
		if (!isdigit(szStr[i]))
			return false;
	}
	return true;
}

bool TestDigit(char ch)
{
	if (ch < '0' || ch > '9')
		return false;
	else
		return true;
}

void  SplitString(const char* szStr, char* szDigit, char* szText)
{
	char* p = (char*)szStr;
	do
	{
		if (!TestDigit(*p))
		{
			break;
		}
		else
			*szDigit++ = *p;
	} while (*p++);
	strcpy(szText, p);
}

void ThreadProc()
{
	return;
}

void test1()
{
	bool bFlag = IsDigitString("123456");
	bFlag = IsDigitString("1234;56");
	bFlag = IsDigitString("10");
	bFlag = IsDigitString("ok");
	char szDigit[16] = { 0 };
	char szText[1024] = { 0 };
	const char* szT1[] = { "0",
							"01",
							"0202 银行失败原因(客户信息不规范，请先在不规范客户管理系统中完善客户信息)",
							"02 银行失败原因(客户信息不规范，请先在不规范客户管理系统中完善客户信息)",
							"银行失败原因(客户信息不规范，请先在不规范客户管理系统中完善客户信息)" };

	std::thread* pThread = new std::thread(ThreadProc);

	std::this_thread::sleep_for(std::chrono::seconds(1));
	pThread->join();
	delete pThread;
	for (auto var : szT1)
	{
		memset(szDigit, 0, 16);
		memset(szText, 0, 1024);
		SplitString(var, szDigit, szText);
		cout << "Digit = " << szDigit << "\t" << "Text = " << szText << endl;
	}
	try
	{
		do
		{
			string strOutputFile = "CA.TXT";
			fstream fs(strOutputFile, ios::in);
			stringstream ss;
			ss << fs.rdbuf();
			string strMessage;

			fs.close();
			CJsonObject tmpJson(ss.str());
			string strErrFlag;
			if (!tmpJson.Get("errflag", strErrFlag))
			{
				strMessage = "can't locate field 'errflag' in output of queryPerPhoto!";
				break;
			}
			int nErrFlag = strtol(strErrFlag.c_str(), nullptr, 10);
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

			string strCaOut;
			ds.Get("ca_out", strCaOut);
			if (!strCaOut.size())
			{
				strMessage = "CAout is empty!";
				break;
			}
			cout << strCaOut << endl;

			string strXmldoc = "<CA>";
			strXmldoc += strCaOut;
			strXmldoc += "</CA>";
			CMarkup xml;
			if (!xml.SetDoc(strXmldoc))
			{
				strMessage = "CAout is is invalid!";
				break;
			}
			if (!xml.FindElem("CA"))
				break;
			xml.IntoElem();
			if (xml.FindElem("QMMY"))
			{
				string strSinatureCerfifate = xml.GetElemContent();
				cout << strSinatureCerfifate << endl;
			}

			if (xml.FindChildElem("QMMY"))
			{
				xml.IntoElem();
				string strSinatureCerfifate = xml.GetElemContent();
				xml.OutOfElem();
			}

			string strSignatureKey = xml.GetAttrib("QMMY");
			string strPrimaryKey = xml.GetAttrib("ZKMY");
			string strAdminPin = xml.GetAttrib("GLYPIN");
			xml.OutOfElem();


			cout << strSignatureKey << endl;
			cout << strPrimaryKey << endl;
			cout << strAdminPin << endl;
		} while (0);

		system("pause");
	}
	catch (std::exception& e)
	{
	}
}

void TestUpateJson()
{
	try
	{
		string strFile = "D:/Work/SSCard_System.MVC/MainProject/SSCard_System/debug/Data/ProgressTemplate.json";
		ifstream ifs(strFile, ios::in);
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObject json;
		if (!json.Parse(ss.str()))
		{
			cout << "Invalid json file:" << strFile;
			return;
		}


		CJsonObject jsonCurProgress = json["NewCardProgress"];
		int nArraySize = jsonCurProgress.GetArraySize();
		for (int i = 0; i < nArraySize; i++)
		{
			CJsonObject json;
			jsonCurProgress.Get(i, json);
			cout << json.ToFormattedString();
		}
		CJsonObject jsonPerson = json["Person"];
		string strPersion = jsonPerson.ToString();
		jsonPerson.Replace("Name", "毛贼懂");
		jsonPerson.Replace("Identity", "342321198012104521");
		jsonPerson.Replace("Gender", "男");
		strPersion = jsonPerson.ToString();
		jsonCurProgress.Replace("queryPersonInfo", 0);
		jsonCurProgress.Replace("chkCanCardSq", 0);
		jsonCurProgress.Replace("saveCardSqList", 0);
		json.Replace("NewCardProgress", jsonCurProgress);
		json.Replace("Person", jsonPerson);
		ifs.close();
		ofstream ofs(strFile, ios::out | ios::binary);
		ofs << json.ToFormattedString();
	}
	catch (std::exception& e)
	{
		cout << "exception:" << e.what();
	}
}
#include <format>

void testMove_InitVec()
{
	vector<string> vecKey =
	{
		"chkCanCardSq",
		"saveCardSqList",
		"queryCardZksqList",
		"saveCardOpen",
		"saveCardCompleted",
		"saveCardActive",
		"saveCardJrzhActive"
	};
	for (auto var : vecKey)
	{
		cout << var << endl;
	}
}

namespace fs = std::filesystem;
void TestFileSystem()
{
	string strSoure = "./Data/Progress_362331197612164214.json";
	string strDest = "./Data/Finished/Progress_362331197612164214.json";
	if (exists("D:/Work/SSCard_System.MVC/MainProject/SSCard_System/debug/Data/ProgressTemplate.json1"))
	{
		cout << "exist!" << endl;
	}
	else
		cout << "missing!" << endl;

	try
	{
		//fs::path strSourcePath = fs::absolute(strSoure);
		//fs::path strDestPath = fs::absolute(strDest);
		//fs::copy(strSourcePath, strDestPath);
		//fs::remove(strSourcePath);
		if (exists(strSoure))
		{
			fs::copy(strSoure, strDest);
			fs::remove(strSoure);
		}
	}
	catch (std::exception& e)
	{
		cout << e.what();
	}
}

#include <map>
using namespace std;

map<string, string> mapBankCode;
bool LoadBankCode(string& strError)
{
	try
	{

		string strFile = "D:/Work/SSCard_System.MVC/MainProject/SSCard_System/debug/Data/BankCode.json";
		ifstream ifs(strFile, ios::in);
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObject json;
		auto tNow = chrono::high_resolution_clock::now();
		if (!json.Parse(ss.str()))
		{
			cout << "Invalid json file:" << strFile;
			return false;
		}
		if (!json.KeyExist("BankList"))
		{
			strError = "银行代码库文件被损坏!";
			return false;
		}
		CJsonObject jsonBankList = json["BankList"];
		if (!jsonBankList.IsArray())
		{
			strError = "银行代码库文件被损坏!";
			return false;
		}
		auto tDuration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - tNow);

		cout << "Parsar duration:" << tDuration.count() << endl;
		tNow = chrono::high_resolution_clock::now();
		int nArraySize = jsonBankList.GetArraySize();
		CJsonObject jsonItem;
		for (int i = 0; i < nArraySize; i++)
		{
			if (!jsonBankList.Get(i, jsonItem))
				break;
			string strCode = jsonItem("code");
			string strName = jsonItem("Name");
			//cout << "Code =" << strCode << "\t" << "Name = " << strName << endl;
			auto [it, Inserted] = mapBankCode.try_emplace(strCode, strName);
		}

		tDuration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - tNow);

		cout << "Parsar duration:" << tDuration.count() << endl;
		tNow = chrono::high_resolution_clock::now();
		//for (auto var : mapBankCode)
		//{
		//	cout << var.first << "-->" << var.second;
		//}
		return true;
	}
	catch (std::exception& e)
	{
		cout << "exception:" << e.what();
	}
	return false;
}
#include <process.h>
#include <windows.h>
void CreateFaceProcess(const char* szPath, const char* szCurrentDir)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		(LPSTR)szPath,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		szCurrentDir,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		cout << "CreateProcess failed (%d).\n" << GetLastError();
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void GetAge()
{
	time_t t1 = time(0);
	auto t2 = chrono::system_clock::from_time_t(time(0));
	auto t3 = chrono::system_clock::from_time_t(time(0));
	auto tDuration = chrono::duration_cast<chrono::years>(t2 - t3);
}

int main()
{
	string strBirthday = "19761216";
	int nYear, nMonth, nDay;
	int nCount = sscanf_s(strBirthday.c_str(), "%04d%02d%02d", &nYear, &nMonth, &nDay);
	time_t tNow = time(nullptr);
	tm* local_time = localtime(&tNow);

	tm tmBirday;
	tmBirday.tm_year = nYear - 1900;
	tmBirday.tm_mon = nMonth - 1;
	tmBirday.tm_mday = nDay;

	//CreateFaceProcess("D:\\Work\\CardPrinter\\FacePhoto\\MattingTool.exe", "D:\\Work\\CardPrinter\\FacePhoto");
	string strMessage;
	LoadBankCode(strMessage);
	std::string strBankNum = "6217974660008798073";
	std::string strChipNum = "6221" + strBankNum.substr(4);
	for (auto var : mapBankCode)
	{
		int nOffset = strBankNum.find(var.first);
		if (nOffset >= 0)
		{
			cout << var.first << "\t" << var.second;
		}
	}
	TestFileSystem();
	testMove_InitVec();
	TestUpateJson();
	return 0;
}
