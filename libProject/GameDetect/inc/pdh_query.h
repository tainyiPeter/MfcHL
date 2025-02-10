// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <string>
#include <vector>
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")

#include "inc/TimeOutThread.hpp"

class pdh_object
{
public:
	static std::vector<std::wstring> get_pdh_instances_by_object_name(const std::wstring &object_name)
	{
		PDH_STATUS  pdhStatus = ERROR_SUCCESS;
		LPTSTR      szCounterListBuffer = NULL;
		DWORD       dwCounterListSize = 0;
		LPTSTR      szInstanceListBuffer = NULL;
		DWORD       dwInstanceListSize = 0;
		LPTSTR      szThisInstance = NULL;
		std::vector<std::wstring> instances{};
		BOOL		result = FALSE;

		// Determine the required buffer size for the data. 
		pdhStatus = PdhEnumObjectItems(
			NULL,                   // real time source
			NULL,                   // local machine
			object_name.c_str(),        // object to enumerate
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
					object_name.c_str(),      // object to enumerate
					szCounterListBuffer,  // buffer to receive counter list
					&dwCounterListSize,
					szInstanceListBuffer, // buffer to receive instance list 
					&dwInstanceListSize,
					PERF_DETAIL_WIZARD,   // counter detail level
					0);

				if (pdhStatus == ERROR_SUCCESS)
				{
					// Walk the instance list. The list can contain one
					// or more null-terminated strings. The last string 
					// is followed by a second null-terminator.
					for (LPTSTR szThisInstance = szInstanceListBuffer;
						*szThisInstance != 0;
						szThisInstance += wcsnlen_s(szThisInstance, dwInstanceListSize) + 1)
					{
						instances.emplace_back(szThisInstance);
					}
					result = TRUE;
				}
			}
		}

		if (szCounterListBuffer)
			free(szCounterListBuffer);
		if (szInstanceListBuffer)
			free(szInstanceListBuffer);

		return instances;
	}
};


class pdh_query
{
public:
	pdh_query()
	{
		PDH_STATUS status = PdhOpenQuery(NULL, NULL, &h_query_);
		if (status != ERROR_SUCCESS)
			throw std::system_error(status, std::system_category(), "can not pdhOpenQuery failed");
	}
	pdh_query(const std::wstring &counter_path)
	{
		PDH_STATUS status = PdhOpenQuery(NULL, NULL, &h_query_);
		if (status != ERROR_SUCCESS)
			throw std::system_error(status, std::system_category(), "can not pdhOpenQuery failed");

		add_counter(counter_path);
	}
	~pdh_query()
	{
		PdhCloseQuery(h_query_);
	}
	bool add_counter(const std::wstring &counter_path)
	{
		PDH_STATUS status = PdhAddEnglishCounter(h_query_, counter_path.c_str(), NULL, &h_counter_);
		if (status != ERROR_SUCCESS)
			return false;
		return true;
	}
	std::unordered_map<std::wstring,double> get_values()
	{
		std::unordered_map<std::wstring, double> ret_values{};
		DWORD dwBufferSize = 0;
		PDH_FMT_COUNTERVALUE_ITEM *p_items = NULL;
		PDH_STATUS status = ERROR_SUCCESS;
		DWORD dwItemCount = 0;
		PdhCollectQueryData(h_query_);
		Sleep(1000);
		PdhCollectQueryData(h_query_);

		// Get the required size of the pItems buffer.
		status = PdhGetFormattedCounterArray(h_counter_, PDH_FMT_DOUBLE, &dwBufferSize, &dwItemCount, p_items);
		if (PDH_MORE_DATA == status)
		{
			p_items = (PDH_FMT_COUNTERVALUE_ITEM *)malloc(dwBufferSize);
			if (p_items)
			{
				status = PdhGetFormattedCounterArray(h_counter_, PDH_FMT_DOUBLE, &dwBufferSize, &dwItemCount, p_items);
				if (ERROR_SUCCESS == status)
				{
					// Loop through the array and print the instance name and counter value.
					for (DWORD i = 0; i < dwItemCount; i++)
					{
						wprintf(L"counter: %s, value %.20g\n", p_items[i].szName, p_items[i].FmtValue.doubleValue);
						ret_values[p_items[i].szName] = p_items[i].FmtValue.doubleValue;
					}
				}
				else
				{
					DBG_PRINTF_FL((L"Second PdhGetFormattedCounterArray call failed with 0x%x.\n", status));
				}

				free(p_items);
				p_items = NULL;
				dwBufferSize = dwItemCount = 0;
			}
			else
			{
				wprintf(L"malloc for PdhGetFormattedCounterArray failed.\n");
			}
		}
		else
		{
			wprintf(L"PdhGetFormattedCounterArray failed with 0x%x.\n", status);
		}
		
		return ret_values;
	}

private:
	HQUERY h_query_;
	HCOUNTER h_counter_;
};

class pdh_usage
{
public:
	pdh_usage(const std::wstring & counter_path) :
		pdh_query_(counter_path),
		time_out_thread_(1, std::bind(&pdh_usage::update_usage, this))
	{
		time_out_thread_.set_helper_desc(string(counter_path.begin(), counter_path.end()));
	}

	virtual ~pdh_usage()
	{
		time_out_thread_.Stop();
		DBG_PRINTF_FL((L"time out thread stopped %d", time_out_thread_.get_thread_id()));
	}

	double get_total_usage()
	{
		time_out_thread_.Start();
		double total_usage = 0;
		{
			std::lock_guard lk(usage_mutex_);
			for (const auto& item : usage_)
			{
				total_usage += item.second;
			}
		}
		return total_usage;
	}
	std::unordered_map<std::wstring, double> get_usage()
	{
		time_out_thread_.Start();
		std::lock_guard lk(usage_mutex_);
		return usage_;
	}

	void update_usage()
	{
		std::lock_guard lk(usage_mutex_);
		usage_ = pdh_query_.get_values();
	}
private:
	pdh_query pdh_query_;
	std::unordered_map<std::wstring, double> usage_{};
	TimeOutThread time_out_thread_;
	std::mutex usage_mutex_;
};