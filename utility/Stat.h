#pragma once
#include <windows.h>
#include "Utility.h"
#include "TimeUtility.h"
#include "CriticalSectionAgent.h"
#include "AutoLock.h"
#include <list>
using namespace std;
class CStat
{
	CCriticalSectionAgent csList;
	list<float> listData;	
	float	fTotalValue;	// 所有时间和
	float	fAvgValue;		// 平均时间
	float   fMax, fMin;
	int		nObjIndex;
	char    szStatName[128];
	int		nMaxSize;
	FILE	*fp;
	
	CStat()
	{
		ZeroMemory(&fTotalValue, sizeof(CStat) - sizeof(listData));
	}
public:
	CStat(char *szName, int nIndex, int nMaxSize = 200)
	{
		/*ZeroMemory(&fTotalValue, sizeof(CStat) - sizeof(listData));*/
		nObjIndex = nIndex;
		strcpy(szStatName, szName);
		this->nMaxSize = nMaxSize;
		fTotalValue = 0.0f;
		fAvgValue = 0.0f;
		fMax = 0.0f; 
		fMin = 0.0f;
		fp = nullptr;

	}
	CStat(int nIndex, int nMaxSize = 200)
	{
		/*ZeroMemory(&fTotalValue, sizeof(CStat) - sizeof(listData));*/
		nObjIndex = nIndex;
		this->nMaxSize = nMaxSize;
		strcpy(szStatName, "TimeStat");
		fTotalValue = 0.0f;
		fAvgValue = 0.0f;
		fMax = 0.0f; 
		fMin = 0.0f;
		fp = NULL;
	}
	~CStat()
	{
		if (listData.size())
			OutputCSV();
		fclose(fp);
	}
	const float GetAvgValue()
	{
		return fAvgValue;
	}
	void Stat(float fValue)
	{
		CAutoLock lock(csList.Get());
		if (listData.size() < nMaxSize)
		{
			listData.push_back(fValue);
			fTotalValue += fValue;
			if (fMax < fValue)
				fMax = fValue;
			if (fMin == 0.0f ||
				fMin > fValue)
				fMin = fValue;
		}
	}

	bool IsFull()
	{
		CAutoLock lock(csList.Get());
		return (listData.size() >= nMaxSize);
	}

	void Reset()
	{
		fTotalValue = 0.0f;
		fAvgValue = 0.0f;
		fMax = 0.0f;
		fMin = 0.0f;
		CAutoLock lock(csList.Get());
		listData.clear();
	}

	void OutputStat(double dfOuputcondition = 0.0f)
	{
		fAvgValue = fTotalValue / listData.size();
		if (!GetStdHandle(STD_OUTPUT_HANDLE))
			return;
		CAutoLock lock(csList.Get());
		int nListSize = listData.size();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf("%s Obj(%d)(%s)\t nQueueSize = %d", __FUNCTION__, nObjIndex, szStatName, nListSize);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf("\tMaxValue = %.3f\tMinValue=%.3f", fMax, fMin);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		printf("\tAvgValue = %.3f", fAvgValue);
		
		char szText[1024*16] = { 0 };
		double dfDiffSum = 0.0f;
		int i = 0;
		for (auto it = listData.begin() ;it != listData.end(); i++,it ++)
		if (dfOuputcondition == 0.0f)
		{
			dfDiffSum += abs(*it - fAvgValue);
			sprintf(&szText[strlen(szText)], "[%03d] = %.03f\t", i, *it);
			if ((i+1) % 10 == 0 )
				strcat(szText, "\n\t");
		}
		else
		{
			if (*it < dfOuputcondition)
				continue;
			sprintf(&szText[strlen(szText)], "[%03d] = %.03f\t", i, *it);
		}
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
		printf("\tAvg Time diff = %.3f.\n\t", dfDiffSum / nListSize);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		
		strcat(szText, "\r\n");
		printf(szText);
	}

	void OutputCSV(double dfOuputcondition = 0.0f)
	{
		char szFileName[256] = { 0 };
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		sprintf_s(szFileName, 256, "%s_%04d%02d%02d_%02d%02d%02d.csv",szStatName,sysTime.wYear,sysTime.wMonth,sysTime.wDay,
			sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
		if (!fp)
		{
			fp = fopen(szFileName, "wb");
		}
		CAutoLock lock(csList.Get());
		int nListSize = listData.size();
		fAvgValue = fTotalValue / nListSize;
		int nBufferSize = 64 * 1024;
		char szText[1024 * 64] = { 0 };
		char szHeader[4096] = { 0 };
		double dfDiffSum = 0.0f;
		int i = 0;
		for (auto it = listData.begin(); it != listData.end(); i++, it++)
			if (dfOuputcondition == 0.0f)
			{
				dfDiffSum += abs(*it - fAvgValue);
				sprintf(&szText[strlen(szText)], "%d,", (int)(*it*1000));
				if ((i + 1) % 25 == 0)
					strcat(szText, "\n");
			}
			else
			{
				if (*it < dfOuputcondition)
					continue;
				sprintf(&szText[strlen(szText)], "%d,", (int)(*it * 1000));
			}

		sprintf_s(szHeader,4096,"StatName,DateTime,nQueueSize,MaxValue,MinValue,AvgValue,Avg Timediff\n");
		int len = strlen(szHeader);
		char szDateTime[32] = { 0 };
		GetDateTimeA(szDateTime, 32);
		sprintf_s(&szHeader[len], 4096 - len, "%s,%s,%d,%d,%d,%d,%d\n", szStatName,szDateTime, nListSize, (int)(fMax * 1000), (int)(fMin * 1000), (int)(fAvgValue * 1000), (int)((dfDiffSum * 1000) / nListSize));
		fprintf(fp, szHeader);
		fprintf(fp, "%s\n",szText);
	}
};

