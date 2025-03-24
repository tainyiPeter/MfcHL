//#include "stdafx.h"
#include "threadpool.h"
#include "UsableValue.h"
#include "CUUID.h"

namespace pcutil
{

CBinderThreadPool::CBinderThreadPool()
{
	m_bCloseAll = FALSE;
	m_dwWaitingAsyncRequestReaders = 0;
}

CBinderThreadPool::~CBinderThreadPool()
{
	//Stop();
}

BOOL CBinderThreadPool::Start( int nMaxThreadCount , int nThreadPriority  )
{
	ATLASSERT( !m_bCloseAll && nMaxThreadCount > 0 );
	for( int i = 0 ; i < nMaxThreadCount ; i++ )
	{
		CSharedPtr< CBinderThread > spThread = new CBinderThread();
		if( spThread.get() != NULL )
		{
			spThread->GetThreadFuncEvent().connect( this , &CBinderThreadPool::ThreadPoolRun );
		}
		m_vecThreads.push_back( spThread );
		spThread->Start( nThreadPriority );
	}	
	return TRUE;
}

void CBinderThreadPool::Stop()
{
	try
	{
		if( TRUE )
		{
			CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
			if( m_bCloseAll ) return;
			m_bCloseAll = TRUE;
			m_deqWaitingAsyncTasks.clear();
		}

		while( m_dwWaitingAsyncRequestReaders > 0 )
		{
			if( TRUE )
			{
				CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
				m_monitor.Notify();
			}
			Sleep(0);
		}

		for( size_t i = 0 ; i < m_vecThreads.size() ; i++ )
		{
			m_vecThreads[i]->Stop();
		}

		if( TRUE )
		{
			CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
			m_deqRunningAsyncTasks.clear();
		}		
		m_vecThreads.clear();
	}
	catch(...)
	{
	
	}

}

BOOL CBinderThreadPool::IsCloseAll( BOOL bLock )
{
	if( bLock )
	{
		CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
		return m_bCloseAll;
	}
	return m_bCloseAll;
}

BOOL CBinderThreadPool::AddTask( IAsyncFuncCall *func )
{
	try
	{
		if( func == NULL ) return FALSE;
		CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
		if( IsCloseAll( FALSE ) ) return FALSE;
		CBinderThreadPool::TAsyncFuncItem item;
		item.m_funcPtr.reset( func );
		m_deqWaitingAsyncTasks.push_back( item );
		if( m_dwWaitingAsyncRequestReaders > 0 )
			m_monitor.Notify();
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}

void CBinderThreadPool::ThreadPoolRun()
{
	while( !IsCloseAll(TRUE) )
	{
		CUsableValue< CBinderThreadPool::TAsyncFuncItem > runItem;
		try
		{
			if( TRUE )
			{
				CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
				while( m_deqWaitingAsyncTasks.size() == 0 && !IsCloseAll( FALSE ) )
				{
					try
					{
						++m_dwWaitingAsyncRequestReaders;
						m_monitor.Wait();
						--m_dwWaitingAsyncRequestReaders;
					}
					catch(...)
					{
						--m_dwWaitingAsyncRequestReaders;
					}
				}
				if( IsCloseAll( FALSE ) ) return;
				CBinderThreadPool::TAsyncFuncItem &firstItem = m_deqWaitingAsyncTasks.front();
				runItem = firstItem;
				m_deqWaitingAsyncTasks.pop_front();
				m_deqRunningAsyncTasks.push_front( runItem.GetValue() );
			}

			if( IsCloseAll( TRUE ) ) return;
			
			try
			{
				if( runItem.IsUsable() )
				{
					runItem.GetValue().m_funcPtr->Execute();
					Sleep(0);
				}
			}
			catch(...)
			{
			}
			try
			{
				if( runItem.IsUsable() )
				{
					CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
					for( size_t i = 0 ; i < m_deqRunningAsyncTasks.size(); i++ )
					{
						if( m_deqRunningAsyncTasks[i].m_strID == runItem.GetValue().m_strID )
						{
							m_deqRunningAsyncTasks.erase( m_deqRunningAsyncTasks.begin() + i );
							break;
						}
					}
				}
			}
			catch(...)
			{
			}
		}
		catch(...)
		{
		}


	}
}


void CBinderThreadPool::RemoveTaskImpl( void *pObj )
{
	try
	{
		BOOL bHasRunning = FALSE;
		unsigned char *pObjTmp = (unsigned char *)pObj;
		do
		{
			bHasRunning = FALSE;
			if( TRUE )
			{
				CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
				for( int i = ( m_deqWaitingAsyncTasks.size() - 1 ) ; i >= 0 ; --i )
				{
					CBinderThreadPool::TAsyncFuncItem &item = m_deqWaitingAsyncTasks[i];
					unsigned char *pItemObjTmp = (unsigned char *)item.m_funcPtr->GetObjectPointer();
					if( pObjTmp == pItemObjTmp ) 
					{
						m_deqWaitingAsyncTasks.erase( m_deqWaitingAsyncTasks.begin() + i );
					}
				}

				for( int i = ( m_deqRunningAsyncTasks.size() - 1 ) ; i >= 0 ; --i )
				{
					CBinderThreadPool::TAsyncFuncItem &item = m_deqRunningAsyncTasks[i];
					unsigned char *pItemObjTmp = (unsigned char *)item.m_funcPtr->GetObjectPointer();
					if( pObjTmp == pItemObjTmp ) 
					{
						bHasRunning = TRUE;
						break;
					}
				}
			}
			if( bHasRunning )
			{
				Sleep( 0 );
			}
		}while( bHasRunning );
	}
	catch( ... )
	{
	}
}

void CBinderThreadPool::RemoveTaskImpl( void *pMemFunc , void* pObj )
{
	try
	{
		BOOL bHasRunning = FALSE;
		unsigned char *pObjTmp = (unsigned char *)pObj;
		unsigned char *pFuncTmp = *(unsigned char **)pMemFunc;
		do
		{
			bHasRunning = FALSE;
			if( TRUE )
			{
				CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
				for( int i = ( m_deqWaitingAsyncTasks.size() - 1 ) ; i >= 0 ; --i )
				{
					CBinderThreadPool::TAsyncFuncItem &item = m_deqWaitingAsyncTasks[i];
					unsigned char *pItemObjTmp = (unsigned char *)item.m_funcPtr->GetObjectPointer();
					unsigned char *pItemFuncTmp = *(unsigned char **)item.m_funcPtr->GetMemberFunctionPointer();
					if( pObjTmp == pItemObjTmp && pItemFuncTmp == pFuncTmp ) 
					{
						m_deqWaitingAsyncTasks.erase( m_deqWaitingAsyncTasks.begin() + i );
					}
				}

				for( int i = ( m_deqRunningAsyncTasks.size() - 1 ) ; i >= 0 ; --i )
				{
					CBinderThreadPool::TAsyncFuncItem &item = m_deqRunningAsyncTasks[i];
					unsigned char *pItemObjTmp = (unsigned char *)item.m_funcPtr->GetObjectPointer();
					unsigned char *pItemFuncTmp = *(unsigned char **)item.m_funcPtr->GetMemberFunctionPointer();
					if( pObjTmp == pItemObjTmp && pItemFuncTmp == pFuncTmp ) 
					{
						bHasRunning = TRUE;
						break;
					}
				}
			}

			if( bHasRunning )
			{
				Sleep( 0 );
			}
		}while( bHasRunning );
	}
	catch( ... )
	{
	}
}

CBinderThreadPool* CBinderThreadPool::GetInstance()
{
	static CBinderThreadPool pool;
	static BOOL bFirst = TRUE;
	if( bFirst )
	{
		bFirst = FALSE;
		pool.Start( 5 );
	}
	return &pool;
}

#ifdef _DEBUG
class CBinderThreadPoolTest
{
public:
	CBinderThreadPoolTest(){}
	~CBinderThreadPoolTest(){}

public:
	void Trace( int n )
	{
		ATLTRACE( _T("\r\nTrace Run %d ThreadID = %d") , n , GetCurrentThreadId() );
		Sleep(2000);
	}

};

//void TestBinderThreadPool()
//{
//	CBinderThreadPoolTest test;
//	CBinderThreadPool pool;
//	pool.Start( 5 );
//	for( int i = 0 ; i < 10 ; i++ )
//	{
//		pool.AddTask( pcutil::bindAsyncFunc( &CBinderThreadPoolTest::Trace , &test , i ) );
//	}
//	Sleep(10);
//	pool.RemoveTaskWithParam1( &CBinderThreadPoolTest::Trace , &test , 8 );
//	Sleep( 1000 * 15 );
//	pool.Stop();
//}
#endif
CBinderThreadPool::TAsyncFuncItem::TAsyncFuncItem()
{
	//CommonHelper::GenerateUUID(m_strID)
	SmartEngine::CUUID::GenerateUUID(m_strID);
}
}