//#include "Stdafx.h"
#include <atlbase.h>
#include <atlstr.h>
#include <string>  

#include "LibThread.h"
#include "GlobalTimer.h"
#include "CSyncWindow.h"
//#include "LibBiz.h"
using namespace pcutil;


CGlobalTimer::CGlobalTimer()
{
	m_thread = new CLibThread(bindAsyncFunc(&CGlobalTimer::ThreadCall, this));
	m_thread->Start();
}

void CGlobalTimer::DeleteTimerImpl(void *object, void* callback)
{
	CComCritSecLock< ATL::CComAutoCriticalSection  > guard(m_monitor);
	for (int i = m_calls.size() - 1; i >= 0; i--)
	{
		CSharedPtr< pcutil::IAsyncFuncCall > item = m_calls[i]->m_call;
		if (item->GetObjectPointer() == object)
		{
			if (callback == NULL ||
				*(void**)item->GetMemberFunctionPointer() == *(void**)callback)
			{
				m_calls[i]->m_stoped = TRUE;
				m_calls.erase(m_calls.begin() + i);
			}
		}
	}
}

void CGlobalTimer::DeleteTimer(IAsyncFuncCall *callback)
{
	CComCritSecLock< ATL::CComAutoCriticalSection  > guard(m_monitor);
	for (int i = m_calls.size() - 1; i >= 0; i--)
	{
		CSharedPtr< pcutil::IAsyncFuncCall > item = m_calls[i]->m_call;

		if (item->GetObjectPointer() == callback->GetObjectPointer()
			&& *(void**)item->GetMemberFunctionPointer() == *(void**)callback->GetMemberFunctionPointer())
		{
			m_calls[i]->m_stoped = TRUE;
			m_calls.erase(m_calls.begin() + i);
		}
	}
}

void CGlobalTimer::ThreadCall()
{
	while (true)
	{
		//需要锁析构 否则会造成死锁
		BOOL wait = FALSE;
		if (TRUE)
		{
			if (m_thread->IsAborted())
			{
				return;
			}
			CComCritSecLock< ATL::CComAutoCriticalSection > guard(m_monitor);
			if (m_calls.size() == 0)
			{
				wait = TRUE;
			}
		}
		if (wait)
		{
			m_thread->Wait();
		}

		m_thread->Sleep(10);

		OnThreadTimer();
	}
}

//此时是在界面主线程
void CGlobalTimer::OnThreadTimer()
{
	if (m_thread->IsAborted())
	{
		return;
	}
	CComCritSecLock< ATL::CComAutoCriticalSection  > guard(m_monitor);
	for (int i = m_calls.size() - 1; i >= 0; i--)
	{
		DWORD currentTick = GetTickCount();
		CSharedPtr< TTimerCallBack > item = m_calls[i];
		if ((currentTick - item->m_lastTime) >= item->m_interval)
		{
			if (item->m_stoped)
			{
				m_calls.erase(m_calls.begin() + i);
				continue;
			}
			if (item->m_loop)
			{
				item->m_lastTime = currentTick;
			}

			if (item->m_uiThread)
			{
				//切换到界面主线程
				SwitchToMainThread(pcutil::bindAsyncFunc(&CGlobalTimer::Execute, this, item));
			}
			else
			{
				Execute(item);
			}
		}
	}
}


void CGlobalTimer::Execute(CSharedPtr< TTimerCallBack > item)
{
	if (!item->m_stoped)
	{
		if (!item->m_loop)
		{
			item->m_stoped = TRUE;
		}
		item->m_call->Execute();
	}
}


CGlobalTimer::~CGlobalTimer()
{
	CComCritSecLock< ATL::CComAutoCriticalSection > guard(m_monitor);
	m_thread->Abort();
}

BOOL CGlobalTimer::AddTimer(pcutil::IAsyncFuncCall *funcCall, DWORD interval, BOOL loop, BOOL uiThread)
{
	CComCritSecLock< ATL::CComAutoCriticalSection  > guard(m_monitor);
	{
		pcutil::CSharedPtr< TTimerCallBack > callback(new TTimerCallBack());
		ZeroMemory(callback.get(), sizeof(TTimerCallBack));
		callback->m_call.reset(funcCall);
		callback->m_interval = interval;
		callback->m_lastTime = GetTickCount();
		callback->m_loop = loop;
		callback->m_uiThread = uiThread;
		callback->m_stoped = FALSE;
		m_calls.push_back(callback);
		m_thread->SetEvent();
		return TRUE;
	}
}

IGlobalTimer* CGlobalTimer::GetInstance()
{
	static CGlobalTimer timer;
	return &timer;
}

