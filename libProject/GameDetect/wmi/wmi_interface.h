#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <any>
#include <vector>

class wmi_interface
{
public:
	virtual ~wmi_interface() = default;
	virtual bool set(const std::wstring &namespace_name,
		const std::wstring &class_name,
		const std::wstring &method_name,
		const std::unordered_map<std::wstring, std::any> &args) = 0;
	virtual std::unordered_map<std::wstring, std::any> get(const std::wstring &namespace_name,
		const std::wstring &class_name,
		const std::wstring &method_name,
		const std::vector<std::wstring> &select_items) = 0;
	virtual std::unordered_map<std::wstring, std::any>  get(
		const std::wstring &namespace_name,
		const std::wstring &class_name,
		const std::wstring &method_name,
		const std::unordered_map<std::wstring, std::any> &args,
		const std::vector<std::wstring> &select_items
		) = 0;
	[[nodiscard]] virtual auto query(const std::wstring &namespace_name,
		const std::wstring& query_wql)->std::vector < std::unordered_map<std::wstring, std::any>> = 0;
	virtual int register_event(const std::wstring&namespace_name,
		const std::wstring &query_wql,
		std::function<bool(std::unordered_map<std::wstring, std::any>)> callback) = 0;
	virtual bool stop_event(const std::wstring& namespace_name, const std::wstring& query_wql, int id) = 0;
};
