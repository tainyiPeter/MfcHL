// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <future>
#include <deque>

class worker_thread
{
public:
	void thread_proc()
	{
		do {
			std::unique_lock<std::mutex> lk(data_.first);
			data_condition_variable_.wait_for(lk, 10s,[&]() { init_.store(true); return !data_.second.empty() || stop_.load(); });
			if (stop_.load())
				break;
			while (!data_.second.empty())
			{
				std::unique_ptr<std::packaged_task<bool()>> ptr_task = std::move(data_.second.front());
				(*ptr_task)();
				data_.second.pop_front();
			}
		} while (true);
	}

	bool add_task_and_wait(std::function<bool()> fn)
	{
		std::unique_ptr<std::packaged_task<bool()>> ptr_task = std::make_unique<std::packaged_task<bool()>>(fn);
		std::future<bool> future = ptr_task->get_future();
		while (!init_.load())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		{
			std::lock_guard<std::mutex> lock(data_.first);
			data_.second.push_back(std::move(ptr_task));
		}
		data_condition_variable_.notify_one();
		return future.get();
	}
	virtual ~worker_thread()
	{
		try
		{
			stop_.store(true);
			data_condition_variable_.notify_one();
			t_.wait();
		}
		catch (...) {

		}

	}
	explicit worker_thread();

private:
	std::pair < std::mutex, std::deque<std::unique_ptr<std::packaged_task<bool()>>>> data_;
	std::condition_variable data_condition_variable_;
	std::future<void> t_;
	std::atomic<bool> init_;
	std::atomic<bool> stop_;
};

inline worker_thread::worker_thread() :
	init_(false),
	stop_(false)
{
	t_ = std::async(std::launch::async, &worker_thread::thread_proc, this);
}