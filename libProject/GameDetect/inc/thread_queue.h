// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <future>
#include <memory>
#include <thread>
#include <any>

class thread_queue
{
public:

	std::future<void> run_task(std::function<void()> fn)
	{
		while (!init_.load())
		{
			std::this_thread::sleep_for(50ms);
		}
		std::unique_ptr<std::packaged_task<void()>> ptr_task = std::make_unique<
			std::packaged_task<void()>>(fn);

		auto ret_future = ptr_task->get_future();
		{
			std::lock_guard<std::mutex> lock(data_.first);
			data_.second.push_back(std::move(ptr_task));
		}

		data_condition_variable_.notify_one();
		return ret_future;
	}

	void thread_queue_proc()
	{
		do {
			std::unique_lock<std::mutex> lk(data_condition_variable_mutex_);
			data_condition_variable_.wait(lk, [&]() { init_.store(true); return !data_.second.empty() || stop_.load(); });
			if (stop_.load())
				break;
			while (!data_.second.empty())
			{
				std::unique_ptr <std::packaged_task<void()>> ptr_task = nullptr;
				{
					std::lock_guard<std::mutex> lock(data_.first);
					ptr_task = std::move(data_.second.front());
					data_.second.pop_front();
				}

				(*ptr_task)();
			}
		} while (true);
	}
	~thread_queue()
	{
		stop();
	}

	void stop()
	{
		stop_.store(true);
		data_condition_variable_.notify_one();
	}
	bool empty()
	{
		//std::unique_lock<std::mutex> lk(data_.first);
		return data_.second.empty();
	}
	thread_queue();
private:
	std::pair < std::mutex, std::deque<std::unique_ptr<std::packaged_task<void()>>>> data_;
	std::condition_variable data_condition_variable_;
	std::mutex data_condition_variable_mutex_;
	std::future<void> t_;
	std::atomic<bool> init_;
	std::atomic<bool> stop_;
};

inline thread_queue::thread_queue():
init_(false),
stop_(false)
{
	t_ = std::async(std::launch::async, &thread_queue::thread_queue_proc, this);
}
