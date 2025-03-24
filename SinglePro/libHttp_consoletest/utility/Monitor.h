#pragma once
#include "ClientConfig.h"
#include "Condition.h"

namespace pcutil
{

class WEAVERLIB_API_UTILS CMonitor
{
public:
	CMonitor();
	~CMonitor();
private:
	CMonitor( const CMonitor &other );
	CMonitor& operator=( const CMonitor &other );

public:
	HRESULT Lock();
	HRESULT Unlock();
	//void TryLock();

    void Wait();
    bool TimedWait( DWORD dwTimeout );
    void Notify();
    void NotifyAll();


private:
	void _NotifyImpl( int nnotify );

private:
	CComAutoCriticalSection m_mutex;
	CCondition m_cond;
	volatile int m_nnotify;

};

}