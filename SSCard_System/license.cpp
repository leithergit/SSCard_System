
#include "license.h"

#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>  
#include <openssl/err.h>  
#include <openssl/bio.h>  
#include <memory>
#include <string>
#include "WMIQuery.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <list>
#include <string>
#include "Gloabal.h"
#include "Utility.h"
#pragma comment(lib,"libssl.lib")
#pragma comment(lib,"libcrypto.lib")

bool     g_bReadMachineid = false;
string   g_strMachineid;

extern "C"
{
#include <openssl/applink.c>
}

#define TimeSpan(t)		(time(NULL) - (time_t)t)

const char* g_szPublicKey = "-----BEGIN PUBLIC KEY-----\n"
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQClc+3UN7YBHTUN0M2xK1inyXg5\n"
"Yaxg3fJLL5DD5c8CibkuDmecPwb+WYQOQec3Pc502RoI4nFISxeWF04Yn0ELrPIf\n"
"sW6bW/Aw6DedAD2uFoyWvvH4PWVM6OoQUq1L7LIWFrXMewdZQJrlPAGnJoVVJEtS\n"
"Ogk7PIMvaSSU9swwbwIDAQAB\n"
"-----END PUBLIC KEY-----\n";

using namespace std;
struct DeviceInfo
{
	int		nFields;
	const	int nMaxFields;
	Declare_ClassName();
	DeviceInfo(int nMax) :nMaxFields(nMax)
	{
	}
	virtual void Reset()
	{
		nFields = 0;
	}
	bool IsReady()
	{
		return nFields == nMaxFields;
	}
	int operator ++(int)		// 操作符后置
	{
		int nTemp = nFields;
		nFields++;
		return nTemp;
	}

	int operator ++()	// 操作符前置
	{
		nFields++;
		return nFields;
	}
};

struct CPUInfo :public DeviceInfo
{
	string strVender;
	string strCPUID;
	Declare_ClassName();
	CPUInfo() :DeviceInfo(2) {};
};

struct BaseBoard :public DeviceInfo
{
	string strManufacturer;
	string strSerialNumber;
	Declare_ClassName();
	BaseBoard() :DeviceInfo(2) {};
	virtual void Reset()
	{
		gInfo() << __className() << "\tManufacturer = " << strManufacturer << "\tSerialNumber = " << strSerialNumber;
	}
};
struct HardDriveInfo :public DeviceInfo
{
	int		nIndex;
	int		nPartitioins;
	string  strModel;
	string	strDiskID;
	string	strInterfaceType;
	Declare_ClassName();
	HardDriveInfo() :DeviceInfo(4) {};
	virtual void Reset()
	{
		gInfo() << __className() << "\tModel = " << strModel << "\tInterfaceType = " << strInterfaceType << "\tDiskID = " << strDiskID;
	}
};

struct NetAdapter :public DeviceInfo
{
	string strVender;
	string strMacID;
	string strPNPDeviceID;
	NetAdapter() :DeviceInfo(3) {};
	Declare_ClassName();
	virtual void Reset()
	{
		gInfo() << __className() << "\tVender = " << strVender << "\tPNPDeviceID = " << strPNPDeviceID << "\tMacAddress = " << strMacID;
	}
	bool isPCI()
	{
		return strPNPDeviceID.find("PCI\\") != string::npos;
	}
	byte* GetMacID()
	{
		static byte szMacID[12] = { 0 };
		const char* pStr = strMacID.c_str();
		int nByte = 0;
		for (int i = 0; i < strMacID.size(); i++)
		{
			szMacID[nByte++] = (byte)strtolong((const char*)&pStr[i], 16, 2);

			i += 2;
		}
		return (byte*)szMacID;
	}
};

struct HardwareID
{
	list<CPUInfo> listCPU;
	BaseBoard MotherBoard;
	list<HardDriveInfo> listHDD;
	list<NetAdapter>listNetAdapter;
};

HardwareID g_HardwareID;

void __stdcall PrintVariant(wstring strName, VARIANT Value, DWORD_PTR pUserPtr)
{
	switch (Value.vt)
	{
	case VT_BSTR:
	{
		TraceMsgW(L"%s =%s\n", strName.c_str(), Value.bstrVal);
		break;
	}
	case VT_I1:
	case VT_I2:
	case VT_I4:
	case VT_I8:
	case VT_INT:
	{
		TraceMsgW(L"%s = 0x%08x\n", strName.c_str(), Value.intVal);
	}break;
	case VT_UI8:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
	case VT_UINT:
	{
		TraceMsgW(L"%s = 0x%08x\n", strName.c_str(), Value.intVal);
	}break;
	case VT_BOOL:
	{
		TraceMsgW(L"%s = %s\n", strName.c_str(), Value.boolVal ? L"TRUE" : L"FASLE");
	}break;
	case VT_NULL:
	{
		break;
	}
	case VT_BSTR | VT_ARRAY:
	{
		CComBSTR HUGEP* pBstr = nullptr;
		HRESULT hr = SafeArrayAccessData(Value.parray, (void HUGEP**) & pBstr);
		hr = SafeArrayUnaccessData(Value.parray);
		TraceMsgW(L"%s = %s\n", strName.c_str(), pBstr->m_str);
	}
	break;

	case VT_I4 | VT_ARRAY:
	{
		LONG low, high;
		BYTE HUGEP* pBuf;
		SafeArrayGetLBound(Value.parray, 1, &low);
		SafeArrayGetUBound(Value.parray, 1, &high);

		HRESULT hr = SafeArrayAccessData(Value.parray, (void HUGEP**) & pBuf);
		hr = SafeArrayUnaccessData(Value.parray);
		high = min(high, MAX_PATH * 2 - 1);
		//TCHAR szText[1024] = { 0 };
		//wprintf(L"%s", szText);
	}
	break;
	default:
	{
		break;
	};
	}
}


void __stdcall AccessValue(wstring strName, VARIANT Value, DWORD_PTR pUserPtr)
{
	//PrintVariant(strName, Value, pUserPtr);
	static CPUInfo mycpu;
	static NetAdapter NetInterface;
	static HardDriveInfo hdd;
	PrintVariant(strName, Value, pUserPtr);
	switch (pUserPtr)
	{
	case Win32_Processor:
	{
		if (strName == L"EnumerationFinished")
		{
			mycpu.Reset();
		}
		if (strName == L"Name")
		{
			mycpu.strVender = _AnsiString(Value.bstrVal, CP_ACP);
			mycpu++;
		}

		if (strName == L"ProcessorId")
		{
			mycpu.strCPUID = _AnsiString(Value.bstrVal, CP_ACP);
			mycpu++;
		}
		if (mycpu.IsReady())
		{
			g_HardwareID.listCPU.push_back(mycpu);

		}


		break;
	}

	case Win32_BaseBoard:
	{
		if (strName == L"EnumerationFinished")
		{
			g_HardwareID.MotherBoard.Reset();
		}
		if (strName == L"SerialNumber")
			g_HardwareID.MotherBoard.strSerialNumber = _AnsiString(Value.bstrVal, CP_ACP);

		else if (strName == L"Manufacturer")
			g_HardwareID.MotherBoard.strManufacturer = _AnsiString(Value.bstrVal, CP_ACP);
		break;
	}

	case Win32_DiskDrive:
	{
		//PrintVariant(strName, Value, pUserPtr);
		if (strName == L"EnumerationFinished")
		{
			if (hdd.nPartitioins >= 1)
			{
				g_HardwareID.listHDD.push_back(hdd);
				hdd.Reset();
			}

			hdd.nPartitioins = 0;
		}

		if (strName == L"Partitions")
		{
			hdd.nPartitioins = Value.intVal;
			hdd++;
		}

		if (strName == L"Index")
		{
			hdd.nIndex = Value.intVal;

		}

		if (strName == L"Model")
		{
			hdd.strModel = _AnsiString(Value.bstrVal, CP_ACP);
			hdd++;
		}

		if (strName == L"SerialNumber")
		{
			hdd.strDiskID = _AnsiString(Value.bstrVal, CP_ACP);
			hdd++;
		}
		if (strName == L"InterfaceType")
		{
			hdd.strInterfaceType = _AnsiString(Value.bstrVal, CP_ACP);
			hdd++;
		}

		break;
	}

	case  Win32_NetworkAdapter:
	{
		if (strName == L"EnumerationFinished")
		{
			if (NetInterface.isPCI())
			{
				g_HardwareID.listNetAdapter.push_back(NetInterface);
				NetInterface.Reset();
			}

		}
		if (strName == L"Name")
		{
			NetInterface.strVender = _AnsiString(Value.bstrVal, CP_ACP);
			NetInterface++;
		}

		if (strName == L"MACAddress")
		{
			NetInterface.strMacID = _AnsiString(Value.bstrVal, CP_ACP);
			NetInterface++;
		}
		if (strName == L"PNPDeviceID")
		{
			NetInterface.strPNPDeviceID = _AnsiString(Value.bstrVal, CP_ACP);
			NetInterface++;
		}

		break;
	}
	}
}

bool SetHardwareCode(std::string& strMachineCode)
{
	QString strAppPath = QCoreApplication::applicationDirPath();
	strAppPath += "/license.lic";

	QSettings LicenseSetting(strAppPath, QSettings::IniFormat);
	LicenseSetting.beginGroup("License");
	LicenseSetting.setValue("MachineCode", strMachineCode.c_str());
	LicenseSetting.endGroup();
	return true;
}

bool GetHardwareCode(std::string& strMachineCode)
{
	shared_ptr<CWMIQuery> pQuery = shared_ptr<CWMIQuery>(new CWMIQuery(L"root\\CIMV2"));
	if (FAILED(pQuery->Initialize()))
		return false;

	if (FAILED(pQuery->ConnectWMI()))
		return false;

	// 取处理器信息
	/*if (FAILED(pQuery->Query(L"SELECT Name,ProcessorId FROM Win32_Processor", AccessValue, Win32_Processor)))
		return false;*/

		// 取主板信息
		/*if (FAILED(pQuery->Query(L"SELECT Name,Manufacturer,SerialNumber FROM Win32_BaseBoard", AccessValue, Win32_BaseBoard)))
			return false;*/

			// 取硬盘信息 Index,Model,SerialNumber
	if (FAILED(pQuery->Query(L"SELECT * FROM Win32_DiskDrive", AccessValue, Win32_DiskDrive)))
		return false;

	// 取网卡信息
	if (FAILED(pQuery->Query(L"SELECT * from Win32_NetworkAdapter", AccessValue, Win32_NetworkAdapter)))
		return false;

	AES_KEY key;
	char szKey[32] = { 0 };
	strcpy(szKey, "Kingaotech(Shanghai)");		// 密钥
	AES_set_encrypt_key((const unsigned char*)szKey, 128, &key);

	// 生成机器码
	// 由网卡ID，第一块硬盘序列号，主板序列号，组成，理论上重复的机率较小
	char szHardWareID[128] = { 0 };
	int nSize = 0;
	// MacID为第一要素
	for (auto var : g_HardwareID.listNetAdapter)
	{
		memcpy(&szHardWareID[nSize], var.GetMacID(), 6);
		nSize += 6;
	}

	// 硬盘序列号
	for (auto var : g_HardwareID.listHDD)
	{
		if (var.strInterfaceType == "USB")
			continue;
		memcpy(&szHardWareID[nSize], var.strDiskID.c_str(), var.strDiskID.size());
		nSize += var.strDiskID.size();
		break;
	}

	// 主板序列号
	//memcpy(&szHardWareID[nSize], g_HardwareID.MotherBoard.strSerialNumber.c_str(), g_HardwareID.MotherBoard.strSerialNumber.size());
	//nSize += g_HardwareID.MotherBoard.strSerialNumber.size();

	AES_set_encrypt_key((const unsigned char*)szKey, 128, &key);
	char szEncryptHD_ID[36] = { 0 };
	// 加密
	AES_encrypt((const unsigned char*)szHardWareID, (unsigned char*)szEncryptHD_ID, &key);
	AES_encrypt((const unsigned char*)&szHardWareID[16], (unsigned char*)&szEncryptHD_ID[16], &key);

	char szMachineCode[128] = { 0 };
	Hex2AscStringA((unsigned char*)szEncryptHD_ID, 32, szMachineCode, 128, '\0');
	strMachineCode = std::string(szMachineCode, 64);
	return true;
}


std::string DecryptRSAKey(const char* szKey, const std::string& strData)
{
	if (strData.empty())
	{
		assert(false);
		return "";
	}
	std::string strRet;
	BIO* bio = NULL;
	RSA* pRSAPubKey = NULL;
	char* chPublicKey = const_cast<char*>(szKey);
	if ((bio = BIO_new_mem_buf(chPublicKey, -1)) == NULL)       //从字符串读取RSA公钥
	{
		return "";
	}
	pRSAPubKey = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);		//从bio结构中得到rsa结构
	if (!pRSAPubKey)
	{
		ERR_load_crypto_strings();
		char errBuf[512];
		ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
		BIO_free_all(bio);
		return "";
	}

	int nLen = RSA_size(pRSAPubKey);
	char* pDecode = new char[nLen + 1];

	int ret = RSA_public_decrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pDecode, pRSAPubKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		strRet = std::string((char*)pDecode, ret);
	}
	else
	{
		ERR_load_crypto_strings();
		char errBuf[512];
		ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
		BIO_free_all(bio);
	}
	delete[] pDecode;
	RSA_free(pRSAPubKey);
	CRYPTO_cleanup_all_ex_data();
	return strRet;
}

//对外接口，每次开启软件的时候都调用一下
bool CheckLocalLicense(DWORD dwSoftwareCode)
{
	string strHardwareCode;
	GetHardwareCode(strHardwareCode);
	SetHardwareCode(strHardwareCode);

	string strLicenseCode;
	if (!GetLocalLicense(strLicenseCode))
		return false;

	if (strLicenseCode.empty())
		return false;

	/*std::string strMachineCodeA = strHardwareCode;
	char szHardwareCodeBin[128] = { 0 };
	AscString2HexA((const char*)strMachineCodeA.c_str(), strMachineCodeA.size(), (unsigned char*)szHardwareCodeBin, 128, '\0');*/

	char szLicenseCodeA[512];
	strcpy(szLicenseCodeA, strLicenseCode.c_str());

	char szLicenseBinary[256] = { 0 };
	int nSize = AscString2HexA(szLicenseCodeA, strlen(szLicenseCodeA), (unsigned char*)szLicenseBinary, 256, '\0');//注册码转换
	std::string strLicenseCodeBinary = std::string(szLicenseBinary, nSize);

	// 使用公钥解密码，得到硬件码,进行比较
	std::string strHardwareCode2 = DecryptRSAKey(g_szPublicKey, strLicenseCodeBinary);
	char szAscHardwareCode2[256] = { 0 };
	Hex2AscStringA((unsigned char*)strHardwareCode2.c_str(), strHardwareCode2.size(), szAscHardwareCode2, 256, '\0');
	return (strHardwareCode.compare(szAscHardwareCode2) == 0);
}

bool GetLocalLicense(string& strLicense)
{
	//[license]
	//MachineCode=11223334445555555
	//RegisterCode=
	QString strAppPath = QCoreApplication::applicationDirPath();
	strAppPath += "/license.lic";
	QFileInfo fi(strAppPath);
	if (fi.isFile())
	{
		QSettings LicenseSetting(strAppPath, QSettings::IniFormat);
		LicenseSetting.beginGroup("License");
		strLicense = LicenseSetting.value("RegisterCode").toString().toStdString();
		LicenseSetting.endGroup();
		return true;
	}
	else
		return false;
}
