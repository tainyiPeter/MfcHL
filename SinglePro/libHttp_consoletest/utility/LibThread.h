// Copyright @<1st year created> Lenovo. All Rights reserved.
// Confidential and Proprietary.
#pragma once
#include "threadT.h"

class CLibThreadEvent
{

public:
	HANDLE m_hStopEvent;
	CLibThreadEvent()
	{
		m_hStopEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	};
	~CLibThreadEvent()
	{
		::CloseHandle(m_hStopEvent);
	};
	BOOL ResetEvent()
	{
		return ::ResetEvent(m_hStopEvent);
	};
	BOOL SetEvent()
	{
		return ::SetEvent(m_hStopEvent);
	};
	BOOL Wait(DWORD milliseconds = INFINITE) const
	{
		return ::WaitForSingleObject(m_hStopEvent, milliseconds) != WAIT_TIMEOUT;
	};
	void Sleep(DWORD milliSeconds)
	{
		::Sleep(milliSeconds);
	}
};


class CLibThread
	:public pcutil::CThreadImpl< CLibThread >, public CLibThreadEvent
{

	enum EThreadPriority
	{
		THREAD_LOWEST = 0,
		THREAD_BELOW_NORMAL = 1,
		THREAD_NORMAL = 2,
		THREAD_ABOVE_NORMAL = 3,
		THREAD_HIGHEST = 4,

	};
public:
	char threadName[64] = "LibThread";
	CLibThread(pcutil::IAsyncFuncCall* threadCall, EThreadPriority priority = THREAD_NORMAL)
	{
		m_threadCall = threadCall;
		m_priority = priority;
	}

	BOOL Start()
	{
		//参考winbase的宏定义，调用接口时需要 -2
		return CThreadImpl<CLibThread>::Start(m_priority - 2);
	}

	const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)
	void SetThreadName(const char* threadName, DWORD dwThreadID = -1) {
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
		__try {
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}
#pragma warning(pop)
	}
	static CLibThread* Start(pcutil::IAsyncFuncCall* threadCall, bool pjthread = true)
	{
		//之前开启一个线程，此处需要由自己处理析构
		CLibThread* thread = new CLibThread(threadCall);
		thread->m_bAutoDelete = TRUE;
		thread->m_pjThread = pjthread;
		thread->Start();
		return thread;
	}
public:
	bool m_pjThread;
private:
	pcutil::IAsyncFuncCall* m_threadCall;
	EThreadPriority m_priority;
public:
	DWORD ThreadRun()
	{
		SetThreadName(threadName);
		m_threadCall->Execute();

		return 0;
	}
	~CLibThread()
	{
		delete m_threadCall;

	}
};

