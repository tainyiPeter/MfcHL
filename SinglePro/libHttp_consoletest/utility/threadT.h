#pragma once
#include "auxtools.h"
#include "AsyncFuncCall.h"
#include "sigslot.h"

namespace pcutil
{

#if defined(_MT) || defined(_DLL)

#endif // _MT

#ifndef ATLTRY
#define ATLTRY(x) (x)
#endif // ATLTRY


// CThread

template< bool t_bManaged >
class CThreadTemplate
{
public:
    CThreadTemplate( HANDLE hThread = NULL ) : m_hThread( hThread ), m_dwThreadID( 0 ), m_bSuspended( false )
    {
    }
    ~CThreadTemplate()
    {
        if ( t_bManaged ) Release();
    }
	BOOL IsCurrent()
	{
		if( m_dwThreadID != 0 )
		{
			if( m_dwThreadID == GetCurrentThreadId() )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

    BOOL Create( LPTHREAD_START_ROUTINE pThreadProc, LPVOID pParam = NULL, int iPriority = THREAD_PRIORITY_NORMAL )
    {
        ATLASSERT( m_hThread == NULL );
        ATLASSERT( pThreadProc );
#if defined(_MT) || defined(_DLL)
        m_hThread = ( HANDLE ) _beginthreadex( NULL, 0, ( UINT ( WINAPI* )( void* ) ) pThreadProc, pParam, CREATE_SUSPENDED, ( UINT* ) & m_dwThreadID );
#else
        m_hThread = ::CreateThread( NULL, 0, pThreadProc, pParam, CREATE_SUSPENDED, &m_dwThreadID );
#endif // _MT
        if ( m_hThread == NULL ) return FALSE;
        if ( iPriority != THREAD_PRIORITY_NORMAL )
        {
            if ( !::SetThreadPriority( m_hThread, iPriority ) )
            {
                ATLASSERT( !"Couldn't set thread priority" );
            }
        }
        return ::ResumeThread( m_hThread ) != ( DWORD ) - 1;
    }

    BOOL Release()
    {
        if ( m_hThread == NULL ) return TRUE;
        ::CloseHandle( m_hThread );
        m_hThread = NULL;
        m_dwThreadID = 0;
        return TRUE;
    }

    void Attach( HANDLE hThread )
    {
        ATLASSERT( m_hThread == NULL );
        m_hThread = hThread;
    }
    HANDLE Detach()
    {
        HANDLE hThread = m_hThread;
        m_hThread = NULL;
        return hThread;
    }
    BOOL SetPriority( int iPriority ) const
    {
        ATLASSERT( m_hThread );
        return ::SetThreadPriority( m_hThread, iPriority );
    }
    int GetPriority() const
    {
        ATLASSERT( m_hThread );
        return ::GetThreadPriority( m_hThread );
    }
    BOOL Suspend()
    {
        ATLASSERT( m_hThread );
        if ( m_bSuspended ) return TRUE;
        if ( ::SuspendThread( m_hThread ) == ( DWORD ) - 1 ) return FALSE;
        m_bSuspended = true;
        return TRUE;
    }
    BOOL Resume()
    {
        ATLASSERT( m_hThread );
        if ( !m_bSuspended ) return TRUE;
        if ( ::ResumeThread( m_hThread ) == ( DWORD ) - 1 ) return FALSE;
        m_bSuspended = false;
        return TRUE;
    }
    BOOL IsSuspended() const
    {
        ATLASSERT( m_hThread );
        return m_bSuspended == true;
    }
    BOOL IsRunning() const
    {
        if ( m_hThread == NULL ) return FALSE;
        DWORD dwCode = 0;
        ::GetExitCodeThread( m_hThread, &dwCode );
        return dwCode == STILL_ACTIVE;
    }
    BOOL WaitForThread( DWORD dwTimeout = INFINITE ) const
    {
        ATLASSERT( m_hThread );
		if( m_hThread == NULL ) return TRUE;
        return ::WaitForSingleObject( m_hThread, dwTimeout ) == WAIT_OBJECT_0;
    }
    BOOL Terminate( DWORD dwExitCode = 0 ) const
    {
        ATLASSERT( m_hThread );
		if( m_hThread == NULL ) return TRUE;
        return ::TerminateThread( m_hThread, dwExitCode );
    }
    DWORD GetThreadID() const
    {
        return m_dwThreadID;
    }
    BOOL GetExitCode( DWORD* pExitCode ) const
    {
        ATLASSERT( m_hThread );
        ATLASSERT( pExitCode );
        return ::GetExitCodeThread( m_hThread, pExitCode );
    }
#if(WINVER >= 0x0500)
    BOOL GetThreadTimes( LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime ) const
    {
        ATLASSERT( m_hThread );
        ATLASSERT( lpExitTime != NULL && lpKernelTime != NULL && lpUserTime != NULL );
        return ::GetThreadTimes( m_hThread, lpCreationTime, lpExitTime, lpKernelTime, lpUserTime );
    }
#endif // WINVER
#if(WINVER >= 0x0501)
    BOOL SetThreadAffinityMask( DWORD dwThreadMask )
    {
        ATLASSERT( m_hThread );
        return ::SetThreadAffinityMask( m_hThread, dwThreadMask ) != 0;
    }
    BOOL SetThreadIdealProcessor( DWORD dwIdealProcessor )
    {
        ATLASSERT( m_hThread );
        return ::SetThreadIdealProcessor( m_hThread, dwIdealProcessor ) != ( DWORD ) - 1;
    }
    DWORD GetThreadIdealProcessor() const
    {
        ATLASSERT( m_hThread );
        return ::SetThreadIdealProcessor( m_hThread, MAXIMUM_PROCESSORS );
    }
#endif // WINVER
    operator HANDLE() const
    {
        return m_hThread;
    }

protected:
    HANDLE m_hThread;       // Handle to thread
    DWORD m_dwThreadID;     // Thread ID
    bool m_bSuspended;      // Thread currently suspended?
};

typedef CThreadTemplate<false> CThreadHandle;
typedef CThreadTemplate<true> CThreadTrue;


/////////////////////////////////////////////////////////////////////////////
// Thread Stop policy

class CThreadStopAtBool
{
public:
    volatile bool m_bStopped;
    CThreadStopAtBool() : m_bStopped( false ) { };
    BOOL _ClearAbort()
    {
        m_bStopped = false;
        return TRUE;
    };
    BOOL _Abort()
    {
        m_bStopped = true;
        return TRUE;
    };
    BOOL _IsAborted() const
    {
        return m_bStopped == true;
    };
};


/////////////////////////////////////////////////////////////////////////////
// CThreadImpl

template < typename T, typename TStopPolicy = CThreadStopAtBool >
class CThreadImpl : public CThreadTrue, protected TStopPolicy
{
public:
    bool m_bAutoDelete;     // Thread class will delete itself upon thread exit?
    bool m_bAutoCleanup;    // Thread class will wait for thread completion upon scope exit?

    CThreadImpl() : m_bAutoDelete( false ), m_bAutoCleanup( true )
    {
    }
    virtual ~CThreadImpl()
    {
        if ( m_bAutoCleanup ) Stop();
    }

    BOOL Start( int iPriority = THREAD_PRIORITY_NORMAL )
    {
        if ( !CThreadStopAtBool::_ClearAbort() ) return FALSE;
        if ( !Create( ThreadProc, ( LPVOID ) static_cast<T*>( this ) , iPriority ) ) return FALSE;
        return TRUE;
    }
    void Stop()
    {
        if ( !Abort() ) return;
        if (!IsCurrent())
			WaitForThread();
        Release();
    }
    BOOL Abort()
    {
        if ( m_hThread == NULL ) return FALSE;
        if ( !CThreadStopAtBool::_Abort() ) return FALSE;
        if ( m_bSuspended ) Resume();
        return TRUE;
    }
    BOOL IsAborted() const
    {
        //ATLASSERT( m_hThread );
        return CThreadStopAtBool::_IsAborted();
    }
    void SetAutoClean( bool bAutoClean = true )
    {
        m_bAutoCleanup = bAutoClean;
    }
    void SetDeleteOnExit( bool bAutoDelete = true )
    {
        m_bAutoDelete = bAutoDelete;
        m_bAutoCleanup = !bAutoDelete;
    }

	static DWORD DoThreadWork(T* pThis) {
		DWORD dwRet = 0;
		//__try
		//{
			dwRet = pThis->ThreadRun();
		//}
		//__except (EXCEPTION_EXECUTE_HANDLER)
		//{

		//}
		
		return dwRet;
	}

    static DWORD WINAPI ThreadProc( LPVOID pData )
    {
		T* pThis = static_cast<T*>(pData);

		DWORD dwRet = 0;
		ATLTRY(dwRet = DoThreadWork(pThis));
		if (pThis->m_bAutoDelete) delete pThis;
		return dwRet;
    }

    // Overridable

    DWORD ThreadRun()
    {
        ATLASSERT( false ); // must override this
        //
        // Sample thread loop...
        //
        //  while( !IsAborted() ) {
        //    ...
        //  }
        //
        return 0;
    }
};

//class CThreadAdapter
//	:public CThreadImpl< CThreadAdapter >
//{
//public:
//	CThreadAdapter(){}
//private:
//	DISALLOW_COPY_AND_ASSIGN( CThreadAdapter );
//
//public:
//	void BindAdapterFunction( IAsyncFuncCall *pCall )
//	{
//		ATLASSERT( _pFunc.get() == NULL );
//		_pFunc.reset( pCall );
//	}
//
//	DWORD ThreadRun()
//	{
//		//ATLASSERT( !_event.m_connected_slots.empty() );
//		ATLASSERT( _pFunc.get() != NULL );
//		_pFunc->Execute();
//		return 0;
//	}
//
//private:
//	shared_ptr<IAsyncFuncCall> _pFunc;
//
//};

class CBinderThread
	:public CThreadImpl< CBinderThread >
{
public:
	CBinderThread(){}
private:
	DISALLOW_COPY_AND_ASSIGN( CBinderThread );

public:
	typedef sigslot::signal0<sigslot::multi_threaded_local> ThreadFuncEvent;
	ThreadFuncEvent& GetThreadFuncEvent()
	{
		return _event;
	}

public:
	DWORD ThreadRun()
	{
		//ATLASSERT( !_event.m_connected_slots.empty() );
		_event();
		ATLTRACE( _T(" \r\nCBinderThread::ThreadRun Exited! [%d]" ) , GetThreadID()  );
		return 0;
	}

private:
	ThreadFuncEvent _event;
};






template< typename T >
class CBinderThreadEx
	:public CThreadImpl< CBinderThreadEx<T> >
{
public:
	typedef typename detail::ValueType<T>::Result ArgType;
	explicit CBinderThreadEx( const ArgType &arg )
	{
		_arg = arg;
	}
private:
	DISALLOW_IMPLICIT_CONSTRUCTORS( CBinderThreadEx );
public:
	typedef sigslot::signal1<const ArgType & , sigslot::multi_threaded_local> ThreadFuncEvent;
	ThreadFuncEvent& GetThreadFuncEvent()
	{
		return _event;
	}

public:
	DWORD ThreadRun()
	{
		//ATLASSERT( FALSE );
		_event( _arg );
		ATLTRACE( _T(" \r\nCBinderThread::ThreadRun Exited! [%d]" ) , CThreadTemplate::GetThreadID()  );
		return 0;
	}
	
	ArgType& GetThreadArg()
	{
		return _arg;
	}


private:
	ThreadFuncEvent _event;
	ArgType _arg;
};






}
