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

bool CheckLocalLicense(DWORD dwSoftCode);

bool GetLocalLicense(string& strLicense);

bool SetHardwareCode(std::string& strMachineCode);

bool GetHardwareCode(std::string& strMachineCode);

#endif // MAGOLICENSE_H
