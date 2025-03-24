#pragma once
#include "ClientConfig.h"
#include <atlsync.h>

/*
*	模仿POSIX线程条件变量的实现

*/

class CMonitor;
namespace pcutil
{

class WEAVERLIB_API_UTILS CCondition
{
public:
	CCondition();
	~CCondition();
    friend class CMonitor;

public:
    void signal();
    void broadcast();

public:
   template <typename M> void
    waitImpl(M& mutex)
    {
        _preWait();

        mutex.Unlock();

        try
        {
            _dowait();
            mutex.Lock();
        }
        catch(...)
        {
            mutex.Lock();
        }
    }
    template <typename M> bool
    timedWaitImpl(M& mutex, DWORD timeout)
    {
        _preWait();
		mutex.Unlock();
        try
        {
            bool rc = _timedDowait(timeout);
            mutex.Lock();
            return rc;
        }
        catch(...)
        {
            mutex.Lock();
        }
		return false;
    }



private:
    void _wake(bool);
    void _preWait() ;
    void _postWait(bool) ;
    bool _timedDowait(DWORD timeout );
    void _dowait() ;

private:
    CComAutoCriticalSection m_internal;
	ATL::CSemaphore m_gate;
	ATL::CSemaphore m_queue;

	mutable long m_blocked;
    mutable long m_unblocked;
    mutable long m_toUnblock;

};





//class CCondition 
//{
//public:
//    CCondition();
//    ~CCondition();
//public:
//     void signal();
//	 void broadcast();
//
//public:
//    friend class CMonitor;
//
//	template <typename M> void
//    waitImpl(M& mutex)
//    {
//        preWait();
//        mutex.Unlock();
//        try
//        {
//            dowait();
//            mutex.Lock();
//        }
//        catch(...)
//        {
//            mutex.Lock();
//        }
//    }
//    template <typename M> bool
//    timedWaitImpl(M& mutex, DWORD timeout)
//    {
//		preWait();
//		mutex.Unlock();
//        try
//        {
//            bool rc = timedDowait(timeout);
//            mutex.Lock();
//            return rc;
//        }
//        catch(...)
//        {
//            mutex.Lock();
//        }
//		return false;
//    }
//    
//private:
//    void wake(bool);
//    void preWait() ;
//    void postWait(bool) ;
//    bool timedDowait(DWORD timeout) ;
//    void dowait();
//
//	CComAutoCriticalSection _internal;
//	ATL::CSemaphore _gate;
//	ATL::CSemaphore _queue;
//
//    mutable long _blocked;
//    mutable long _unblocked;
//    enum State
//    {
//        StateIdle,
//        StateSignal,
//        StateBroadcast
//    };
//    mutable State _state;
//
//
//};
//










}