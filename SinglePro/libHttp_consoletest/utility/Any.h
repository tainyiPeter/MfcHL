#pragma once


#include "auxtools.h" 
#include <typeinfo>

namespace pcutil
{

class CAny
{
public: 
    CAny()
		:content( 0 )
    {
    }
    template<typename valueType>
    CAny( const valueType& value )
            : content( new holder<valueType>( value ) )
    {
    }
    CAny( const CAny & other )
            : content( other.content ? other.content->clone() : 0 )
    {
    }
    ~CAny()
    {
        delete content;
    }
public: 
    CAny& swap( CAny & rhs )
    {
        std::swap( content, rhs.content );
        return *this;
    }
    template<typename valueType>
    CAny& operator=( const valueType & rhs )
    {
        CAny( rhs ).swap( *this );
        return *this;
    }
    CAny& operator=( const CAny & rhs )
    {
        CAny( rhs ).swap( *this );
        return *this;
    }

	bool operator==( const CAny & rhs ) const
	{
		if( content == NULL && rhs.content == NULL )
			return true;
		if( content == NULL || rhs.content == NULL )
			return false;
		if( content->type().name() != rhs.content->type().name() )
			return false;
		return  content  == rhs.content ;//shit й╫еп╤о
	}

	bool operator<( const CAny & rhs ) const
	{
		if( content == NULL && rhs.content == NULL )
			return false;
		if( content == NULL || rhs.content != NULL )
			return true;
		if( content != NULL || rhs.content == NULL )
			return false;
		if( content->type().name() < rhs.content->type().name() )
			return true;
		if( content->type().name() > rhs.content->type().name() )
			return false;
		return content  < rhs.content ; //shit й╫еп╤о
	}



public: 
    bool empty() const
    {
        return !content;
    }
    const std::type_info & type() const
    {
        return content ? content->type() : typeid( void );
    }
public: 
    class placeholder
    {
    public: 
        virtual ~placeholder()
        {
        }
    public: 
        virtual const std::type_info & type() const = 0;
        virtual placeholder * clone() const = 0;
    };

    template<typename valueType>
    class holder : public placeholder
    {
    public: 
        holder( const valueType & value )
                : held( value )
        {
			//PCUTIL_STATIC_CHECK( detail::PointerTraits<valueType>::result == false, CAny_not_Support_Pointer_Type );

        }
    public: 
        virtual const std::type_info & type() const
        {
            return typeid( valueType );
        }
        virtual placeholder * clone() const
        {
            return new holder( held );
        }
    public: 
        //valueType held;
		typedef typename detail::ValueType<valueType>::Result VType; 
		VType held;
    };


public: 
    template<typename valueType>
    friend valueType * any_cast( CAny * );

    template<typename valueType>
    friend valueType * unsafe_any_cast( CAny * );
public:
    placeholder *content;
};

class bad_any_cast : public std::bad_cast
{
public:
    virtual const char * what() const throw()
    {
        return "bad_any_cast: "
               "failed conversion using pcutil::any_cast";
    }
};

template<typename valueType>
valueType* any_cast( CAny * operand )
{
    return operand && operand->type() == typeid( valueType )
           ? &static_cast<CAny::holder<valueType> *>( operand->content )->held
           : 0;
}

template<typename valueType>
inline const valueType* any_cast( const CAny *operand )
{
    return any_cast<valueType>( const_cast<CAny *>( operand ) );
}


template<typename valueType>
valueType any_cast( CAny & operand  )
{
	typedef detail::ValueType<valueType>::Result nonref;
	//PCUTIL_STATIC_CHECK( detail::ReferenceTraits<nonref>::result == false , CAny_not_Support_Pointer_Type );
    nonref* result = any_cast<nonref>( &operand );
    if ( !result )
	{
		throw bad_any_cast() ;
	}
    return *result;
}

template<typename valueType>
inline valueType any_cast( const CAny & operand )
{
	typedef detail::ValueType<valueType>::Result nonref;
	//PCUTIL_STATIC_CHECK( detail::ReferenceTraits<nonref>::result == false , CAny_not_Support_Pointer_Type );
    return any_cast<const nonref &>( const_cast<CAny &>( operand ) );
}










template<typename valueType>
inline valueType  unsafe_cany_cast( CAny * operand )
{
	return (static_cast<CAny::holder<valueType> *>(operand->content))->held;
}




//template<typename valueType>
//inline valueType * unsafe_any_cast( CAny * operand )
//{
//    return &static_cast<CAny::holder<valueType> *>( operand->content )->held;
//}

//template<typename valueType>
//inline const valueType * unsafe_any_cast( const CAny * operand )
//{
//    return unsafe_any_cast<valueType>( const_cast<CAny *>( operand ) );
//}

//#ifdef _DEBUG
//void testCAny();
//#endif


}
