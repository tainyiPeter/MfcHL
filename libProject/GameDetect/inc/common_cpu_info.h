// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "inc/Registry.h"
#include "inc/DebugUtil.h"
#include "inc/StringUtility.h"
#include <pdh.h>  
#include <sstream>

#include "cpu_frequency.h"
#include "cpu_usage.h"
#include "hw_manager.h"

#pragma comment(lib, "pdh.lib")

class common_cpu_information
{
public:
	common_cpu_information()
	{
		init_cpu_max_frequency();
		init_cpu_name();
	}
	int get_cpu_usage()
	{
		return usage_.get_usage();
	}
	std::string get_cpu_current_frequency()
	{
		return frequency_.s_get_current_frequency();
	}
	std::wstring w_get_cpu_current_frequency()
	{
		return _A2W_(frequency_.s_get_current_frequency());
	}
	std::string get_cpu_max_frequency()
	{
		return max_cpu_frequency_;
	}
	[[nodiscard]] std::wstring w_get_cpu_max_frequency() const
	{
		return _A2W_(max_cpu_frequency_);
	}
	std::string get_cpu_name()
	{
		return cpu_name_;
	}

	[[nodiscard]] std::wstring w_get_cpu_name() const
	{
		return _A2W_(cpu_name_);
	}
private:
	std::string max_cpu_frequency_;
	std::string cpu_name_;
	cpu_usage usage_;
	cpu_frequency frequency_;
	static constexpr const char * k_processor_key = R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)";
	static constexpr const char * k_processor_name = "ProcessorNameString";
	static constexpr const char * k_processor_max_frequency = "~MHz";

private:
	static std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with)
	{
		std::size_t count{};
		for (std::string::size_type pos{};
			inout.npos != (pos = inout.find(what.data(), pos, what.length()));
			pos += with.length(), ++count) {
			inout.replace(pos, what.length(), with.data(), with.length());
		}
		return count;
	}
	bool init_cpu_name()
	{
		registry::registry_key processor_key = registry::registry_local_machine(k_processor_key, KEY_QUERY_VALUE);
		if (!processor_key.valid())
		{
			DBG_PRINTF_FL((L"can not open cpu name's registry key %S",k_processor_key));
			return false;
		}
		//查询CPU主频  
		if (processor_key.get_value(k_processor_name, cpu_name_))
		{
			DBG_PRINTF_FL((L"can not query cpu frequency's registry value name:%S, key: %S",k_processor_name,k_processor_key));
			return false;
		}
		replace_all(cpu_name_, "  ", " ");

		return true;
	}

	bool init_cpu_max_frequency()
	{
		registry::registry_key processor_key = registry::registry_local_machine(k_processor_key, KEY_QUERY_VALUE);
		if (!processor_key.valid())
		{
			DBG_PRINTF_FL((L"can not open cpu frequency's registry key %S",k_processor_key));
			return false;
		}
		//查询CPU主频  
		DWORD frequency_value;
		if (processor_key.get_value(k_processor_max_frequency, frequency_value))
		{
			DBG_PRINTF_FL((L"can not query cpu frequency's registry value:%S, key: %S",k_processor_max_frequency,k_processor_key));
			return false;
		}

		if (frequency_value >= 1000)
		{ 
			std::stringstream ss_frequency;
			ss_frequency.setf(ios::fixed);
			ss_frequency.precision(2); // 1355 will be 1.36. 1354 will be 1.34.
			ss_frequency << static_cast<float>(frequency_value / 1000.0) << "GHz";
			max_cpu_frequency_ = ss_frequency.str();
		}
		else
		{
			std::stringstream ss_frequency;
			ss_frequency << frequency_value << "MHz";
			max_cpu_frequency_ = ss_frequency.str();
		}
		return true;
	}
};

class cpu_info_agent
{
public:
	static std::shared_ptr<cpu_info_agent> get_instance()
	{
		std::shared_ptr<cpu_info_agent> cpu_info_agent_ = hw_manager::get_instance().resolve_shared<cpu_info_agent>(cpu_info_agent_instance_key);
		if (cpu_info_agent_ == nullptr)
		{
			hw_manager::get_instance().add_type<cpu_info_agent>(cpu_info_agent_instance_key, std::shared_ptr<cpu_info_agent>(new cpu_info_agent()));
			cpu_info_agent_ = hw_manager::get_instance().resolve_shared<cpu_info_agent>(cpu_info_agent_instance_key);
		}
		return cpu_info_agent_;
	}
	common_cpu_information &get_common_cpu_information()
	{
		return information_;
	}
private:
	static constexpr char cpu_info_agent_instance_key[] = "cpu_info_agent";
	cpu_info_agent() {}
	common_cpu_information information_;
};