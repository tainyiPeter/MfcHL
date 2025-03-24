#pragma once
#include "ClientConfig.h"
#include "Monitor.h"
#include "threadT.h"
//#include "CommonHelper.h"
#include "CStringHelper.h"

namespace pcutil
{
class WEAVERLIB_API_UTILS CBinderThreadPool:
	public sigslot::has_slots< sigslot::multi_threaded_local >
{
public:	
	virtual ~CBinderThreadPool();

private:
	CBinderThreadPool();
	int Start( int nMaxThreadCount = 2 , int nThreadPriority = THREAD_PRIORITY_BELOW_NORMAL );
	void Stop();
public:
	BOOL AddTask( IAsyncFuncCall *func );
	template <typename PointerToObj , typename PointerToMemFunc > void RemoveTask( PointerToMemFunc pMemFunc , PointerToObj pObj )
    {
        void *pObjTmp = reinterpret_cast<void*>( pObj );
        void *pMemFuncTmp = reinterpret_cast<void*>( &pMemFunc );
		RemoveTaskImpl( pMemFuncTmp , pObjTmp );
    }

	template <typename PointerToObj > void RemoveTask( PointerToObj pObj )
    {
        void *pObjTmp = reinterpret_cast<void*>( pObj );
		RemoveTaskImpl( pObjTmp );
    }

	template <typename PointerToObj , typename PointerToMemFunc , typename Param1 > void RemoveTaskWithParam1
									( PointerToMemFunc pMemFunc , PointerToObj pObj , Param1 param1 )
    {
		void *pObjTmp = reinterpret_cast<void*>( pObj );
		void *pMemFuncTmp = reinterpret_cast<void*>( &pMemFunc );
		BOOL bHasRunning = FALSE;
		try
		{
			do
			{
				bHasRunning = FALSE;
				if( TRUE )
				{
					CComCritSecLock< pcutil::CMonitor > guard( m_monitor );
					for( int i = m_deqWaitingAsyncTasks.size() - 1 ; i >= 0 ; i-- )
					{
						TAsyncFuncItem &item = m_deqWaitingAsyncTasks[i];
						unsigned char *pItemObjTmp = (unsigned char *)item.m_funcPtr->GetObjectPointer();
						unsigned char *pItemFuncTmp = *(unsigned char **)item.m_funcPtr->GetMemberFunctionPointer();
						if( pObjTmp == pItemObjTmp && pItemFuncTmp == ( *(unsigned char**)pMemFuncTmp ) ) 
						{
							typedef typename detail::ValueType<Param1>::Result Param1Type;
							CAny any1 = item.m_funcPtr->GetMemberFunctionParam1();
							if( !any1.empty() )
							{
								tuple1<Param1Type> *paramitem1 = any_cast< tuple1< Param1Type > >( &any1 );
								if( paramitem1 != NULL )
								{
									if( paramitem1->v1() == param1 )
									{
										m_deqWaitingAsyncTasks.erase( m_deqWaitingAsyncTasks.begin() + i );
									}
								}
							}
						}
					}

					for( int i = m_deqRunningAsyncTasks.size() - 1 ; i >= 0 ; i-- )
					{
						TAsyncFuncItem &item = m_deqRunningAsyncTasks[i];
						unsigned char *pItemObjTmp = (unsigned char *)item.m_funcPtr->GetObjectPointer();
						unsigned char *pItemFuncTmp = *(unsigned char **)item.m_funcPtr->GetMemberFunctionPointer();
						if( pObjTmp == pItemObjTmp && pItemFuncTmp == ( *(unsigned char**)pMemFuncTmp ) ) 
						{
							typedef typename detail::ValueType<Param1>::Result Param1Type;
							CAny any1 = item.m_funcPtr->GetMemberFunctionParam1();
							if( !any1.empty() )
							{
								tuple1<Param1Type> *paramitem1 = any_cast< tuple1< Param1Type > >( &any1 );
								if( paramitem1 != NULL )
								{
									if( paramitem1->v1() == param1 )
									{
										bHasRunning = TRUE;
										break;
									}
								}
							}
						}
					}
				}
				if( bHasRunning )
				{
					Sleep( 0 );
				}

			}while( bHasRunning );
		}
		catch(...)
		{
		}
    }





public:
	static CBinderThreadPool* GetInstance();
protected:
	void ThreadPoolRun();
	virtual void RemoveTaskImpl( void *pObj );
	virtual void RemoveTaskImpl( void *pMemFunc , void* pObj );

	BOOL IsCloseAll( BOOL bLock = TRUE );
private:
	struct TAsyncFuncItem
	{
		TAsyncFuncItem();
		//{
		//	//CommonHelper::GenerateUUID(m_strID);
		//	CStringHelper::GenerateUUID( m_strID );
		//}
		CString m_strID;
		pcutil::CSharedPtr< pcutil::IAsyncFuncCall > m_funcPtr; 
	};

private:
	volatile BOOL m_bCloseAll;
	std::vector< CSharedPtr< CBinderThread > > m_vecThreads;
	std::deque< TAsyncFuncItem > m_deqWaitingAsyncTasks;
	std::deque< TAsyncFuncItem > m_deqRunningAsyncTasks;
	pcutil::CMonitor  m_monitor;
	volatile DWORD m_dwWaitingAsyncRequestReaders;
};

#ifdef _DEBUG
//void TestBinderThreadPool();
#endif



}
