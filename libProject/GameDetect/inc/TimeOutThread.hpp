// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once

#include <future>
#include <atomic> 
#include "Event.hpp"
#include "DebugUtil.h"


class TimeOutThread
{
public:
	template <typename Func, typename... Args>
	TimeOutThread(DWORD interval, Func&& f, Args && ... args) :mStopEvent(false, false), mLoopInterval(interval*1000),
		mMaxTimes(10), mFirstTime(true)
	{
		//can't use &f in mFunc [&f, &args...], if f is destroyed outside, mFunc can't call f anymore
		//mFunc = [f, &args...] { return f(std::forward<Args>(args)...); };
		mFunc = [f = std::forward<Func>(f), args = std::make_tuple(std::forward<Args>(args)...)]() mutable { std::apply(std::move(f), std::move(args)); };
	};

	~TimeOutThread()
	{
		Stop();
	}

	void Start()
	{
		std::unique_lock< std::mutex> locker(mMutex);
		if (mFunc == nullptr)
			return;

		mCount = 0;
		if (mThread.valid())
		{
			if (mThread.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
				return;
		}

		mThread = async(launch::async, [this]()->void {
			thread_id_ = GetCurrentThreadId();
			DBG_PRINTF_FL((L"TimeOutThread thread created - %S, thread id %d",thread_desc_.c_str(), thread_id_));
			do 
			{
				DBG_PRINTF_FL((L"TimeOutThread mCount=%d mMaxTimes=%d", (int)mCount, mMaxTimes));
			    mFunc();
				if (mCount > mMaxTimes)
					break;
				mCount++;
			} while (mStopEvent.Wait(mLoopInterval) == WAIT_TIMEOUT);

			DBG_PRINTF_FL((L"TimeOutThread exit- %S thread id %d",thread_desc_.c_str(),thread_id_));
		});
	}

	DWORD get_thread_id() const
	{
		return thread_id_;
	}

	void Stop()
	{
		try
		{
			if (mThread.valid())
			{
				mStopEvent.Set();
				mThread.wait();
			}
		}
		catch (...) {};
	}
	void set_helper_desc(const string &thread_desc)
	{
		thread_desc_ = thread_desc;
	}

private:
	CEvent mStopEvent;
	DWORD mLoopInterval;
	DWORD mMaxTimes;
	bool  mFirstTime;
	std::future<void> mThread;
	std::function<void()> mFunc;
	std::mutex mMutex;
	std::atomic<DWORD>      mCount;
	DWORD thread_id_ = 0;
	std::string thread_desc_ = "";
};