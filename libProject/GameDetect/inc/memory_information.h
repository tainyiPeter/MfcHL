// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <iomanip>
#include <string>
#include <sstream>
#include <sysinfoapi.h>

#include "inc/SMBiosInformationRetriever.h"

class memory_information
{
public:
	memory_information():usage_(0),used_(0),total_(0)
	{
		std::wstring w_total;
		std::wstring w_type;
		std::wstring w_name;
		MachineInformationHelper().GetFormatMemoryInfo(w_total ,w_type, w_name, total_);
		s_total_ = _W2A_(w_total);
		type_ = _W2A_(w_type);
		name_ = _W2A_(w_name);
	}

	[[nodiscard]] std::string get_total() const
	{
		return s_total_;
	}
	int get_usage() 
	{
		update_usage_used();
		return usage_;
	}
	std::string get_used()
	{
		update_usage_used();
		std::stringstream ss;
		ss << std::fixed << std::setprecision(1) << used_ << "GB";
		return ss.str();
	}
	std::string get_type()
	{
		return type_;
	}
	std::string get_name()
	{
		return name_;
	}
private:
	int usage_;
	float used_;
	int total_;
	std::string s_total_;
	std::string type_;
	std::string name_;
	void update_usage_used()
	{
		const int nAvailable = GetMemoryAvailable();
		if (total_ != 0)
			usage_ = (total_ - nAvailable) * 100 / total_;

		used_ = static_cast<float>(total_ - nAvailable) / static_cast<float>(1024.0);
	}
	static int GetMemoryAvailable()
	{
		MEMORYSTATUSEX m_status_ex;
		m_status_ex.dwLength = sizeof(m_status_ex);
		if (FALSE == GlobalMemoryStatusEx(&m_status_ex))
			return 0;

		return static_cast<int>((m_status_ex.ullAvailPhys / 1024.0) / 1024.0);
	}
};

class memory_information_agent
{
public:
	static std::shared_ptr<memory_information_agent> get_instance()
	{
		std::shared_ptr<memory_information_agent> memory_information_agent_ = hw_manager::get_instance().resolve_shared<memory_information_agent>(memory_information_agent_instance_key);
		if (memory_information_agent_ == nullptr)
		{
			hw_manager::get_instance().add_type<memory_information_agent>(memory_information_agent_instance_key, std::shared_ptr<memory_information_agent>(new memory_information_agent()));
			memory_information_agent_ = hw_manager::get_instance().resolve_shared<memory_information_agent>(memory_information_agent_instance_key);
		}
		return memory_information_agent_;
	}
	memory_information & get_memory_information()
	{
		return information_;
	}

private:
	static constexpr char memory_information_agent_instance_key[] = "memory_information_agent";

	memory_information_agent()
	{
		
	}
	memory_information information_;
};