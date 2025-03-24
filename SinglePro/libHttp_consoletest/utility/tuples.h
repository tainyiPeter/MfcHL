#pragma once
#include "auxtools.h" 

namespace pcutil
{



template< typename T1 >
class tuple1
{
public:
	typedef tuple1<T1> thisType;
	typedef typename detail::ValueType< T1 >::Result TV1;

public:
	tuple1(){}
	~tuple1(){}

public:
	tuple1( const TV1 &p1 )
		:t1( p1 )
	{
	}

	tuple1( const thisType &other )
	{
		*this = other;
	}
	
	thisType& operator=( const thisType &other )
	{
		if( this != &other )
		{
			t1 = other.t1;
		}
		return *this;
	}
public:
	TV1& v1(){ return t1;}

protected:
	TV1 t1;
};

template< typename T1 , typename T2 >
class tuple2
{
public:
	typedef tuple2< T1 , T2 > thisType;
	typedef typename detail::ValueType< T1 >::Result TV1;
	typedef typename detail::ValueType< T2 >::Result TV2;
public:
	tuple2(){}
	~tuple2(){}
public:
	tuple2( const TV1 &p1 , const TV2 &p2 )
		:t1( p1 ),
		 t2( p2 )
	{
	}

	tuple2( const thisType &other )
	{
		*this = other;
	}
	thisType& operator=( const thisType &other )
	{
		if( this != &other )
		{
			t1 = other.t1;
			t2 = other.t2;
		}
		return *this;
	}
public:
	TV1& v1(){ return t1;}
	TV2& v2(){ return t2;}
	const TV1& v1() const { return t1; }
	const TV2& v2() const { return t2; }
protected:
	TV1 t1;
	TV2 t2;
};

template< typename T1 , typename T2 , typename T3 >
class tuple3
{
public:
	typedef tuple3< T1 , T2 , T3 > thisType;
	typedef typename detail::ValueType< T1 >::Result TV1;
	typedef typename detail::ValueType< T2 >::Result TV2;
	typedef typename detail::ValueType< T3 >::Result TV3;
public:
	tuple3(){}
	~tuple3(){}
public:
	tuple3( const TV1 &p1 ,
			const TV2 &p2 ,
			const TV3 &p3 )
		:t1( p1 ),
		 t2( p2 ),
		 t3( p3 )
	{
	
	}
	tuple3( const thisType &other )
	{
		*this = other;
	}
	thisType& operator=( const thisType &other )
	{
		if( this != &other )
		{
			t1 = other.t1;
			t2 = other.t2;
			t3 = other.t3;
		}
		return *this;
	}
public:
	TV1& v1(){ return t1;}
	TV2& v2(){ return t2;}
	TV3& v3(){ return t3;}
    const TV1& v1() const { return t1; }
    const TV2& v2() const { return t2; }
    const TV3& v3() const { return t3; }
protected:
	TV1 t1;
	TV2 t2;
	TV3 t3;
};


template< typename T1 , 
		  typename T2 ,
		  typename T3 ,
		  typename T4 >
class tuple4
{
public:
	typedef tuple4< T1 , T2 , T3 , T4 > thisType;
	typedef typename detail::ValueType< T1 >::Result TV1;
	typedef typename detail::ValueType< T2 >::Result TV2;
	typedef typename detail::ValueType< T3 >::Result TV3;
	typedef typename detail::ValueType< T4 >::Result TV4;

public:
	tuple4(){}
	~tuple4(){}
public:
	tuple4( const TV1 &p1 ,
			const TV2 &p2 ,
			const TV3 &p3 ,
			const TV4 &p4 )
		:t1( p1 ),
		 t2( p2 ),
		 t3( p3 ),
		 t4( p4 )
	{
	
	}
	tuple4( const thisType &other )
	{
		*this = other;
	}
	thisType& operator=( const thisType &other )
	{
		if( this != &other )
		{
			t1 = other.t1;
			t2 = other.t2;
			t3 = other.t3;
			t4 = other.t4;
		}
		return *this;
	}
public:
	TV1& v1(){ return t1;}
	TV2& v2(){ return t2;}
	TV3& v3(){ return t3;}
	TV4& v4(){ return t4;}
	const TV1& v1() const { return t1; }
	const TV2& v2() const { return t2; }
	const TV3& v3() const { return t3; }
	const TV4& v4() const { return t4; }
protected:
	TV1 t1;
	TV2 t2;
	TV3 t3;
	TV4 t4;
};


template< typename T1 , 
		  typename T2 ,
		  typename T3 ,
		  typename T4 ,
		  typename T5>
class tuple5
{
public:
	typedef tuple5< T1 , T2 , T3 , T4 , T5 > thisType;
	typedef typename detail::ValueType< T1 >::Result TV1;
	typedef typename detail::ValueType< T2 >::Result TV2;
	typedef typename detail::ValueType< T3 >::Result TV3;
	typedef typename detail::ValueType< T4 >::Result TV4;
	typedef typename detail::ValueType< T5 >::Result TV5;

public:
	tuple5(){}
	~tuple5(){}
public:
	tuple5( const TV1 &p1 ,
			const TV2 &p2 ,
			const TV3 &p3 ,
			const TV4 &p4 ,
			const TV5 &p5)
		:t1( p1 ),
		 t2( p2 ),
		 t3( p3 ),
		 t4( p4 ),
		 t5( p5 )
	{
	
	}
	tuple5( const thisType &other )
	{
		*this = other;
	}
	thisType& operator=( const thisType &other )
	{
		if( this != &other )
		{
			t1 = other.t1;
			t2 = other.t2;
			t3 = other.t3;
			t4 = other.t4;
			t5 = other.t5;
		}
		return *this;
	}
public:
	TV1& v1(){ return t1;}
	TV2& v2(){ return t2;}
	TV3& v3(){ return t3;}
	TV4& v4(){ return t4;}
	TV5& v5(){ return t5;}
protected:
	TV1 t1;
	TV2 t2;
	TV3 t3;
	TV4 t4;
	TV5 t5;
};

template< typename T1 , 
		  typename T2 ,
		  typename T3 ,
		  typename T4 ,
		  typename T5 ,
		  typename T6>
class tuple6
{
public:
	typedef tuple6< T1 , T2 , T3 , T4 , T5 , T6 > thisType;
	typedef typename detail::ValueType< T1 >::Result TV1;
	typedef typename detail::ValueType< T2 >::Result TV2;
	typedef typename detail::ValueType< T3 >::Result TV3;
	typedef typename detail::ValueType< T4 >::Result TV4;
	typedef typename detail::ValueType< T5 >::Result TV5;
	typedef typename detail::ValueType< T6 >::Result TV6;
public:
	tuple6(){}
	~tuple6(){}
public:
	tuple6( const TV1 &p1 ,
			const TV2 &p2 ,
			const TV3 &p3 ,
			const TV4 &p4 ,
			const TV5 &p5 ,
			const TV6 &p6 
			)
		:t1( p1 ),
		 t2( p2 ),
		 t3( p3 ),
		 t4( p4 ),
		 t5( p5 ),
		 t6( p6 )
	{
	
	}
	tuple6( const thisType &other )
	{
		*this = other;
	}
	thisType& operator=( const thisType &other )
	{
		if( this != &other )
		{
			t1 = other.t1;
			t2 = other.t2;
			t3 = other.t3;
			t4 = other.t4;
			t5 = other.t5;
			t6 = other.t6;
		}
		return *this;
	}
public:
	TV1& v1(){ return t1;}
	TV2& v2(){ return t2;}
	TV3& v3(){ return t3;}
	TV4& v4(){ return t4;}
	TV5& v5(){ return t5;}
	TV6& v6(){ return t6;}
protected:
	TV1 t1;
	TV2 t2;
	TV3 t3;
	TV4 t4;
	TV5 t5;
	TV6 t6;
};

template< typename T1 , 
		  typename T2 ,
		  typename T3 ,
		  typename T4 ,
		  typename T5 ,
		  typename T6 ,
		  typename T7>
class tuple7
{
public:
	typedef tuple7< T1 , T2 , T3 , T4 , T5 , T6 , T7 > thisType;
	typedef typename detail::ValueType< T1 >::Result TV1;
	typedef typename detail::ValueType< T2 >::Result TV2;
	typedef typename detail::ValueType< T3 >::Result TV3;
	typedef typename detail::ValueType< T4 >::Result TV4;
	typedef typename detail::ValueType< T5 >::Result TV5;
	typedef typename detail::ValueType< T6 >::Result TV6;
	typedef typename detail::ValueType< T7 >::Result TV7;
public:
	tuple7(){}
	~tuple7(){}
public:
	tuple7( const TV1 &p1 ,
			const TV2 &p2 ,
			const TV3 &p3 ,
			const TV4 &p4 ,
			const TV5 &p5 ,
			const TV6 &p6 ,
			const TV7 &p7 
			)
		:t1( p1 ),
		 t2( p2 ),
		 t3( p3 ),
		 t4( p4 ),
		 t5( p5 ),
		 t6( p6 ),
		 t7( p7 )
	{
	
	}
	tuple7( const thisType &other )
	{
		*this = other;
	}
	thisType& operator=( const thisType &other )
	{
		if( this != &other )
		{
			t1 = other.t1;
			t2 = other.t2;
			t3 = other.t3;
			t4 = other.t4;
			t5 = other.t5;
			t6 = other.t6;
			t7 = other.t7;
		}
		return *this;
	}
public:
	TV1& v1(){ return t1;}
	TV2& v2(){ return t2;}
	TV3& v3(){ return t3;}
	TV4& v4(){ return t4;}
	TV5& v5(){ return t5;}
	TV6& v6(){ return t6;}
	TV7& v7(){ return t7;}
protected:
	TV1 t1;
	TV2 t2;
	TV3 t3;
	TV4 t4;
	TV5 t5;
	TV6 t6;
	TV7 t7;
};


template< typename T1 , 
		  typename T2 ,
		  typename T3 ,
		  typename T4 ,
		  typename T5 ,
		  typename T6 ,
		  typename T7 ,
		  typename T8>
class tuple8
{
public:
	typedef tuple8< T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > thisType;
	typedef typename detail::ValueType< T1 >::Result TV1;
	typedef typename detail::ValueType< T2 >::Result TV2;
	typedef typename detail::ValueType< T3 >::Result TV3;
	typedef typename detail::ValueType< T4 >::Result TV4;
	typedef typename detail::ValueType< T5 >::Result TV5;
	typedef typename detail::ValueType< T6 >::Result TV6;
	typedef typename detail::ValueType< T7 >::Result TV7;
	typedef typename detail::ValueType< T8 >::Result TV8;
public:
	tuple8(){}
	~tuple8(){}
public:
	tuple8( const TV1 &p1 ,
			const TV2 &p2 ,
			const TV3 &p3 ,
			const TV4 &p4 ,
			const TV5 &p5 ,
			const TV6 &p6 ,
			const TV7 &p7 ,
			const TV8 &p8 
			)
		:t1( p1 ),
		 t2( p2 ),
		 t3( p3 ),
		 t4( p4 ),
		 t5( p5 ),
		 t6( p6 ),
		 t7( p7 ),
		 t8( p8 )
	{
	
	}
	tuple8( const thisType &other )
	{
		*this = other;
	}
	thisType& operator=( const thisType &other )
	{
		if( this != &other )
		{
			t1 = other.t1;
			t2 = other.t2;
			t3 = other.t3;
			t4 = other.t4;
			t5 = other.t5;
			t6 = other.t6;
			t7 = other.t7;
			t8 = other.t8;
		}
		return *this;
	}
public:
	TV1& v1(){ return t1;}
	TV2& v2(){ return t2;}
	TV3& v3(){ return t3;}
	TV4& v4(){ return t4;}
	TV5& v5(){ return t5;}
	TV6& v6(){ return t6;}
	TV7& v7(){ return t7;}
	TV8& v8(){ return t8;}
protected:
	TV1 t1;
	TV2 t2;
	TV3 t3;
	TV4 t4;
	TV5 t5;
	TV6 t6;
	TV7 t7;
	TV8 t8;
};


template< typename T1 , 
		  typename T2 ,
		  typename T3 ,
		  typename T4 ,
		  typename T5 ,
		  typename T6 ,
		  typename T7 ,
		  typename T8 ,
		  typename T9 >
class tuple9
{
public:
	typedef tuple9< T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > thisType;
	typedef typename detail::ValueType< T1 >::Result TV1;
	typedef typename detail::ValueType< T2 >::Result TV2;
	typedef typename detail::ValueType< T3 >::Result TV3;
	typedef typename detail::ValueType< T4 >::Result TV4;
	typedef typename detail::ValueType< T5 >::Result TV5;
	typedef typename detail::ValueType< T6 >::Result TV6;
	typedef typename detail::ValueType< T7 >::Result TV7;
	typedef typename detail::ValueType< T8 >::Result TV8;
	typedef typename detail::ValueType< T9 >::Result TV9;
public:
	tuple9(){}
	~tuple9(){}
public:
	tuple9( const TV1 &p1 ,
			const TV2 &p2 ,
			const TV3 &p3 ,
			const TV4 &p4 ,
			const TV5 &p5 ,
			const TV6 &p6 ,
			const TV7 &p7 ,
			const TV8 &p8 ,
			const TV9 &p9 
			)
		:t1( p1 ),
		 t2( p2 ),
		 t3( p3 ),
		 t4( p4 ),
		 t5( p5 ),
		 t6( p6 ),
		 t7( p7 ),
		 t8( p8 ),
		 t9( p9 )
	{
	
	}
	tuple9( const thisType &other )
	{
		*this = other;
	}
	thisType& operator=( const thisType &other )
	{
		if( this != &other )
		{
			t1 = other.t1;
			t2 = other.t2;
			t3 = other.t3;
			t4 = other.t4;
			t5 = other.t5;
			t6 = other.t6;
			t7 = other.t7;
			t8 = other.t8;
			t9 = other.t9;
		}
		return *this;
	}
public:
	TV1& v1(){ return t1;}
	TV2& v2(){ return t2;}
	TV3& v3(){ return t3;}
	TV4& v4(){ return t4;}
	TV5& v5(){ return t5;}
	TV6& v6(){ return t6;}
	TV7& v7(){ return t7;}
	TV8& v8(){ return t8;}
	TV9& v9(){ return t9;}
protected:
	TV1 t1;
	TV2 t2;
	TV3 t3;
	TV4 t4;
	TV5 t5;
	TV6 t6;
	TV7 t7;
	TV8 t8;
	TV8 t9;

};














#ifdef _DEBUG
void testTuple();
#endif

}