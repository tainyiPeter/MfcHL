#pragma once
#ifndef _pcutil_detail_shared
#define _pcutil_detail_shared

namespace pcutil
{

	namespace detail_shared
	{
		template<typename T>
		class shared_count
		{
		public:
			shared_count():m_refCount(1),m_ptr( NULL )
			{
			}
			shared_count( T *ptr ):m_refCount(1),m_ptr( ptr )
			{
			}

		protected:
			~shared_count()
			{
				if( m_ptr != NULL )
				{
					delete m_ptr;
				}
			}

		public:
			void release()
			{
				ATLASSERT( InterlockedExchangeAdd( &m_refCount, 0) > 0 );
				if( InterlockedDecrement( &m_refCount) == 0 )
				{
					delete this;
				}
			}

			void addref()
			{
				ATLASSERT( InterlockedExchangeAdd( &m_refCount, 0 ) >= 0 );
				InterlockedIncrement( &m_refCount );
			}

			long getref()
			{
				return ::InterlockedExchangeAdd( &m_refCount , 0 );
			}

		private:
			volatile long m_refCount;
			shared_count( shared_count const & );
			shared_count & operator= ( shared_count const & );

		public:
			T* m_ptr;
		};


	}

	template< typename T > class CSharedPtr
	{
	public:
		typedef T& reference;
		typedef CSharedPtr<T> this_type;

	public:
		CSharedPtr():m_ptr( NULL ) , m_pc( NULL )
		{

		}
		/*explicit */CSharedPtr( T *ptr ):m_ptr( ptr ), m_pc( NULL )
		{
			//ATLASSERT( ptr != NULL );
			m_pc = new detail_shared::shared_count<T>( ptr );
			//ATLASSERT( m_pc != NULL );
		}

		/*template<class T>*/
		CSharedPtr( CSharedPtr const &r ): m_ptr( r.m_ptr ), m_pc( r.m_pc )
		{
			if( m_pc )
			{
				m_pc->m_ptr = m_ptr;
				m_pc->addref();
			}
		}

		template<class Other>
		CSharedPtr( CSharedPtr< Other > /*const*/ &r )//: m_ptr( r.m_ptr ), m_pc( r.m_pc )
		{
			m_ptr = static_cast< T* >(r.m_ptr);
			m_pc = reinterpret_cast< detail_shared::shared_count<T> * >(r.m_pc);
			if( m_pc )
			{
				m_pc->m_ptr = m_ptr;
				m_pc->addref();
			}
		}

		template<class Other>
		CSharedPtr& operator=( CSharedPtr<Other> const &r )
		{
			unsigned char* pThis = (unsigned char*)(this);
			unsigned char* pOther = (unsigned char*)(&r);
			if( pThis != pOther )
			{
				if( m_pc ) m_pc->release();
				m_ptr = static_cast< T* >(r.m_ptr);
				m_pc = reinterpret_cast< detail_shared::shared_count<T> * >(r.m_pc);
				if( m_pc )
				{
					m_pc->m_ptr = m_ptr;
					m_pc->addref();
				}
			}
			return *this;
		}


		CSharedPtr& operator=( CSharedPtr const &r )
		{
			if( this != &r )
			{
				if( m_pc ) m_pc->release();
				m_ptr = r.m_ptr ;
				m_pc = r.m_pc; 
				if( m_pc )
				{
					m_pc->m_ptr = m_ptr;
					m_pc->addref();
				}
			}
			return *this;
		}


		~CSharedPtr()
		{
			if( m_pc )
			{
				m_pc->release();
			}
		}
	public:
		long use_count()
		{
			long ret = 0;
			if( m_pc )
			{
				ret = m_pc->getref();
			}
			return ret;
		}

	public:
		void reset()
		{
			this_type().swap( *this );
		}

		template<class Y> void reset(Y * p)		
		{
			ATLASSERT( p == 0 || p != m_ptr );	
			this_type(p).swap(*this);
		}

	public:
		T * get() const
		{
			//		ATLASSERT( m_ptr != NULL );
			return m_ptr;
		}

		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		T** operator&() throw()
		{
			//ATLASSERT(m_ptr==NULL);
			return &m_ptr;
		}

		T * operator-> () const
		{
			ATLASSERT( m_ptr != NULL );
			return m_ptr;
		}

		T& operator* () const
		{
			ATLASSERT( m_ptr != NULL );
			return *m_ptr;
		}

	private:
		void swap( CSharedPtr<T> & other )
		{
			std::swap( m_ptr, other.m_ptr );
			std::swap( m_pc, other.m_pc );
		}


	public:
		T*	m_ptr;	
		detail_shared::shared_count<T> *m_pc;
	};

	template<class T, class U> inline bool operator==(CSharedPtr<T> const & a, CSharedPtr<U> const & b)
	{
		return a.get() == b.get();
	}

	template<class T, class U> inline bool operator!=(CSharedPtr<T> const & a, CSharedPtr<U> const & b)
	{
		return a.get() != b.get();
	}

	template<class T, class U> inline bool operator<(CSharedPtr<T> const & a, CSharedPtr<U> const & b)
	{
		return a.get() < b.get();
	}


}

#endif