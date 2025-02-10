#pragma once
#include <any>
#include <utility>
#include <string>
#include <functional>

#include "hwkit/hw_manager.h"

class wmi_error_const
{
public:
	static constexpr unsigned long e_wmi_failed = 0xFFFFFFFF;
};

class gaming_wmi_helper
{
public:
	explicit gaming_wmi_helper()
	{
	}
	gaming_wmi_helper(std::wstring name_space) :namespace_name_(std::move(name_space))
	{
	}
	gaming_wmi_helper(std::wstring name_space, std::wstring class_name) :namespace_name_(std::move(name_space)),class_name_(std::move(class_name))
	{
	}
	auto get(const std::wstring& method_name, const std::vector<std::wstring>& select_items) const
	{
		const std::shared_ptr<wmi_interface> agent = hw_manager::get_instance().resolve_shared<wmi_interface>("gaming_wmi_agent");
		return agent->get(namespace_name_, class_name_, method_name, select_items);
	}

	auto get(const std::wstring& method_name, const std::unordered_map<std::wstring, std::any>& args, const std::vector<std::wstring>& select_items) const
	{
		const std::shared_ptr<wmi_interface> agent = hw_manager::get_instance().resolve_shared<wmi_interface>("gaming_wmi_agent");
		return agent->get(namespace_name_, class_name_, method_name, args,select_items);
	}
	unsigned long get_data(const std::wstring& method_name) const
	{
		try
		{
			const std::shared_ptr<wmi_interface> agent = hw_manager::get_instance().resolve_shared<wmi_interface>("gaming_wmi_agent");
			std::unordered_map<std::wstring, std::any> data = agent->get(namespace_name_, class_name_, method_name, { L"Data" });
			if (data.find(L"Data") != data.end())
			{
				return std::any_cast<unsigned long>(data[L"Data"]);
			}
		}
		catch(...)
		{
			//DBG_PRINTF_FL((L"gaming_wmi_helper get_data %s throw exception !!!", method_name.c_str()));
		}
		
		return wmi_error_const::e_wmi_failed;
	}
	bool set(const std::wstring& method_name, const std::unordered_map<std::wstring, std::any>& args) const
	{
		const std::shared_ptr<wmi_interface> agent = hw_manager::get_instance().resolve_shared<wmi_interface>("gaming_wmi_agent");
		return agent->set(namespace_name_, class_name_, method_name, args);
	}
	bool set_data(const std::wstring& method_name, long data) const
	{
		return set(method_name, { {L"Data",data} });
	}
	auto query(const std::wstring& query_wql) const
	{
		const std::shared_ptr<wmi_interface> agent = hw_manager::get_instance().resolve_shared<wmi_interface>("gaming_wmi_agent");
		return agent->query(namespace_name_, query_wql);
	}
	//return:
	// success - non zero.
	// failed - zero. (two reason: First: spends more than 20 second to register event will be failed. Second: some of wmi methods init failed.)
	int register_event(const std::wstring& query_wql,
			std::function<bool(std::unordered_map<std::wstring, std::any>)> callback) const
	{
		const std::shared_ptr<wmi_interface> agent = hw_manager::get_instance().resolve_shared<wmi_interface>("gaming_wmi_agent");
		return agent->register_event(namespace_name_, query_wql, std::move(callback));
	}

	[[nodiscard]] bool stop_event(const std::wstring& query_wql, const int id) const
	{
		try
		{
			const std::shared_ptr<wmi_interface> agent = hw_manager::get_instance().resolve_shared<wmi_interface>("gaming_wmi_agent");
			return agent->stop_event(namespace_name_, query_wql, id);
		}
		catch (...) {}

		return false;
	}

	void Release()
	{
		hw_manager::get_instance().unregister_type("gaming_wmi_agent");
	}

private:
	std::wstring namespace_name_ = L"root\\wmi";
	std::wstring class_name_ = L"lenovo_gamezone_data";
};