// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "inc/GpuUtilizationInfo.h"
#include "inc/DebugUtil.h"
#include "inc/UserPrivilegeUtil.h"


#pragma comment(lib, "gdi32.lib")  
#pragma comment(lib, "advapi32.lib")



class CGpuHelper
{
public:

	CGpuHelper() : _current_usage(0.0)
	{
	}

	~CGpuHelper()
	{
	}

	static BOOL CheckDiscrete(const wchar_t * LowPart, const wchar_t * HighPart)
	{
		vector<wstring> vObjects;
		BOOL bRet = CPdhMoniter::GetPdhObjectBuffer(L"GPU Non Local Adapter Memory", vObjects);
		if (bRet)
		{
			for (auto item : vObjects)
			{
				DBG_PRINTF_FL((L"CheckDiscrete item=%s\n inHigh=%s inLow=%s",
					item.c_str(), HighPart, LowPart));
				wstring outLow;
				wstring outHigh;
				if (CPdhMoniter::GetLuidFromObject(item, outLow, outHigh))
				{					
					if (outLow == LowPart && outHigh == HighPart)
						return TRUE;
				}
			}
		}
		return FALSE;
	}

	static wstring GetEngineName(const wstring & instanceName, const wstring & counterName)
	{
		UNREFERENCED_PARAMETER(counterName);
		CString strLuid = instanceName.c_str();
		strLuid = strLuid.MakeLower();
		return strLuid.Mid(strLuid.Find(L"eng")).GetBuffer();
	}

	void GetProcessUtilization(DWORD proId, const wchar_t* highPart, const wchar_t* lowPart, DWORD & utilization)
	{	
		CPdhMoniter pdhMoniter;
		double maxValue=0;
		CString strGUID;
		strGUID.Format(L"pid_%d_luid_%s_%s", proId, highPart, lowPart);
		pdhMoniter.ForEachInstance(L"GPU Engine", strGUID.GetBuffer(), { L"Utilization Percentage" },
			&GetEngineName, [&](wstring & keyName, double & value)
		{
			UNREFERENCED_PARAMETER(keyName);		
			if (value > maxValue)
				maxValue = value;				
		});
	
		utilization = (DWORD)maxValue;
	}

	static wstring CreateDisplayCardMemKeyName(const wstring & instanceName, const wstring & counterName)
	{
		size_t start = instanceName.find(L"pid_");
		if (start != instanceName.npos)
		{
			wstring keyname = instanceName.substr(start + 4);
			keyname = keyname.substr(0, keyname.find(L"_"));
			if (counterName == L"Dedicated Usage")
				return (keyname + L"_Dedicated");

			if (counterName == L"Shared Usage")
				return (keyname + L"_Shared");
		}
		return L"";
	}

	void GetDisplayCardMem(DWORD proId, const wchar_t* highPart, const wchar_t* lowPart, long &dedicatedUsage, long &sharedUsage)
	{
		CPdhMoniter pdhMoniter;
		CString strGUID;
		strGUID.Format(L"pid_%d_luid_%s_%s", proId, highPart, lowPart);
		pdhMoniter.ForEachInstance(L"GPU Process Memory", strGUID.GetBuffer(), { L"Dedicated Usage", L"Shared Usage" },
			&CreateDisplayCardMemKeyName, [&](wstring & keyName, double & value)
		{
			UNREFERENCED_PARAMETER(keyName);
			if (keyName.find(L"_Dedicated") != keyName.npos)
				dedicatedUsage = (long)(value / 1024 / 1024);
			if (keyName.find(L"_Shared") != keyName.npos)
				sharedUsage = (long)(value / 1024 / 1024);
			
		});
	}

	float GetMaxEngine(std::map<wstring, double> gupInfo)
	{
		auto ite = std::max_element(gupInfo.begin(), gupInfo.end(),
			[](std::pair<wstring, double> val1, std::pair<wstring, double> val2)->bool {
			if (val1.second < val2.second)
			{
				return true;
			}
			return false;
		});
		return (float)ite->second;
	}

	float GetGpuUtilization()
	{
		return _current_usage;
	};

	void ClearUtilization()
	{
		_current_usage = 0.0;
	}

	DWORD UpdateGPUUtilization( wstring gpu_engine, const vector<ULONG> & proclist)
	{
		CPdhMoniter pdhMoniter;
		double utilization = 0;

		for (auto procid : proclist)
		{
			if (procid == 0)
				continue;

			CString process_utili_counter;
			process_utili_counter.Format(L"pid_%d_%s", procid, gpu_engine.c_str());

			CString querystr;
			querystr.Format(L"\\%s(%s)\\%s", L"GPU Engine", process_utili_counter.GetBuffer(), L"Utilization Percentage");		

			PDH_FUNCTION ret = PdhValidatePath(querystr.GetBuffer());
			if (ret != ERROR_SUCCESS)
			{
				DBG_PRINTF_FL((L"CGpuHelper::UpdateGPUUtilization PdhValidatePath fail\n"));
				continue;
			}

			pdhMoniter.AddCounter(querystr.GetBuffer(), process_utili_counter, L"");
		}

		if (pdhMoniter.GetSize() > 0)
		{
			pdhMoniter.SetQueryTime();

			if (WAIT_OBJECT_0 == pdhMoniter.Wait())
			{
				pdhMoniter.ForEach([&](wstring & keyName, double & value)
				{
					if (value > 0.0)
						DBG_PRINTF_FL((L"GPUManager::CallBack querystr %s value %d\n", keyName.c_str(), (int)value));
					UNREFERENCED_PARAMETER(keyName);
					utilization += value;
				});
			}
		}

		_current_usage = (float)utilization;
		return (DWORD)_current_usage;
	}


private:
	float _current_usage;

};


