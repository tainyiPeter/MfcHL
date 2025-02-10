// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include<list>
#include<mutex>
#include<thread>
#include<condition_variable>
#include <iostream>
#include <unordered_map>

using namespace std;

template<typename T>
class SyncQueue
{
public:
	SyncQueue(int maxSize = 20) :m_maxSize(maxSize), m_needStop(false), m_bInit(false)
	{
	}

	bool IsStop()
	{
		return m_needStop == true;
	}

	void Put(const T&x)
	{
		Add(x);
	}

	void Put(T&&x)
	{
		Add(std::forward<T>(x));
	}

	void StartWork(bool bInit)
	{
		m_bInit = bInit;
	}

	void Notify()
	{
		m_notEmpty.notify_one();
	}

	void Take(std::list<T>& list)
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_notEmpty.wait(locker, [this] {return m_needStop || (NotEmpty() && m_bInit); });

		if (m_needStop)
			return;
		list = std::move(m_queue);
	}

	bool Take(T& t)
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_notEmpty.wait(locker, [this] {return m_needStop || (NotEmpty() && m_bInit); });

		if (m_needStop)
			return false;
		t = m_queue.front();
		m_queue.pop_front();
		return true;
	}

	void Stop()
	{
		{
			std::lock_guard<std::mutex> locker(m_mutex);
			m_needStop = true;
		}
		m_notEmpty.notify_all();
	}

	bool Empty()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.empty();
	}

	bool Full()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.size() == m_maxSize;
	}

	size_t Size()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.size();
	}

private:
	bool NotEmpty() const
	{
		bool empty = m_queue.empty();
		return !empty;
	}

	template<typename F>
	void Add(F&&x)
	{
		std::unique_lock< std::mutex> locker(m_mutex);
		if (m_needStop)
			return;

		m_queue.push_back(std::forward<F>(x));
		m_notEmpty.notify_one();
	}

public:
	std::list<T> m_queue= {};
	std::mutex m_mutex;
	std::condition_variable m_notEmpty;
	int m_maxSize;
	bool m_bInit;
	bool m_needStop;
};

// not be called
template<typename T>
class SyncQueueUnique : public SyncQueue<T>
{
public:
	void PutUnique(const T&x)
	{
		AddUnique(x);
	}

	void PutUnique(T&&x)
	{
		AddUnique(std::forward<T>(x));
	}

private:
	template<typename F>
	void AddUnique(F&&x)
	{
		std::unique_lock< std::mutex> locker(SyncQueue<T>::m_mutex);
		if (SyncQueue<T>::m_needStop)
			return;

		auto item = std::find(SyncQueue<T>::m_queue.begin(), SyncQueue<T>::m_queue.end(), x);
		if (item == std::end(SyncQueue<T>::m_queue))
		{
			SyncQueue<T>::m_queue.push_back(std::forward<F>(x));
			SyncQueue<T>::m_notEmpty.notify_one();
		}
	}
};

template<class _Kty, class _Ty>
class SyncQueue_map
{
public:
	SyncQueue_map() : m_bInit(false), m_needStop(false)
	{
	}

	bool is_stop() const
	{
		return m_needStop == true;
	}

	void Put(const _Kty& key, _Ty value)
	{
		std::unique_lock< std::mutex> locker(m_mutex);
		if (m_needStop)
			return;

		//save the last one if the key has existed
		if (m_queue.contains(key))
		{
			DBG_PRINTF_FL((L"SyncQueue_map:Put the key has existed, replace it !\n"));
		}
		m_queue[key] = std::move(value);
		m_notEmpty.notify_one();
	}
	 
	void StartWork(bool bInit)
	{
		m_bInit = bInit;
	}

	void Notify()
	{
		m_notEmpty.notify_one();
	}

	void Take(_Ty& value)
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_notEmpty.wait(locker, [this] {return m_needStop || (NotEmpty() && m_bInit); });

		if (m_needStop)
			return;

		const auto ite = m_queue.begin();
		value = std::move(ite->second);
		m_queue.erase(ite);
	}

	void Stop()
	{
		{
			std::lock_guard<std::mutex> locker(m_mutex);
			m_needStop = true;
		}
		m_notEmpty.notify_all();
	}

	bool Empty()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.empty();
	}

	size_t Size()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.size();
	}

private:
	bool NotEmpty() const
	{
		const bool empty = m_queue.empty();
		return !empty;
	}


public:
	std::map<_Kty, _Ty> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_notEmpty;
	bool m_bInit;
	bool m_needStop;
};