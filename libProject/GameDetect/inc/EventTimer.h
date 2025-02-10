#pragma once

#include <windows.h>
#include <iostream>
#include <functional>
#include <mutex>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class EventTimer
{
public:
	EventTimer(WAITORTIMERCALLBACK callBack)
	{
		m_hTimer = NULL;
		m_hTimerQueue = NULL;
		m_TimerRoutine = callBack;
		m_delay=0;
		m_period=0;
	}

	~EventTimer()
	{
		End();
	}

	void Start(DWORD delay, DWORD period, void *pData)
	{
		if (m_hTimer != NULL)
			return;

		m_delay = delay;
		m_period = period;
		m_hTimerQueue = CreateTimerQueue();
		//WT_EXECUTEINTIMERTHREAD:trigger new timer after the previous timer ends
		if (m_hTimerQueue != NULL)
		    CreateTimerQueueTimer(&m_hTimer, m_hTimerQueue,m_TimerRoutine, pData, m_delay, m_period, WT_EXECUTEINTIMERTHREAD);
	}

	void End()
	{
		if (m_hTimer != NULL)
		{
			//will wait the quit of timer thread
			DeleteTimerQueueTimer(m_hTimerQueue, m_hTimer, INVALID_HANDLE_VALUE);
			m_hTimer = NULL;
		}

		if (m_hTimerQueue != NULL)
		{
			DeleteTimerQueue(m_hTimerQueue);
			m_hTimerQueue = NULL;
		}
	}

	void ChangeTimer(DWORD period)
	{
		if (m_period != period)
		{
			m_period = period;
			ChangeTimerQueueTimer(m_hTimerQueue, m_hTimer, m_delay, m_period);
		}
	}

private:
	DWORD m_delay;
	DWORD m_period;
	WAITORTIMERCALLBACK m_TimerRoutine;
	HANDLE m_hTimerQueue;
	HANDLE m_hTimer;
};

