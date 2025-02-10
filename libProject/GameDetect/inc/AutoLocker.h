// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <Windows.h>

class CriticalSection {

public:
	CriticalSection() {
		::InitializeCriticalSection(&m_cs);
	}
	~CriticalSection() {
		::DeleteCriticalSection(&m_cs);
	}
	void Lock() { ::EnterCriticalSection(&m_cs); }
	void Unlock() { ::LeaveCriticalSection(&m_cs); }

private:
	CRITICAL_SECTION m_cs;
	CriticalSection(const CriticalSection&);
	CriticalSection( CriticalSection&&);
	CriticalSection& operator=(const CriticalSection&);
	CriticalSection& operator=( CriticalSection&&);
};

class CAutoLocker
{
	public:
		CAutoLocker(CriticalSection& cs) : m_cs(cs)
		{
			m_cs.Lock();
		}
		~CAutoLocker() {
			m_cs.Unlock();
		}

	private:
		CriticalSection& m_cs;

};


