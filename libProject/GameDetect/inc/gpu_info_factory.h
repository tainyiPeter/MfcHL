// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <string>
#include <optional>

#include "inc/DebugUtil.h"
#include "gpu_memory_usage.h"


class gpu_info
{
public:
	virtual ~gpu_info() = default;
	virtual std::string get_total() = 0;
	virtual std::string get_used() = 0;
	virtual std::string get_name() = 0;
	virtual int get_utilization() = 0;
};

class none_gpu_info :public gpu_info
{
public:

	std::string get_used() override
	{
		return "0";
	}

	std::string get_total() override
	{
		return "0";
	}
	std::string get_name() override
	{
		return "none";
	}
	int get_utilization() override
	{
		return 0;
	}
};

class integrated_gpu_info:public gpu_info, public integrated_gpu_memory_static_info,public integrated_gpu_memory_usage,public integrated_gpu_usage
{
public:
	static bool is_support()
	{
		return integrated_gpu_memory_static_info().get_luid().empty() == false;
	}

	std::string get_used() override
	{
		return memory_formater::format_usage_memory(integrated_gpu_memory_usage::get_total_usage() / 1000.0 / 1000.0 / 1000.0);
	}

	std::string get_total() override
	{
		return integrated_gpu_memory_static_info::get_total();
	}
	std::string get_name() override
	{
		return integrated_gpu_memory_static_info::get_name();
	}
	int get_utilization() override
	{
		return static_cast<int>(integrated_gpu_usage::get_usage());
	}
};

class discrete_gpu_info:public gpu_info, public discrete_gpu_static_info,public discrete_gpu_memory_usage,public discrete_gpu_usage
{
public:
	static bool is_support()
	{
		return !discrete_gpu_static_info::get_luid().empty();
	}

	std::string get_used() override
	{
		return memory_formater::format_usage_memory(discrete_gpu_memory_usage::get_total_usage() / 1000.0 / 1000.0 / 1000.0);
	}
	std::string get_total() override
	{
		return discrete_gpu_static_info::get_total();
	}
	std::string get_name() override
	{
		return discrete_gpu_static_info::get_name();
	}
	int get_utilization() override
	{
		return static_cast<int>(discrete_gpu_usage::get_usage());
	}
};

 
class gpu_info_creator
{
public:
	virtual ~gpu_info_creator() = default;
	virtual std::unique_ptr<gpu_info> create() = 0;
};

class integrated_creator: public gpu_info_creator
{
public:
	std::unique_ptr<gpu_info> create() override
	{
		if(integrated_gpu_info::is_support())
		{
			return std::make_unique<integrated_gpu_info>();
		}
		return nullptr;
	}
};

class discrete_creator : public gpu_info_creator
{
public:
	std::unique_ptr<gpu_info> create() override
	{
		if (discrete_gpu_info::is_support())
		{
			return std::make_unique<discrete_gpu_info>();
		}
		return nullptr;
	}
};

enum gpu_type { none, discrete, integrated };

class gpu_info_factory
{
public:
	static std::shared_ptr<gpu_info_factory> get_instance()
	{
		std::shared_ptr<gpu_info_factory> gpu_info_factory_ = hw_manager::get_instance().resolve_shared<gpu_info_factory>(gpu_info_factory_instance_key);
		if (gpu_info_factory_ == nullptr)
		{
			hw_manager::get_instance().add_type<gpu_info_factory>(gpu_info_factory_instance_key, std::shared_ptr<gpu_info_factory>(new gpu_info_factory()));
			gpu_info_factory_ = hw_manager::get_instance().resolve_shared<gpu_info_factory>(gpu_info_factory_instance_key);
		}
		return gpu_info_factory_;
	}
	std::shared_ptr<gpu_info> get_gpu_info()
	{
		std::lock_guard lk(gpu_info_mutex_);
		return gpu_info_;
	}

	void update_gpu_info()
	{
		create_gpu_info();
	}

	[[nodiscard]] gpu_type get_gpu_type() const
	{
		return gpu_type;
	}

private:

	bool create_gpu_info()
	{
		std::lock_guard lk(gpu_info_mutex_);
		gpu_info_ = discrete_creator().create();
		gpu_type = discrete;
		if (gpu_info_ == nullptr)
		{
			DBG_PRINTF_FL((L"Does not have dgpu. so create integrated gpu info"));

			gpu_info_ = integrated_creator().create();
			gpu_type = integrated;
		}

		if (gpu_info_ == nullptr)
		{
			DBG_PRINTF_FL((L"Does not have dgpu. so create none_gpu_info"));
			gpu_info_ = std::make_shared<none_gpu_info>();
			gpu_type = none;
		}

		return gpu_info_ != nullptr;
	}
	gpu_info_factory()
	{
		if(!create_gpu_info())
		{
			DBG_PRINTF_FL((L"create gpu info failed in gpu factory"));
		}
	}
	std::shared_ptr<gpu_info> gpu_info_;
	std::mutex gpu_info_mutex_;
	static constexpr char gpu_info_factory_instance_key[] = "gpu_info_factory";
	gpu_type gpu_type = none;
};