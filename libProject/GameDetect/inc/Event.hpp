// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <windows.h>

class CEvent
{
public:

	CEvent(bool bManual, bool bSignaled);
	~CEvent();

	HANDLE Handle() const;

	void Signal();
	void Reset();
	void Pulse();
	void Set();

	DWORD Wait(DWORD dwWaitTime = INFINITE) const;
	bool IsSignalled() const;

protected:

	HANDLE	m_hEvent;

private:

	CEvent(const CEvent&);
	CEvent& operator=(const CEvent&);
};

inline CEvent::CEvent(bool bManual, bool bSignaled)
{
	m_hEvent = ::CreateEvent(NULL, bManual, bSignaled, NULL);
}

inline CEvent::~CEvent()
{
	if (m_hEvent != NULL)
	    ::CloseHandle(m_hEvent);
}

inline HANDLE CEvent::Handle() const
{
	return m_hEvent;
}

inline void CEvent::Signal()
{
	if (m_hEvent != NULL)
	    ::SetEvent(m_hEvent);
}

inline void CEvent::Reset()
{
	if (m_hEvent != NULL)
	    ::ResetEvent(m_hEvent);
}

inline void CEvent::Pulse()
{
	if (m_hEvent != NULL)
	    ::PulseEvent(m_hEvent);
}

inline void CEvent::Set()
{
	if (m_hEvent != NULL)
		::SetEvent(m_hEvent);
}

inline DWORD CEvent::Wait(DWORD dwWaitTime) const
{
	return ::WaitForSingleObject(m_hEvent, dwWaitTime);
}

inline bool CEvent::IsSignalled() const
{
	return (Wait(0)== WAIT_OBJECT_0);
}

