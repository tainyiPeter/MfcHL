#pragma once

#include<mutex>

template<typename T>
class LockObject
{
public:
	void Set(const T &obPtr)
	{
		std::unique_lock< std::mutex> locker(object_Mutex);
		object_ptr.reset(new T());
		*object_ptr = obPtr;
	}

	std::shared_ptr<const T> Get() {
		std::unique_lock< std::mutex> locker(object_Mutex);
		return object_ptr;
	}

	std::shared_ptr<const T> WaitForResult(DWORD timeOut)
	{
		DWORD Interval = 0;
		while (Interval < timeOut)
		{
			DBG_PRINTF_FL((L"LockObject wait for timeout"));
			Sleep(100);
			auto sharedPtr = Get();
			if (sharedPtr != nullptr)
				return sharedPtr;
			Interval += 100;
		}
		return nullptr;
	}

private:
	std::mutex object_Mutex;
	std::shared_ptr<T> object_ptr = nullptr;
};

template<class key, class value>
class ThreadSafeMap
{
public:

	void Set(const key mapKey, const value mapValue)
	{
		std::lock_guard< std::mutex> locker(object_Mutex);
		m_map[mapKey] = mapValue;
	}

	map<key, value> Get() {
		std::lock_guard< std::mutex> locker(object_Mutex);
		return m_map;
	}

	vector<ULONG> Getpidlist() {
		std::lock_guard< std::mutex> locker(object_Mutex);
		vector<ULONG> c_list;
		for (auto &item : m_map)
		{
			if (item.second>0)
				c_list.push_back(item.second);
		}
		return c_list;
	}

	bool DeleteFormKey(const key mapKey)
	{
		std::lock_guard< std::mutex> locker(object_Mutex);
		if (m_map.find(mapKey) != m_map.end())
		{
			m_map.erase(mapKey);
			return true;
		}
		return false;
	}

	bool ResetValue(const value mapValue, const value resetValue)
	{
		std::lock_guard< std::mutex> locker(object_Mutex);
		for (auto &item : m_map)
		{
			if (item.second == mapValue)
			{
				item.second = resetValue;
				return true;
			}
		}
		return false;
	}

	void Clear()
	{
		std::lock_guard< std::mutex> locker(object_Mutex);
		m_map.clear();
	}

private:
	std::mutex object_Mutex;
	map<key, value> m_map;
};