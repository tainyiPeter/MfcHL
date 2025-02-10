// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <vector>
#include <string>
#include <iomanip>

#include <D3D10_1.h>
#include <dxgi1_6.h>
#include <DXGI.h>
#include <sstream>

#include "pdh_query.h"
#pragma comment(lib, "dxgi.lib")



class memory_formater
{
public:
	static std::string format_total_memory(SIZE_T memory_size)
	{
		stringstream gpu_memory;
		memory_size = memory_size / 1024 / 1024;
		if (memory_size >= 1024)
		{
			memory_size = (memory_size + 512) / 1024;
			gpu_memory << memory_size << "GB";
		}
		else
		{
			gpu_memory << memory_size << "MB";
		}
		return gpu_memory.str();
	}
	static std::string format_usage_memory(double usage)
	{
		stringstream ss;
		ss.setf(ios::fixed);
		ss.precision(2);
		ss << usage << "GB";
		return ss.str();
	}
};

class integrated_gpu_memory_static_info
{
public:
	integrated_gpu_memory_static_info()
	{
		if (!init_integrated_gpu_memory_info())
		{
			DBG_PRINTF_FL((L"init integrated gpu memory info failed."));
		}
		DBG_PRINTF_FL((L"integrated gpu success name %S,total %S, luid %s",name_.c_str(),total_.c_str(),luid_.c_str()));
	}
	std::string get_name()
	{
		return name_;
	}
	std::string get_total()
	{
		return total_;
	}
	std::wstring get_luid()
	{
		return luid_;
	}
private:
	std::string name_;
	std::string total_;
	std::wstring luid_;//luid_0x00000000_0x10ea9_phys_0
	bool init_integrated_gpu_memory_info()
	{
		HRESULT err = S_OK;
		bool bRet = false;
#ifdef USE_DXGI1_2
		REFIID iidVal = OSGetVersion() >= 8 ? __uuidof(IDXGIFactory2) : __uuidof(IDXGIFactory1);
#else
		REFIID iidVal = __uuidof(IDXGIFactory4);
#endif

		IDXGIFactory4 *factory = NULL;
		if (SUCCEEDED(err = CreateDXGIFactory1(iidVal, (void**)&factory)))
		{
			unsigned int i = 0;
			IDXGIAdapter1 *giAdapter = NULL;

			while (factory->EnumAdapters1(i++, &giAdapter) == S_OK)
			{
				//Log(TEXT("------------------------------------------"));
				DXGI_ADAPTER_DESC1 adapterDesc;
				if (SUCCEEDED(err = giAdapter->GetDesc1(&adapterDesc)))
				{

					if (adapterDesc.DedicatedVideoMemory != 0 ||
						adapterDesc.SubSysId != 0)
					{
						unsigned int j = 0;
						IDXGIOutput *giOutput = NULL;
						while (giAdapter->EnumOutputs(j++, &giOutput) == S_OK)
						{
							DXGI_OUTPUT_DESC outputDesc;
							if (SUCCEEDED(giOutput->GetDesc(&outputDesc)))
							{
								name_ = _W2A_(adapterDesc.Description);
								if (outputDesc.AttachedToDesktop)
								{
									if ((string::npos != name_.find("AMD")) || (string::npos != name_.find("Radeon")))
									{
										bRet = true;
										break;
									}
									if (string::npos != name_.find("Intel"))
									{
										bRet = true;
										break;
									}
								}
								else
									DBG_PRINTF_FL((L"Not attache to desktop.\n"));

							}
							giOutput->Release();
						}
						if (bRet == true)
						{
							total_ = memory_formater::format_total_memory(adapterDesc.SharedSystemMemory);
							wstringstream wss;
							wss << L"luid_0x"
								<< std::hex << std::setw(8) << std::setfill(L'0')
								<< adapterDesc.AdapterLuid.HighPart
								<< L"_0x"
								<< std::hex << std::setw(8) << std::setfill(L'0')
								<< adapterDesc.AdapterLuid.LowPart
								<< L"_phys_0";
							luid_ = wss.str();
						}
					}
				}

				giAdapter->Release();
				if (bRet == true) {
					break;
				}
			}
			factory->Release();
		}

		return bRet;
	}
};

class integrated_gpu_memory_usage: public pdh_usage
{
	//get_gpu_counter_path(pdh_object::get_pdh_instances_by_object_name(L"GPU Non Local Adapter Memory")
public:
	integrated_gpu_memory_usage():pdh_usage(get_gpu_counter_path())
	{
	}
private:
	static std::wstring get_gpu_counter_path()
	{
		std::wstringstream w_ss;
		w_ss << L"\\GPU Adapter Memory("<< integrated_gpu_memory_static_info().get_luid() << L")\\Shared Usage";
		return w_ss.str();
	}
};

class discrete_gpu_static_info
{
public:
	discrete_gpu_static_info()
	{
		if (!init_gpu_static_info(get_luid())) {
			DBG_PRINTF_FL((L"init discrete gpu static info failed!"));
		}
		DBG_PRINTF_FL((L"discrete static gpu info init success name %S,total %S", name_.c_str(), total_.c_str()));
	}

	virtual std::string get_name() {
		return name_;
	}

	virtual std::string get_total() {
		return total_;
	}
	static std::wstring get_luid()
	{
		const std::vector<std::wstring> &instances =
			pdh_object::get_pdh_instances_by_object_name(L"GPU Non Local Adapter Memory");
		if (!instances.empty())
		{
			DBG_PRINTF_FL((L"Has dicreted luid instance is %s", instances[0].c_str()));
			return instances[0].substr(0,33);
		}
		return L"";
	}
private:
	std::string name_;
	std::string total_;
	bool init_gpu_static_info(const std::wstring &luid)
	{
		HRESULT err = S_OK;
		bool bRet = false;
#ifdef USE_DXGI1_2
		REFIID iidVal = OSGetVersion() >= 8 ? __uuidof(IDXGIFactory2) : __uuidof(IDXGIFactory1);
#else
		REFIID iidVal = __uuidof(IDXGIFactory);
#endif
		const long high = stol(luid.substr(5, 10), nullptr, 16);
		const unsigned long low = stoul(luid.substr(16, 10), nullptr, 16);

		IDXGIFactory4 *factory = NULL;
		if (SUCCEEDED(err = CreateDXGIFactory1(iidVal, (void**)&factory)))
		{
			UINT i = 0;
			IDXGIAdapter1 *giAdapter = NULL;

			while (factory->EnumAdapters1(i++, &giAdapter) == S_OK)
			{
				//Log(TEXT("------------------------------------------"));
				DXGI_ADAPTER_DESC1 adapterDesc;
				if (SUCCEEDED(err = giAdapter->GetDesc1(&adapterDesc)))
				{
					//wstringstream device_luid;
					//device_luid << hex << setw(8) << setfill(L'0')
					//<< adapterDesc.AdapterLuid.HighPart
					//<< hex << setw(8) << setfill(L'0')
					//<< adapterDesc.AdapterLuid.LowPart;
					DBG_PRINTF_FL((L"input luid high %x,low %x, device high %x,low %x,adapter desc %s,dedicated memory %ld",
						high,low, adapterDesc.AdapterLuid.HighPart, adapterDesc.AdapterLuid.LowPart,
						adapterDesc.Description,
						adapterDesc.DedicatedVideoMemory));

					if (high == adapterDesc.AdapterLuid.HighPart && low == adapterDesc.AdapterLuid.LowPart)
					{
						name_ = _W2A_(adapterDesc.Description);
						total_ = memory_formater::format_total_memory(adapterDesc.DedicatedVideoMemory);
						bRet = true;
					}
				}
				
				giAdapter->Release();
			}
			factory->Release();
		}

		return bRet;
	}

};

class discrete_gpu_memory_usage: public pdh_usage
{
public:
	discrete_gpu_memory_usage() :pdh_usage(get_discrete_gpu_counter_path())
	{
	}
private:
	static std::wstring get_discrete_gpu_counter_path()
	{
		std::wstring luid = discrete_gpu_static_info::get_luid();
		if (!luid.empty())
		{
			std::wstringstream w_ss;
			w_ss << L"\\GPU Adapter Memory(" << luid.c_str() << L")\\Dedicated Usage";
			return w_ss.str();
		}
		return L"";
	}
};
class gpu_usage : public pdh_usage
{
public:
	gpu_usage(std::wstring luid) :
	luid_(luid),
	pdh_usage(get_gpu_usage_counter_path(luid))
	{

	}
	double get_usage()
	{
		auto usages_ = pdh_usage::get_usage();
		std::unordered_map<std::wstring, double> engines_usage{};
		for(const auto &item: usages_)
		{
			std::wstring key_name = item.first.substr(item.first.find(luid_) + luid_.length(), item.first.length());
			if(engines_usage.find(key_name) == engines_usage.end())
			{
				engines_usage[key_name] = item.second;
			}
			else
			{
				engines_usage[key_name] += item.second;
			}
		}

		// get the max value of engine type.
		auto ite = std::max_element(engines_usage.begin(), engines_usage.end(),
			[](std::pair<wstring, double> val1, std::pair<wstring, double> val2)->bool {
			if (val1.second < val2.second)
			{
				return true;
			}
			return false;
		});
		if(ite != engines_usage.end())
			return ite->second;
		return 0;
	}
private:
	std::wstring luid_;
	std::wstring get_gpu_usage_counter_path(std::wstring luid)
	{
		if (!luid.empty())
		{
			std::wstringstream w_ss;
			w_ss << L"\\GPU Engine(*" << luid.c_str() << L"*)\\Utilization Percentage";
			return w_ss.str();
		}
		return L"";
	}
};

class integrated_gpu_usage: public gpu_usage
{
public:
	integrated_gpu_usage():gpu_usage(integrated_gpu_memory_static_info().get_luid()){}

};
class discrete_gpu_usage : public gpu_usage
{
public:
	discrete_gpu_usage() :gpu_usage(discrete_gpu_static_info::get_luid()) {}

};