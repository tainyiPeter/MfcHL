// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "windows.h"
#include "wininfo.h"
#include <pdh.h>  
#include <pdhmsg.h>  
//#include<tchar.h>  
#pragma comment(lib, "pdh.lib") 

class CCpuUsage
{
public:

	CCpuUsage()
	{
		m_cpureadevent = NULL;
		m_monitorflag = 0;
		m_CpuUsage = 0.0f;
		CWindowsInfo().GetWindowsVersion(WindowsVersion);
	}

	~CCpuUsage()
	{
		m_monitorflag = 0;
	}


private:
	ULONG WindowsVersion;
	float m_CpuUsage;
	HANDLE m_cpureadevent;
	int m_monitorflag;


public:



	static DWORD WINAPI _ThreadProc_GetCpuUsage(LPVOID lParam)
	{
		CCpuUsage* CpuUsageInfo = (CCpuUsage*)lParam;
		ULONG timeoutcount = 0;
		do
		{
			DWORD ret = WaitForSingleObject(CpuUsageInfo->m_cpureadevent, 1000);
			if (CpuUsageInfo->m_monitorflag == 1)
				break;
			if (ret == WAIT_TIMEOUT)
			{
				timeoutcount++;
				if (timeoutcount >= 10)
					break;
			}
			else
				timeoutcount = 0;

			{

				double  fvalue = 0.0;
				HQUERY query;
				PDH_STATUS status = PdhOpenQuery(NULL, NULL, &query);

				if (status != ERROR_SUCCESS)
					DBG_PRINTF_FL((L"Open Query Error"));

				HCOUNTER  counter = NULL;

				//counter = (HCOUNTER)GlobalAlloc(GPTR, sizeof(HCOUNTER));

				status = PdhAddCounter(query, L"\\Processor Information(_Total)\\% Processor Utility", NULL, &counter);

				if (status != ERROR_SUCCESS)
					DBG_PRINTF_FL((L"Add Counter Error"));

				PdhCollectQueryData(query);

				Sleep(1000);

				PdhCollectQueryData(query);

				PDH_FMT_COUNTERVALUE pdhValue;
				DWORD dwValue;

				status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
				if (status == ERROR_SUCCESS)
					fvalue = pdhValue.doubleValue;

				PdhCloseQuery(query);

				if (fvalue < 0)
					fvalue = 0.0f;
				if (fvalue > 100)
					fvalue = 100.0f;

				WCHAR  wTemp[13];
				swprintf_s(wTemp, 13, _T("%.lf"), fvalue);
				DBG_PRINTF_FL((L"Cpu load -------------%f\n", fvalue));
				CpuUsageInfo->m_CpuUsage = (float)fvalue;
			}

		} while (1);

		CloseHandle(CpuUsageInfo->m_cpureadevent);
		CpuUsageInfo->m_cpureadevent = NULL;
		CpuUsageInfo->m_monitorflag = 2;
		return 0;
	}


	void GetCpuUsage(float & nCpuUsepercentage)
	{
		DWORD ThreadId;  //Create a new thread for monitoring

		nCpuUsepercentage = m_CpuUsage;
		HANDLE  handle;
		if (m_cpureadevent == NULL)
		{
			m_cpureadevent = CreateEvent(NULL, FALSE, FALSE, NULL);
			handle = CreateThread(NULL, 0, _ThreadProc_GetCpuUsage, this, 0, &ThreadId);
		}
		else
			SetEvent(m_cpureadevent);
	}

	void StopMonitor()
	{
		m_monitorflag = 1;
		if (m_cpureadevent)
			SetEvent(m_cpureadevent);
		int i = 0;
		do
		{
			Sleep(200);
			if (m_monitorflag == 2)
				break;

		} while (i++<6);
	}
};


