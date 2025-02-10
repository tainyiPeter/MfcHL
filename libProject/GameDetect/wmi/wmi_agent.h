#pragma once

#include "wmi_interface.h"
#include "wmi_class.h"
#include "wmi_instance.h"
#include "wmi_event.h"
#include "wmi_server_thread.h"

#include <any>
#include <chrono>
#include <utility>
#include <functional>
#include <string>
#include <mutex>
#include <unordered_map>

//#include "../../inc/DebugUtil.h"

class wmi_agent : public wmi_interface
{
public:
	wmi_agent()
	{
		
	}

	~wmi_agent() override
	{
		wmi_namespace_server_.clear();
	}

	bool set(
		const std::wstring &namespace_name,
		const std::wstring &class_name,
		const std::wstring &method_name,
		const std::unordered_map<std::wstring, std::any> &args) override
	{
		if (add_namespace_server(namespace_name))
		{
			return wmi_namespace_server_[namespace_name]->add_task_and_wait([&]()
			{
				try
				{
					wmi_class wmi(wmi_namespace_server_[namespace_name]->get(), class_name);
					return wmi.set(method_name, args);
				}
				catch (std::exception& e)
				{
					//DBG_PRINTF_FL((L"wmi_class::set method_name(%s) throw an exception %S", method_name.c_str(), e.what()));
				}

				return false;
			});

		}
		return false;
	}

	std::unordered_map<std::wstring, std::any>  get(
		const std::wstring &namespace_name,
		const std::wstring &class_name,
		const std::wstring &method_name,
		const std::vector<std::wstring> &select_items) override
	{
		if (add_namespace_server(namespace_name))
		{
			std::unordered_map<std::wstring, std::any> ret_value;
			wmi_namespace_server_[namespace_name]->add_task_and_wait([&]()
			{
				try
				{
					const wmi_class wmi(wmi_namespace_server_[namespace_name]->get(), class_name);
					ret_value = wmi.exec_method(method_name, select_items);
				}
				catch (std::exception& e)
				{
					//DBG_PRINTF_FL((L"wmi_class::get method_name(%s) throw an exception %S", method_name.c_str(), e.what()));
				}

				return true;
			});
			return ret_value;
		}
		return std::unordered_map<std::wstring, std::any>{  };
	}

	std::unordered_map<std::wstring, std::any>  get(
		const std::wstring &namespace_name,
		const std::wstring &class_name,
		const std::wstring &method_name,
		const std::unordered_map<std::wstring, std::any> &args,
		const std::vector<std::wstring> &select_items) override
	{
		if (add_namespace_server(namespace_name))
		{
			std::unordered_map<std::wstring, std::any> ret_value;
			wmi_namespace_server_[namespace_name]->add_task_and_wait([&]()
			{
				try
				{
					wmi_class wmi(wmi_namespace_server_[namespace_name]->get(), class_name);
					return wmi.execute(method_name, args, select_items, ret_value);
				}
				catch (std::exception& e)
				{
					//DBG_PRINTF_FL((L"wmi_class::get method_name(%s) throw an exception %S", method_name.c_str(), e.what()));
				}

				return true;
			});
			return ret_value;
		}
		return std::unordered_map<std::wstring, std::any>{  };
	}

	[[nodiscard]] virtual auto query(const std::wstring &namespace_name,
		const std::wstring& query_wql) -> std::vector < std::unordered_map<std::wstring, std::any>> override
	{
		if (add_namespace_server(namespace_name))
		{
			std::vector < std::unordered_map<std::wstring, std::any>> ret_value={};
			wmi_namespace_server_[namespace_name]->add_task_and_wait([&]()
			{
				try
				{
					const static_wmi_instance wmi(wmi_namespace_server_[namespace_name]->get());
					ret_value = wmi.query(query_wql);
				}
				catch (std::exception& e)
				{
					//DBG_PRINTF_FL((L"wmi_class::query query_wql(%s) throw an exception %S", query_wql.c_str(), e.what()));
				}

				return true;
			});
			return ret_value;
		}

		return std::vector < std::unordered_map<std::wstring, std::any>>{};
	}

	int register_event(const std::wstring& namespace_name, const std::wstring& query_wql,
		std::function<bool(std::unordered_map<std::wstring, std::any>)> callback) override;
	bool stop_event(const std::wstring& namespace_name, const std::wstring& query_wql, int id) override;
private:
	std::unordered_map<std::wstring, std::unique_ptr<wmi_server_thread>> wmi_namespace_server_;
	std::unordered_map<std::wstring, std::unique_ptr<wmi_event>> events_;
	std::mutex events_mutex_;
	std::mutex wmi_namespace_server_mutex_;

	bool add_namespace_server(const std::wstring& namespace_name)
	{
		std::lock_guard<std::mutex> lk(wmi_namespace_server_mutex_);
		if (wmi_namespace_server_.find(namespace_name) == wmi_namespace_server_.end())
		{
			std::unique_ptr<wmi_server_thread> server = std::make_unique<wmi_server_thread>(namespace_name);
			if (server->get() != nullptr)
				wmi_namespace_server_[namespace_name] = std::move(server);
			else return false;
		}
		return true;
	}
};

inline int wmi_agent::register_event(const std::wstring& namespace_name, const std::wstring& query_wql,
	std::function<bool(std::unordered_map<std::wstring, std::any>)> callback)
{
	std::lock_guard<std::mutex> lk(events_mutex_);
	const auto &iter = events_.find(namespace_name+query_wql);
	if (iter == events_.end())
	{
		events_[namespace_name + query_wql] = std::make_unique<wmi_event>(namespace_name, query_wql);
		int ret_id = events_[namespace_name + query_wql]->register_event(callback);
		std::mutex event_create_complete_mutex;
		std::unique_lock<std::mutex> lk(event_create_complete_mutex);

		std::condition_variable cv_event_init_complete;
		events_[namespace_name + query_wql]->create_event_thread(cv_event_init_complete);
		cv_event_init_complete.wait_for(lk, std::chrono::seconds(20));
		if (events_[namespace_name + query_wql]->is_create_event_success())
		{
			return ret_id;
		}
		else
		{
			events_.erase(namespace_name + query_wql);
			return 0;
		}
	}
	else
	{
		return events_[namespace_name + query_wql]->register_event(callback);
	}
}

inline bool wmi_agent::stop_event(const std::wstring& namespace_name, const std::wstring& query_wql,int id)
{
	std::lock_guard<std::mutex> lk(events_mutex_);
	const auto &iter = events_.find(namespace_name + query_wql);
	if(iter != events_.end())
	{
		if(iter->second->unregister_event(id) == 1)
		{
			events_.erase(namespace_name + query_wql);
			return true;
		}

	}
	return true;
}
