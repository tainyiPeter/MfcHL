#pragma once
#include "Any.h"
#include "tuples.h"
#include <deque>
#include <atlstr.h>
#include "shared_ptr.h"


namespace pcutil
{

	class IAsyncFuncCall
	{
	public:
		virtual ~IAsyncFuncCall() {}
		CString m_context;
	public:
		virtual void Execute() = 0;
		virtual void* GetObjectPointer() = 0 ;
		virtual void* GetMemberFunctionPointer() = 0;
		virtual CAny GetMemberFunctionParam1(){ return CAny(); }

	};

	template < typename PointerToObj ,
		typename PointerToMemFunc >
	class CAsyncMemFun0: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun0( PointerToObj pObj, PointerToMemFunc pMemFunc )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc )
		{
		}
	public:
		virtual void Execute()
		{
			if ( pObj_ != NULL )
			{
				( ( *pObj_ ).*pMemFunc_ )();				
			}
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_ );
		}

	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;

	};

	template < typename PointerToObj ,
		typename PointerToMemFunc , typename T1 >
	class CAsyncMemFun1: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun1( PointerToObj pObj, PointerToMemFunc pMemFunc , T1 p1 )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc ),
			p1_( p1 )
		{
		}
	public:
		virtual void Execute()
		{
			( ( *pObj_ ).*pMemFunc_ )( p1_.v1() );
			return ;
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_ );
		}
		virtual CAny GetMemberFunctionParam1()
		{ 
			CAny ret;
			ret = p1_;
			return ret;
		}

	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;
		tuple1<T1> p1_;
	};

	template < typename PointerToObj ,
		typename PointerToMemFunc ,
		typename T1 ,
		typename T2 >
	class CAsyncMemFun2: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun2( PointerToObj pObj, PointerToMemFunc pMemFunc , T1 p1 , T2 p2 )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc ),
			p2_( p1 , p2 )
		{
		}
	public:
		virtual void Execute()
		{
			( ( *pObj_ ).*pMemFunc_ )( p2_.v1() , p2_.v2() );
			return ;
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_  );
		}

	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;
		tuple2<T1, T2> p2_;
	};


	template < typename PointerToObj ,
		typename PointerToMemFunc ,
		typename T1 ,
		typename T2 ,
		typename T3 >
	class CAsyncMemFun3: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun3( PointerToObj pObj, PointerToMemFunc pMemFunc , T1 p1 , T2 p2 , T3 p3 )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc ),
			p3_( p1 , p2 , p3 )
		{
		}
	public:
		virtual void Execute()
		{
			( ( *pObj_ ).*pMemFunc_ )( p3_.v1() , p3_.v2() , p3_.v3() );
			return ;
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_  );
		}


	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;
		tuple3<T1, T2, T3> p3_;
	};


	template < typename PointerToObj ,
		typename PointerToMemFunc ,
		typename T1 ,
		typename T2 ,
		typename T3 ,
		typename T4 >
	class CAsyncMemFun4: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun4( PointerToObj pObj,
			PointerToMemFunc pMemFunc ,
			T1 p1 , T2 p2 , T3 p3 , T4 p4 )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc ),
			p4_( p1 , p2 , p3 , p4 )
		{
		}
	public:
		virtual void Execute()
		{
			( ( *pObj_ ).*pMemFunc_ )( p4_.v1() , p4_.v2() , p4_.v3() , p4_.v4() );
			return ;
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_  );
		}

	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;
		tuple4<T1, T2, T3, T4> p4_;
	};


	template < typename PointerToObj ,
		typename PointerToMemFunc ,
		typename T1 ,
		typename T2 ,
		typename T3 ,
		typename T4 ,
		typename T5 >
	class CAsyncMemFun5: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun5( PointerToObj pObj,
			PointerToMemFunc pMemFunc ,
			T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5 )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc ),
			p5_( p1 , p2 , p3 , p4 , p5 )
		{
		}
	public:
		virtual void Execute()
		{
			( ( *pObj_ ).*pMemFunc_ )( p5_.v1() , p5_.v2() , p5_.v3() , p5_.v4() , p5_.v5() );
			return ;
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_  );
		}

	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;
		tuple5<T1, T2, T3, T4, T5> p5_;
	};


	template < typename PointerToObj ,
		typename PointerToMemFunc ,
		typename T1 ,
		typename T2 ,
		typename T3 ,
		typename T4 ,
		typename T5 ,
		typename T6 >
	class CAsyncMemFun6: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun6( PointerToObj pObj,
			PointerToMemFunc pMemFunc ,
			T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5,
			T6 p6 )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc ),
			p6_( p1 , p2 , p3 , p4 , p5 , p6 )
		{
		}
	public:
		virtual void Execute()
		{
			( ( *pObj_ ).*pMemFunc_ )( p6_.v1() , p6_.v2() , p6_.v3() , p6_.v4() , p6_.v5() , p6_.v6() );
			return ;
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_ );
		}

	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;
		tuple6<T1, T2, T3, T4, T5, T6> p6_;
	};



	template < typename PointerToObj ,
		typename PointerToMemFunc ,
		typename T1 ,
		typename T2 ,
		typename T3 ,
		typename T4 ,
		typename T5 ,
		typename T6 ,
		typename T7 >
	class CAsyncMemFun7: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun7( PointerToObj pObj,
			PointerToMemFunc pMemFunc ,
			T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5,
			T6 p6 , T7 p7 )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc ),
			p7_( p1 , p2 , p3 , p4 , p5 , p6 , p7 )
		{
		}
	public:
		virtual void Execute()
		{
			( ( *pObj_ ).*pMemFunc_ )( p7_.v1() , p7_.v2() , p7_.v3() , p7_.v4() , p7_.v5() , p7_.v6() , p7_.v7()  );
			return ;
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_  );
		}

	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;
		tuple7<T1, T2, T3, T4, T5, T6, T7> p7_;
	};


	template < typename PointerToObj ,
		typename PointerToMemFunc ,
		typename T1 ,
		typename T2 ,
		typename T3 ,
		typename T4 ,
		typename T5 ,
		typename T6 ,
		typename T7 ,
		typename T8 >
	class CAsyncMemFun8: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun8( PointerToObj pObj,
			PointerToMemFunc pMemFunc ,
			T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5,
			T6 p6 , T7 p7 , T8 p8 )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc ),
			p8_( p1 , p2 , p3 , p4 , p5 , p6 , p7 , p8 )
		{
		}
	public:
		virtual void Execute()
		{
			( ( *pObj_ ).*pMemFunc_ )( p8_.v1() , p8_.v2() , p8_.v3() , p8_.v4() , p8_.v5() , p8_.v6() , p8_.v7() , p8_.v8() );
			return ;
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_  );
		}
	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;
		tuple8<T1, T2, T3, T4, T5, T6, T7, T8> p8_;
	};


	template < typename PointerToObj ,
		typename PointerToMemFunc ,
		typename T1 ,
		typename T2 ,
		typename T3 ,
		typename T4 ,
		typename T5 ,
		typename T6 ,
		typename T7 ,
		typename T8 ,
		typename T9 >
	class CAsyncMemFun9: public IAsyncFuncCall
	{
	public:
		CAsyncMemFun9( PointerToObj pObj,
			PointerToMemFunc pMemFunc ,
			T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5,
			T6 p6 , T7 p7 , T8 p8 , T9 p9 )
			: pObj_( pObj ),
			pMemFunc_( pMemFunc ),
			p9_( p1 , p2 , p3 , p4 , p5 , p6 , p7 , p8 , p9 )
		{
		}
	public:
		virtual void Execute()
		{
			( ( *pObj_ ).*pMemFunc_ )( p9_.v1() , p9_.v2() , p9_.v3() , p9_.v4() , p9_.v5() , p9_.v6() , p9_.v7() , p9_.v8() , p9_.v9() );
			return ;
		}
		virtual void* GetObjectPointer()
		{
			return reinterpret_cast<void*>( pObj_ );
		}
		virtual void* GetMemberFunctionPointer()
		{
			return reinterpret_cast<void*>( &pMemFunc_  );
		}

	private:
		PointerToObj pObj_;
		PointerToMemFunc pMemFunc_;
		tuple9<T1, T2, T3, T4, T5, T6, T7, T8, T9> p9_;
	};




	template <typename PointerToObj , typename PointerToMemFunc >
	IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj  )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun0< PointerToObj , PointerToMemFunc >( pObj , pMemFunc );
		return pAsync;
	}

	template <typename PointerToObj , typename PointerToMemFunc , typename T1 >
	IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj , T1 p1 )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun1< PointerToObj , PointerToMemFunc , T1 >( pObj , pMemFunc , p1 );
		return pAsync;
	}

	template <typename PointerToObj , typename PointerToMemFunc , typename T1 , typename T2 >
	IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj , T1 p1 , T2 p2 )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun2< PointerToObj , PointerToMemFunc , T1 , T2 >( pObj , pMemFunc , p1 , p2 );
		return pAsync;
	}

	template <typename PointerToObj , typename PointerToMemFunc , typename T1 , typename T2 , typename T3 >
	IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj , T1 p1 , T2 p2 , T3 p3 )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun3< PointerToObj , PointerToMemFunc , T1 , T2 , T3 >( pObj , pMemFunc , p1 , p2 , p3 );
		return pAsync;
	}

	template <typename PointerToObj , typename PointerToMemFunc , typename T1 , typename T2 , typename T3 , typename T4 >
	IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj , T1 p1 , T2 p2 , T3 p3 , T4 p4 )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun4< PointerToObj , PointerToMemFunc , T1 , T2 , T3 , T4 >( pObj , pMemFunc , p1 , p2 , p3 , p4 );
		return pAsync;
	}

	template < typename PointerToObj , typename PointerToMemFunc ,
		typename T1 , typename T2 , typename T3 , typename T4 ,
		typename T5	>
		IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj ,
		T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5 )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun5 < PointerToObj , PointerToMemFunc , T1 , T2 ,
			T3 , T4 , T5 > ( pObj , pMemFunc ,
			p1 , p2 , p3 , p4 , p5 );
		return pAsync;
	}


	template < typename PointerToObj , typename PointerToMemFunc ,
		typename T1 , typename T2 , typename T3 , typename T4 ,
		typename T5 , typename T6	>
		IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj ,
		T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5 , T6 p6 )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun6 < PointerToObj , PointerToMemFunc , T1 , T2 ,
			T3 , T4 , T5 , T6 > ( pObj , pMemFunc ,
			p1 , p2 , p3 , p4 , p5 , p6 );
		return pAsync;
	}


	template < typename PointerToObj , typename PointerToMemFunc ,
		typename T1 , typename T2 , typename T3 , typename T4 ,
		typename T5 , typename T6	, typename T7 >
		IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj ,
		T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5 , T6 p6 , T7 p7 )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun7 < PointerToObj , PointerToMemFunc , T1 , T2 ,
			T3 , T4 , T5 , T6 , T7 > ( pObj , pMemFunc ,
			p1 , p2 , p3 , p4 , p5 , p6 , p7 );
		return pAsync;
	}


	template < typename PointerToObj , typename PointerToMemFunc ,
		typename T1 , typename T2 , typename T3 , typename T4 ,
		typename T5 , typename T6	, typename T7 , typename T8 >
		IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj ,
		T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5 , T6 p6 , T7 p7 , T8 p8 )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun8 < PointerToObj , PointerToMemFunc , T1 , T2 ,
			T3 , T4 , T5 , T6 , T7 , T8 > ( pObj , pMemFunc ,
			p1 , p2 , p3 , p4 , p5 , p6 , p7 , p8 );
		return pAsync;
	}


	template < typename PointerToObj , typename PointerToMemFunc ,
		typename T1 , typename T2 , typename T3 , typename T4 ,
		typename T5 , typename T6	, typename T7 , typename T8 , typename T9 >
		IAsyncFuncCall* bindAsyncFunc( PointerToMemFunc pMemFunc , PointerToObj pObj ,
		T1 p1 , T2 p2 , T3 p3 , T4 p4 , T5 p5 , T6 p6 , T7 p7 , T8 p8 , T9 p9 )
	{
		IAsyncFuncCall *pAsync = new CAsyncMemFun9 < PointerToObj , PointerToMemFunc , T1 , T2 ,
			T3 , T4 , T5 , T6 , T7 , T8 , T9 > ( pObj , pMemFunc ,
			p1 , p2 , p3 , p4 , p5 , p6 , p7 , p8 , p9 );
		return pAsync;
	}

	class IAsyncFuncsDispatcher
	{
	protected:
		IAsyncFuncsDispatcher(){}
	public:
		virtual ~IAsyncFuncsDispatcher(){}

	public:
		virtual bool AddOneAsyncFunc( IAsyncFuncCall *pAsyncFunc ) = 0;
		virtual void DispathOneAsyncFunc() = 0;
		virtual void ClearAllAsyncFuncs() = 0;
		virtual int GetAsyncFuncCount() = 0;

	public:
		template <typename PointerToObj , typename PointerToMemFunc > void RemoveAsyncFuncs( PointerToMemFunc pMemFunc , PointerToObj pObj )
		{
			void *pObjTmp = reinterpret_cast<void*>( pObj );
			void *pMemFuncTmp = reinterpret_cast<void*>( &pMemFunc );
			RemoveAsyncFuncsImpl( pMemFuncTmp , pObjTmp );
		}

		template <typename PointerToObj > void RemoveAsyncFuncs( PointerToObj pObj )
		{
			void *pObjTmp = reinterpret_cast<void*>( pObj );
			RemoveAsyncFuncsImpl( pObjTmp );
		}

	protected:
		virtual void RemoveAsyncFuncsImpl( void *pMemFunc , void* pObj ) = 0;
		virtual void RemoveAsyncFuncsImpl( void* pObj ) = 0;


	};



	template< class MUTEX >
	class CAsyncFuncsDispatcherT:
		public IAsyncFuncsDispatcher
	{
	public:
		typedef MUTEX MUTEXTYPE;
	public:
		CAsyncFuncsDispatcherT()
		{

		}

		virtual ~CAsyncFuncsDispatcherT()
		{
			ClearAllAsyncFuncs();
		}

		virtual bool AddOneAsyncFunc( IAsyncFuncCall *pAsyncFunc )
		{
			try
			{
				ATLASSERT( pAsyncFunc != NULL );
				CComCritSecLock<MUTEXTYPE> guard( m_csObj );
				m_lstAsyncFunc.push_back( CSharedPtr<IAsyncFuncCall>( pAsyncFunc ) );

				//			ATLTRACE( _T("\r\nAddOneAsyncFunc Excute!! ptr = %08x , timestamp = %d , threadid = %d" ) , (DWORD)pAsyncFunc , GetTickCount() , GetCurrentThreadId() );

			}
			catch ( ... )
			{

			}
			return true;
		}

		virtual void DispathOneAsyncFunc()
		{
			/* try
			{*/
			CSharedPtr<IAsyncFuncCall> func;
			if ( true )
			{
				CComCritSecLock<MUTEXTYPE> guard( m_csObj );
				if ( !m_lstAsyncFunc.empty() )
				{
					func = m_lstAsyncFunc.front();
					m_lstAsyncFunc.pop_front();
				}
			}

			if ( func.get() != NULL )
			{
				//				ATLTRACE( _T("\r\nDispathOneAsyncFunc Excute!! ptr = %08x , timestamp = %d , threadid = %d") , (DWORD)func.get() , GetTickCount() , GetCurrentThreadId() );
				func->Execute();
			}
			/* }
			catch ( ... )
			{
			}*/
		}

		virtual void ClearAllAsyncFuncs()
		{
			try
			{
				CComCritSecLock<MUTEXTYPE> guard( m_csObj );
				m_lstAsyncFunc.clear();
			}
			catch ( ... )
			{

			}
		}

		virtual int GetAsyncFuncCount()
		{
			int nRet = 0;
			try
			{
				CComCritSecLock<MUTEXTYPE> guard( m_csObj );
				nRet = m_lstAsyncFunc.size();
			}
			catch ( ... )
			{

			}
			return nRet;
		}

		MUTEX& GetMutex()
		{
			return m_csObj;
		}
		//can't copy
	private:
		CAsyncFuncsDispatcherT( const CAsyncFuncsDispatcherT &other );
		CAsyncFuncsDispatcherT& operator=( const CAsyncFuncsDispatcherT &other );

	protected:
		class CAsyncDelFunctor
		{
		private:
			CAsyncDelFunctor();
			CAsyncDelFunctor( void *pObj , void *pMemFunc )
				: _pObj( pObj ),
				_pMemFunc( pMemFunc )
			{
				ATLASSERT( pMemFunc != NULL );
			}

			CAsyncDelFunctor( void *pObj  )
				: _pObj( pObj ),
				_pMemFunc( NULL )
			{

			}
		public:
			bool operator()( const CSharedPtr<IAsyncFuncCall> &asyncItem ) const
			{
				if ( _pMemFunc != NULL )
				{
					if ( ( *( char** )_pMemFunc ) == ( *( char** )asyncItem->GetMemberFunctionPointer() ) && _pObj == asyncItem->GetObjectPointer() )
						return true;
				}
				else
				{
					if ( _pObj == asyncItem->GetObjectPointer() )
						return true;
				}
				return false;
			}

		public:
			~CAsyncDelFunctor() {}
			friend class CAsyncFuncsDispatcherT;
		private:
			void *_pObj;
			void *_pMemFunc;

		};


	protected:
		virtual void RemoveAsyncFuncsImpl( void *pMemFuncTmp , void* pObjTmp )
		{
			try
			{
				CComCritSecLock<MUTEXTYPE> guard( m_csObj );
				std::deque< CSharedPtr<IAsyncFuncCall> >::iterator endnew = m_lstAsyncFunc.end();
				endnew = std::remove_if( m_lstAsyncFunc.begin() , m_lstAsyncFunc.end() , CAsyncDelFunctor( pObjTmp , pMemFuncTmp ) );
				m_lstAsyncFunc.erase( endnew , m_lstAsyncFunc.end() );
			}
			catch ( ... )
			{

			}
		}

		virtual void RemoveAsyncFuncsImpl( void* pObjTmp )
		{
			try
			{
				CComCritSecLock<MUTEXTYPE> guard( m_csObj );
				std::deque< CSharedPtr<IAsyncFuncCall> >::iterator endnew = m_lstAsyncFunc.end();
				endnew = std::remove_if( m_lstAsyncFunc.begin() , m_lstAsyncFunc.end() , CAsyncDelFunctor( pObjTmp ) );
				m_lstAsyncFunc.erase( endnew , m_lstAsyncFunc.end() );
			}
			catch ( ... )
			{

			}
		}

	protected:
		std::deque< CSharedPtr<IAsyncFuncCall> > m_lstAsyncFunc;
		MUTEX m_csObj;


	};


	typedef CAsyncFuncsDispatcherT<CComAutoCriticalSection> CAsyncFuncsDispatcher;
	typedef CAsyncFuncsDispatcherT<CComFakeCriticalSection> CAsyncFuncsDispatcherNotThreadSafe;



#ifdef _DEBUG
	void testAsyncFunc();
#endif


	//下面的两个函数会引起代码膨胀,尽量用强制转换参数的方式使用上面两个函数替代



}
