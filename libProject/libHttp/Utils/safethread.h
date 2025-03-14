#pragma once

#include <WTypes.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
using namespace std;

#include <tchar.h>
#include <cassert>
#include <process.h> 

#ifndef ASSERT
#define ASSERT	assert
#endif

template <typename _Ty, int N = 1> class SafeThreadBase
{
protected:
	typedef vector<HANDLE>	HANDLEVEC;

	typedef UINT (_Ty::*MemWorkFunc)();
	typedef UINT (_stdcall *WorkFunc)(void*);

	struct SAFETHREADPARAM
	{
		_Ty*		pThis;
		MemWorkFunc pFunc;
	} ;

#define CHECKTHREADINDEX(n) \
	ASSERT((n) >= 0 && (n) < N);	\
	if ((n) < 0 || (n) >= N) return FALSE

public:
	SafeThreadBase()
	{
		m_vecHThread.resize(N, NULL);
	}

	~SafeThreadBase() 
	{
		JoinAllThread();
	}

	BOOL SafeResetThreadHandle(int nThreadIndex)
	{
		CHECKTHREADINDEX(nThreadIndex);

		HANDLE hHandle = m_vecHThread[nThreadIndex];

		if (hHandle == NULL)
		{
			return TRUE;
		}

		m_vecHThread[nThreadIndex] = NULL;

		DWORD dwRet = WaitForSingleObject(hHandle, 0);

		if (dwRet == WAIT_TIMEOUT)		// 线程正在运行，将其加入到临时线程区
		{
			m_vecHThread.push_back(hHandle);
		}
		else
		{
			CloseHandle(hHandle);		
		}

		return TRUE;
	}

	BOOL SafeStartThread(MemWorkFunc pFunc, int nThreadIndex = 0)
	{
		CHECKTHREADINDEX(nThreadIndex);

		SafeResetThreadHandle(nThreadIndex);
		ASSERT(m_vecHThread[nThreadIndex] == NULL);

		m_vecHThread[nThreadIndex] = CreateWorkThread(pFunc);
		ASSERT(m_vecHThread[nThreadIndex] != NULL);

		return m_vecHThread[nThreadIndex] != NULL;
	}

	BOOL SafeStartThread(WorkFunc pFunc, void* pParam, int nThreadIndex = 0)
	{
		CHECKTHREADINDEX(nThreadIndex);

		SafeResetThreadHandle(nThreadIndex);
		ASSERT(m_vecHThread[nThreadIndex] == NULL);

		m_vecHThread[nThreadIndex] = CreateWorkThread(pFunc, pParam);
		ASSERT(m_vecHThread[nThreadIndex] != NULL);

		return m_vecHThread[nThreadIndex] != NULL;
	}

	BOOL IsThreadAlive(int nThreadIndex)
	{
		CHECKTHREADINDEX(nThreadIndex);

		HANDLE handle = m_vecHThread[nThreadIndex];

		if (handle == NULL)
		{
			return FALSE;
		}

		DWORD dwExitCode = 0;
		BOOL bRet = GetExitCodeThread(handle, &dwExitCode);

		if (bRet && dwExitCode == STILL_ACTIVE)
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOL IsAllThreadAlive()
	{
		for (HANDLEVEC::iterator itr = m_vecHThread.begin(); itr != m_vecHThread.end(); ++itr)
		{
			if (*itr == NULL)
			{
				return FALSE;
			}

			DWORD dwExitCode = 0;
			BOOL bRet = GetExitCodeThread(*itr, &dwExitCode);

			if (!bRet || dwExitCode != STILL_ACTIVE)
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	void JoinAllThread(DWORD dwMilliseconds = INFINITE, BOOL bTerminateIfRunning = FALSE)
	{
		HANDLEVEC vecHandle;

		remove_copy_if(m_vecHThread.begin(), m_vecHThread.end(), back_inserter(vecHandle), bind2nd(equal_to<HANDLE>(), (HANDLE)NULL));

		if (vecHandle.empty())
		{
			return;
		}

		DWORD dwRet = WaitForMultipleObjects(vecHandle.size(), &vecHandle[0], TRUE, dwMilliseconds);

		if (dwRet == WAIT_TIMEOUT && bTerminateIfRunning)
		{
			for (HANDLEVEC::iterator itr = vecHandle.begin(); itr != vecHandle.end(); ++itr)
			{
				DWORD dwCode = 0;

				GetExitCodeThread(*itr, &dwCode);

				if (dwCode == STILL_ACTIVE)
				{
					TerminateThread(*itr, 0);
				}
			}
		}

		for_each(vecHandle.begin(), vecHandle.end(), CloseHandle);
		m_vecHThread.assign(m_vecHThread.size(), NULL);
	}

	BOOL JoinThread(int nThreadIndex, DWORD dwMilliseconds = INFINITE)
	{
		CHECKTHREADINDEX(nThreadIndex);

		HANDLE hHandle = m_vecHThread[nThreadIndex];

		if (hHandle == NULL)
		{
			return TRUE;
		}

		DWORD dwRet = WaitForSingleObject(hHandle, dwMilliseconds);

		CloseHandle(hHandle);		
		m_vecHThread[nThreadIndex] = NULL;

		return TRUE;
	}

protected:
	HANDLE CreateWorkThread(MemWorkFunc func)		
	{
		SAFETHREADPARAM *pParam = new SAFETHREADPARAM;
		pParam->pThis = static_cast<_Ty*>(this);
		pParam->pFunc = func;

		return (HANDLE)_beginthreadex(NULL, 0, ThreadHelpFunc, pParam, 0, NULL);
	}

	HANDLE CreateWorkThread(WorkFunc pFunc, void* pParam)
	{
		return (HANDLE)_beginthreadex(NULL, 0, pFunc, pParam, 0, NULL);
	}

	static UINT _stdcall ThreadHelpFunc(void* pParam)
	{
		SAFETHREADPARAM *pFuncParam = (SAFETHREADPARAM*)pParam;

		UINT nRet = 0;
		__try
		{
			nRet = (pFuncParam->pThis->*pFuncParam->pFunc)();
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			OutputDebugString(_T("thread error\n"));
		}

		delete pFuncParam;
		return nRet;
	}
protected:
	HANDLEVEC	m_vecHThread;		// 线程对象
};
