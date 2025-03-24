//#include "StdAfx.h"
#include "HttpPostArgument.h"

namespace pcutil
{

CHttpPostArgument::CHttpPostArgument()
{
}
CHttpPostArgument::~CHttpPostArgument()
{
}

CHttpPostArgument::CHttpPostArgument( const std::string &strName , const std::string &strValue , DWORD dwArgumentType )
        : m_strName( strName ),
        m_strValue( strValue ),
        m_dwArgumentType( dwArgumentType )
{

}

CHttpPostArgument::CHttpPostArgument( const CHttpPostArgument &other )
{
    *this = other;
}

CHttpPostArgument& CHttpPostArgument::operator=( const CHttpPostArgument & other )
{
    if ( this != &other )
    {
        m_strName = other.m_strName;
        m_strValue = other.m_strValue;
        m_dwArgumentType = other.m_dwArgumentType;
    }
    return *this;
}

bool CHttpPostArgument::operator==( const CHttpPostArgument &other ) const
{
    return ( m_strName == other.m_strName && m_strValue == other.m_strValue && m_dwArgumentType == other.m_dwArgumentType );
}
bool CHttpPostArgument::operator<( const CHttpPostArgument &other ) const
{
    if ( m_strName < other.m_strName )
    {
        return true;
    }
    else if ( m_strValue < other.m_strValue )
    {
        return true;
    }
    else if ( m_dwArgumentType < other.m_dwArgumentType )
    {
        return true;
    }
    return false;
}

std::string CHttpPostArgument::GetName()
{
    return m_strName ;
}
std::string CHttpPostArgument::GetValue()
{
    return m_strValue;
}

DWORD CHttpPostArgument::GetType()
{
    return m_dwArgumentType ;
}

void CHttpPostArgument::SetName( const std::string &strName )
{
    m_strName = strName;
}
void CHttpPostArgument::SetValue( const std::string &strValue )
{
    m_strValue = strValue;
}

void CHttpPostArgument::SetType( DWORD dwArgumentType )
{
    m_dwArgumentType = dwArgumentType;
}



CHttpPostBinaryRefArgument::CHttpPostBinaryRefArgument( LPCTSTR argName, const CString fileName, LPCSTR pBuffer, DWORD length, LPCTSTR contentType /*= _T( "application/octet-stream" ) */ ) : ArgName( argName ), FileName( fileName ), PBuffer( pBuffer ), Length( length ), ContentType( contentType )
{
    BinaryCache.reserve( length );
    BinaryCache.append( pBuffer , length );
    PBuffer = BinaryCache.data();
}
}