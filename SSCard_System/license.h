#ifndef MAGOLICENSE_H
#define MAGOLICENSE_H

#include <QDialog>
#include "../utility/Utility.h"
#include "qsettings.h"
#include <process.h>
#include <TCHAR.H>
#include <string>
using namespace std;

#define Code_License		1048577

// 检查授权统一接口
bool CheckLocalLicense(DWORD dwSoftCode);

// 基于于硬件ID的授权检查
bool CheckLocalLicense1(DWORD dwSoftCode);

bool GetLocalLicense(string& strLicense);

bool SetHardwareCode(std::string& strMachineCode);

bool GetHardwareCode1(std::string& strMachineCode);

// 基于打印机序列号的授权检查
bool CheckLocalLicense2(DWORD dwSoftCode);
bool GetHardwareCode2(std::string& strMachineCode);

#endif // MAGOLICENSE_H
