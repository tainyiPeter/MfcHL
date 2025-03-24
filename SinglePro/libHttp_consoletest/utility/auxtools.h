#pragma once
#include <atlbase.h>

namespace pcutil
{
	namespace detail
	{

		template <class U> struct ReferenceTraits
		{
			enum { result = false };
			typedef U ReferredType;
		};

		template <class U> struct ReferenceTraits<U&>
		{
			enum { result = true };
			typedef U ReferredType;
		};

		template <class U> struct UnConst
		{
			typedef U Result;
			enum { isConst = 0 };
		};

		template <class U> struct UnConst<const U>
		{
			typedef U Result;
			enum { isConst = 1 };
		};

		template <class U> struct ValueType
		{
			typedef typename UnConst< typename ReferenceTraits<U>::ReferredType >::Result Result;
		};

		struct NullType;

		struct EmptyType{};

		template <class U> struct PointerTraits
		{
			enum { result = false };
			typedef void* PointeeType;
		};

		template <class U> struct PointerTraits<U*>
		{
			enum { result = true };
			typedef U PointeeType;
		};

		template <class U> struct PointerTraits<U*&>
		{
			enum { result = true };
			typedef U PointeeType;
		};

		template <class U> struct UnVolatile
		{
			typedef U Result;
			enum { isVolatile = 0 };
		};

		template <class U> struct UnVolatile<volatile U>
		{
			typedef U Result;
			enum { isVolatile = 1 };
		};

		template < typename T > struct VolatileType
		{
			typedef typename volatile UnVolatile<T>::Result Result;
		};

		template <typename T> struct UnvolatileType
		{
			typedef typename UnVolatile<T>::Result Result;
		};

		template<int> struct CompileTimeError;
		template<> struct CompileTimeError<true> {};

	}

	class CStackComEnv
	{
	public:
		explicit CStackComEnv( DWORD dwCoInit = COINIT_APARTMENTTHREADED  )
			:m_bOle( false )
		{
			CoInitializeEx( NULL , dwCoInit );
		}

		explicit CStackComEnv( bool bOle  )
			:m_bOle( bOle )
		{
			if( !m_bOle )
			{		
				CoInitializeEx( NULL , COINIT_APARTMENTTHREADED );
			}
			else
			{
				OleInitialize( NULL );
			}
		}

		~CStackComEnv()
		{
			if( !m_bOle )
			{		
				CoUninitialize();
			}
			else
			{
				OleUninitialize();
			}

		}
	private:
		bool m_bOle;
	};

}




#define PCUTIL_STATIC_CHECK(expr, msg) \
{ pcutil::detail::CompileTimeError<((expr) != 0)> ASSERT_##msg; (void)ASSERT_##msg; } 


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	TypeName& operator=(const TypeName&)


#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
	TypeName();                                    \
	DISALLOW_COPY_AND_ASSIGN(TypeName)


template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define ARRAY_SIZE_ARRAY(array) (sizeof(ArraySizeHelper(array)))
