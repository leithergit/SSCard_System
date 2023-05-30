// SampleCode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include<filesystem>
#include "../utility/Markup.h"
#include "../utility/json/CJsonObject.hpp"
#include "SimpleIni.h"
#include "../SDK/QREncode/qrencode.h"
#pragma comment(lib, "../SDK/QREncode/qrencoded")
using namespace std;
using namespace neb;
using namespace std::filesystem;
void TestSimpleINI()
{
	CSimpleIniA ReadINI;
	auto nResult = ReadINI.LoadFile("./Configure2.ini");
	if (nResult != SI_OK)
	{
		cout << "Faild in load configure.ini" << endl;
		return;
	}
	string strPrinter = ReadINI.GetValue("Device", "Printer");
	string strUpgradeCode = ReadINI.GetValue("Region", "UpgradeCode");
	string strSSCardReadType = ReadINI.GetValue("Device", "SSCardReadType");
	

	CSimpleIniA::TNamesDepend keys;

	ReadINI.GetAllKeys("Bank", keys);

	for (auto var : keys)
		cout << "Key = " << var.pItem << endl;

	cout << "Printer = " << strPrinter << "\nUpgradeCode" << strUpgradeCode << "\nSSCardReadType" << strSSCardReadType << endl;
	strPrinter = "Zenius KC200B";
	strUpgradeCode = "400001";
	strSSCardReadType = "DC_READ2";
	ReadINI.SetValue("Device", "Printer", strPrinter.c_str());
	ReadINI.SetValue("Region", "UpgradeCode", strUpgradeCode.c_str());
	ReadINI.SetValue("Device", "SSCardReadType", strSSCardReadType.c_str());
	ReadINI.SetBoolValue("TestSection", "BoolValue1", true);
	ReadINI.SetBoolValue("TestSection", "BoolValue2", false);
	ReadINI.SaveFile("./Configure2.ini");
}

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

void QREncode()
{
	QRcode* qr = QRcode_encodeString("1234567890abcdefgh", 1, QR_ECLEVEL_Q, QR_MODE_KANJI, 1);
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
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iterator>
#include <time.h>
void TestStringPerformance()
{
	std::ostringstream os;
	std::ifstream file1("d:/20220704-094310.3564.log");
	std::ifstream file2("d:/20220704-094310.3565.log");
	std::ifstream file3("d:/20220704-094310.3566.log");
	std::ifstream file4("d:/20220704-094310.3567.log");

	clock_t start1 = clock();
	os << file1.rdbuf();
	std::string s = os.str();
	clock_t stop1 = clock();
	std::cout << "s.length() = " << s.length() << endl;

	std::string s2;
	clock_t start2 = clock();
	file2.seekg(0, std::ios_base::end);
	const std::streampos pos = file2.tellg();
	file2.seekg(0, std::ios_base::beg);
	if (pos != std::streampos(-1))
		s2.reserve(static_cast<std::string::size_type>(pos));
	s2.assign(std::istream_iterator<char>(file2), std::istream_iterator<char>());
	clock_t stop2 = clock();
	std::cout << "s2.length = " << s2.length() << endl;


	std::string s3;
	clock_t start3 = clock();
	file3.seekg(0, std::ios::end);
	s3.reserve(file3.tellg());
	file3.seekg(0, std::ios::beg);
	s3.assign((std::istreambuf_iterator<char>(file3)),
		std::istreambuf_iterator<char>());
	clock_t stop3 = clock();
	std::cout << "s3.length = " << s3.length() << endl;

	// New Test
	std::string s4;

	clock_t start4 = clock();
	file4.seekg(0, std::ios::end);
	s4.resize(file4.tellg());
	file4.seekg(0, std::ios::beg);
	file4.read(&s4[0], s4.length());
	clock_t stop4 = clock();

	std::cout << "s4.length = " << s4.length() << endl;

	std::cout << "Time using rdbuf:" << stop1 - start1 << endl;
	std::cout << "Time using istream_iterator : " << stop2 - start2 << endl;
	std::cout << "Time using istreambuf_iterator : " << stop3 - start3 << endl;
	std::cout << "Time using read : " << stop4 - start4 << endl;

}

void ThreadProc()
{
	return;
}

string&& ReturnString()
{
	string strT = "This s a string for test!";
	return std::move(strT);
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

void TestUpateJson2()
{
	CJsonObject json;
	json.AddEmptySubArray("Array");
	json["Array"].Add("String 1");
	json["Array"].Add("String B");
	json["Array"].Add("String iii");
	int nCount = json["Array"].GetArraySize();
	cout << json.ToFormattedString() << endl;
	for (int i = 0; i < nCount; i++)
		cout << "Array[" << i << "] = " << json["Array"](i) << endl;

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
#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
using namespace Gdiplus;

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

class A1
{
	char* szName = nullptr;
public:
	A1()
	{
		cout << "construct 1" << endl;
	}
	A1(const char* szIn)
	{
		cout << "construct 2" << endl;
		szName = new char[strlen(szIn) + 1];
		strcpy(szName, szIn);
	}
	A1(const A1& a)
	{
		cout << "copy construct " << endl;
		szName = new char[strlen(a.szName) + 1];
		strcpy(szName, a.szName);
	}
	A1(A1&& a) :szName(a.szName)
	{
		cout << "move construct " << endl;
		a.szName = nullptr;
	}
	~A1()
	{
		cout << "deconstruct " << endl;
		if (szName)
			delete[]szName;
	}
};

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

A1 GetTempClassA()
{
	A1 T("Test Return Class!");
	return T;
}

void TestStrok()
{
	char szCardInfo[1024] = { 0 };
	strcpy_s(szCardInfo, 1024, "姓名|社会保障号码|社保卡号|发卡日期");

	char* szNextToken = nullptr;
	char* pToken = strtok_s((char*)szCardInfo, "|", &szNextToken);

	while (pToken)
	{
		cout << "pToken = " << pToken << endl;
		pToken = strtok_s(NULL, "|", &szNextToken);
	}
}



int main()
{
	QREncode();
	/*TestSimpleINI();
	TestStrok();
	int A = 1;
	int B;
	int C = (B = A);
	cout << A << B << C << endl;
	A1 T = GetTempClassA();
	TestUpateJson2();


	string strReturned = ReturnString();
	cout << strReturned << endl;*/
	string strT(1024, 0);
	int nCapcity = strT.capacity();
	strT.at(0) = '1';
	nCapcity = strT.capacity();
	int nSize = strT.size();
	strT = "0123456789";
	nCapcity = strT.capacity();
	nSize = strT.size();
// 	for (int i = 0; i < 1024; i++)
// 	{
// 		strT += strT;
// 		cout << "Size = " << strT.size() << " " << "Capcity = " << strT.capacity() << endl;
// 	}
	strT.resize(10);
	strcpy(strT.data(), "0123456789");
	TestStringPerformance();
	
	auto tNow = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	stringstream Datetime;

	Datetime << std::put_time(std::localtime(&tNow), "%Y%m%d%H%M%S");


	char* pBuffer = strT.data();
	for (int i = 0; i < strT.size(); i++)
		pBuffer[i] = 0x30 + i % 10;
	string strT2 = std::move(strT);
	//GdiplusStartupInput gdiplusStartupInput;
	//ULONG_PTR gdiplusToken;
	//GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//Image* image = new Image(L"FakePhoto.jpg");
	//CLSID encoderClsID;


	//GdiplusShutdown(gdiplusToken);

	string strBirthday = "19761216";
	int nYear, nMonth, nDay;
	int nCount = sscanf_s(strBirthday.c_str(), "%04d%02d%02d", &nYear, &nMonth, &nDay);
	tNow = time(nullptr);
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
