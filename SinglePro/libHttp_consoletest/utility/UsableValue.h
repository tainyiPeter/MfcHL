#pragma once

namespace pcutil
{

template< typename T >
class CUsableValue
{
public:
	CUsableValue()
		:m_bUsable( FALSE )
	{
	}
	~CUsableValue(){}
public:
	CUsableValue( const CUsableValue &other )
	{
		*this = other;
	}
	CUsableValue& operator=( const CUsableValue &other )
	{
		if( this != &other )
		{
			m_bUsable = other.m_bUsable;
			m_value = other.m_value;
		}
		return *this;
	}
	CUsableValue& operator=( const T &value )
	{
		m_value = value;
		m_bUsable = TRUE;
		return *this;
	}
	bool operator==( const CUsableValue &other ) const
	{
		return ( m_bUsable == other._bUseable && m_value == other.m_value );
	}
	bool operator<( const CUsableValue &other ) const
	{
		if( m_bUsable < other._bUseable )
			return true;
		if( m_value < other.m_value )
			return true;
		return false;
	}
	
	BOOL operator!() const
	{
		return !m_bUsable;
	}

	operator BOOL()
	{
		return m_bUsable;
	}

	const T& GetValue() const
	{
		return m_value;
	}

	T& GetValue()
	{
		return m_value;
	}

	BOOL IsUsable() const
	{
		return m_bUsable;
	}

	void SetUsable( BOOL bUsable )
	{
		m_bUsable = bUsable;
	}

private:
	T m_value;
	BOOL m_bUsable;

};
#ifdef _DEBUG
int testUsableValue();
#endif

}