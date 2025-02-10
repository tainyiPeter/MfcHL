// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <atlstr.h>
#include <string>
#include <windows.h>  
#include <stdio.h>  
#include <pdh.h>  
#include <pdhmsg.h>  
#include<tchar.h>  
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include "DebugUtil.h"
#include "TemplateUtil.h"
#pragma comment(lib, "pdh.lib")  

using namespace std;

typedef struct GPU_USG_INFO
{
	GPU_USG_INFO()
	{
		StrName = L"";
		gputotalusage = 0.0;
	}
	CString StrName;
	std::vector<HCOUNTER> VecHCount;
	float gputotalusage;
}GUInfo;

class CPdhMoniter
{
public:
	static BOOL GetPdhObjectBuffer(const wchar_t * objectName, vector<wstring> &vObjects, const wchar_t * findLuid = nullptr)
	{
		PDH_STATUS  pdhStatus = ERROR_SUCCESS;
		LPTSTR      szCounterListBuffer = NULL;
		DWORD       dwCounterListSize = 0;
		LPTSTR      szInstanceListBuffer = NULL;
		DWORD       dwInstanceListSize = 0;

		// Determine the required buffer size for the data. 
		pdhStatus = PdhEnumObjectItems(
			NULL,                   // real time source
			NULL,                   // local machine
			objectName,        // object to enumerate
			szCounterListBuffer,    // pass NULL and 0
			&dwCounterListSize,     // to get length required
			szInstanceListBuffer,   // buffer size 
			&dwInstanceListSize,    // 
			PERF_DETAIL_WIZARD,     // counter detail level
			0);

		if (pdhStatus == PDH_MORE_DATA)
		{
			// Allocate the buffers and try the call again.
			szCounterListBuffer = (LPTSTR)malloc(
				(dwCounterListSize * sizeof(TCHAR)));
			szInstanceListBuffer = (LPTSTR)malloc(
				(dwInstanceListSize * sizeof(TCHAR)));

			if ((szCounterListBuffer != NULL) &&
				(szInstanceListBuffer != NULL))
			{
				pdhStatus = PdhEnumObjectItems(
					NULL,                 // real time source
					NULL,                 // local machine
					objectName,      // object to enumerate
					szCounterListBuffer,  // buffer to receive counter list
					&dwCounterListSize,
					szInstanceListBuffer, // buffer to receive instance list 
					&dwInstanceListSize,
					PERF_DETAIL_WIZARD,   // counter detail level
					0);

				if (pdhStatus == ERROR_SUCCESS)
				{
					for (LPTSTR szThisInstance = szInstanceListBuffer;
						*szThisInstance != 0;
						szThisInstance += wcsnlen_s(szThisInstance, dwInstanceListSize) + 1)
					{
						if (findLuid == nullptr)
							vObjects.push_back(szThisInstance);
						else
						{
							if (FindWithoutCase(szThisInstance, findLuid))
								vObjects.push_back(szThisInstance);
						}
					}
				}
			}
		}

		if (szCounterListBuffer)
			free(szCounterListBuffer);
		if (szInstanceListBuffer)
			free(szInstanceListBuffer);

		return vObjects.size() > 0;
	}

	static BOOL GetLuidFromObject(wstring & object, wstring &LowPart, wstring &HighPart)
	{
		std::vector<wstring> vSplit = SplitString(TransformCase(object), L"_", L"luid");
		
		if (vSplit.size() > 0)
		{
			if (vSplit.size() > 3)
			{
				HighPart = vSplit[1].c_str();
				LowPart = vSplit[2].c_str();
				return TRUE;
			}
		}

		return FALSE;
	}

	BOOL AddCounter(const wchar_t * queryString, const wchar_t * keyName, const wchar_t * instanceName)
	{
		if (mQuery == NULL)
		{
			if (PdhOpenQuery(NULL, NULL, &mQuery) != ERROR_SUCCESS)
				return FALSE;
		}

		HCOUNTER counter;
		if (PdhAddEnglishCounter(mQuery, queryString, NULL, &counter) != ERROR_SUCCESS)
			return FALSE;

		mCounterMap[counter].keyName = keyName;
		mCounterMap[counter].instanceName = instanceName;
		return TRUE;
	}

	BOOL SetQueryTime(DWORD time = 1)
	{
		if (mEvent == NULL)
		{
			PdhCollectQueryData(mQuery);
			if (time > 1)
			{
				mEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("MyEvent"));
				PdhCollectQueryDataEx(mQuery, time, mEvent);
			}
		}

		return TRUE;
	}

	DWORD Wait(DWORD dwTimeout = INFINITE)
	{
		if (mEvent != NULL)
			return WaitForSingleObject(mEvent, dwTimeout);
		else
		{
			Sleep(1500);
			PdhCollectQueryData(mQuery);
			return WAIT_OBJECT_0;
		}
		return WAIT_FAILED;
	}

	template<typename Fn>
	void ForEach(Fn && f)
	{
		for (auto & item : mCounterMap)
		{
			PDH_FMT_COUNTERVALUE pdhValue;
			DWORD dwCounterType = 0;
			if (PdhGetFormattedCounterValue(item.first, PDH_FMT_DOUBLE, &dwCounterType, &pdhValue) == ERROR_SUCCESS)
			{
				f(item.second.keyName, pdhValue.doubleValue);
			}
		}
	}

	static void GetAllEngines(const wchar_t* highPart, const wchar_t* lowPart, std::set<wstring> &engs)
	{
		CPdhMoniter pdhMoniter;
		vector<wstring> vObjects;
		CString strGUID;
		strGUID.Format(L"luid_%s_%s", highPart, lowPart);
		pdhMoniter.GetPdhObjectBuffer(L"GPU Engine", vObjects, strGUID.GetBuffer());

		for (auto item : vObjects)
		{
			engs.insert(item.substr(item.find(L"luid_")));
		}
	}

	template<typename Fn>
	void ForEachInstance(const wchar_t * objectName, const wchar_t * findString, vector<wstring> childPaths,
		std::function<wstring(const wstring &, const wstring &)> setKeyName, Fn && f)
	{
		vector<wstring> vObjects;
		GetPdhObjectBuffer(objectName, vObjects, findString);
		for (auto item : vObjects)
		{
			if (_wcsnicmp(item.c_str(), L"_Total", array_size<decltype(L"_Total")>::value) != 0)
			{
				for (auto path : childPaths)
				{
					CString querystr;
					querystr.Format(L"\\%s(%s)\\%s", objectName, item.c_str(), path.c_str());
					AddCounter(querystr.GetBuffer(), setKeyName(item, path).c_str(), item.c_str());
				}
			}
		}

		if (GetSize() > 0)
		{
			// for sample
			SetQueryTime();
			if (Wait() == WAIT_OBJECT_0)
			{
				PDH_STATUS ret = PDH_INVALID_HANDLE;
				for (auto & item : mCounterMap)
				{
					PDH_FMT_COUNTERVALUE pdhValue;
					DWORD dwCounterType = 0;
					ret = PdhGetFormattedCounterValue(item.first, PDH_FMT_DOUBLE, &dwCounterType, &pdhValue);
					if (ret == ERROR_SUCCESS)
						f(item.second.keyName, pdhValue.doubleValue);
					else
					{
						DBG_PRINTF_FL((L"PdhGetFormattedCounterValue return false, findString=%s instance=%s\n",
							findString, item.second.instanceName.c_str()));
					}
				}
			}
		}
	}

	void Release()
	{
		if (mQuery != NULL)
		{
			if (mEvent != NULL)
			{
				SetEvent(mEvent);
				CloseHandle(mEvent);
				mEvent = NULL;
			}
			PdhCloseQuery(mQuery);
			mQuery = NULL;
			mCounterMap.clear();
		}
	}

	DWORD GetSize()
	{
		return (DWORD)mCounterMap.size();
	}

	CPdhMoniter() :mQuery(NULL), mEvent(NULL) {}
	~CPdhMoniter()
	{
		Release();
	}

private:
	struct InstanceInfo
	{
		wstring keyName;
		wstring instanceName;
	};
	HQUERY mQuery;
	std::map<HCOUNTER, InstanceInfo> mCounterMap;
	HANDLE mEvent;
};