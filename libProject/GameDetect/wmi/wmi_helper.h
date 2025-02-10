#pragma once
//for mock
#include "wmi_agent.h"

class wmi_helper
{
public:
	wmi_helper()
	{
	}
	virtual ~wmi_helper()
	{
	}
	bool set(const std::wstring& namespace_name, const std::wstring& class_name, const std::wstring& method_name,
		const std::unordered_map<std::wstring, std::any>& args)
	{
		return wmi_agent::get_instance()->set(namespace_name, class_name, method_name, args);
	}
	std::unordered_map<std::wstring, std::any> get(const std::wstring& namespace_name, const std::wstring& class_name,
		const std::wstring& method_name, const std::vector<std::wstring>& select_items)
	{
		return wmi_agent::get_instance()->get(namespace_name, class_name, method_name, select_items);
	}
	[[nodiscard]] auto query(const std::wstring& namespace_name,
		const std::wstring& query_wql) -> std::vector<std::unordered_map<std::wstring, std::any>>
	{
		return wmi_agent::get_instance()->query(namespace_name, query_wql);
	}

	int register_event(const std::wstring& namespace_name, const std::wstring& query_wql,
		std::function<bool(std::unordered_map<std::wstring, std::any>)> callback) {
		return true;
	};
	bool stop_event(int id) { return false; };
};
