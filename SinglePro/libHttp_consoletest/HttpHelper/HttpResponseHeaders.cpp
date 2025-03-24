//#include "StdAfx.h"
#include <atlbase.h>
#include <WinInet.h>
#include "HttpResponseHeaders.h"
#include "CStringHelper.h"
#include "AutoBuffer.h"
//#include "CommonHelper.h"

namespace pcutil
{

namespace httpheader_detail
{

static const int kMaxPairs = 16;
static const char kTerminator[]      = "\n\r\0";
static const int  kTerminatorLen     = sizeof( kTerminator ) - 1;
static const char kWhitespace[]      = " \t";
static const char kQuoteTerminator[] = "\"";
static const char kValueSeparator[]  = ";";
static const char kTokenSeparator[]  = ";=";

// Returns true if |c| occurs in |chars|
// TODO maybe make this take an iterator, could check for end also?
static inline bool CharIsA( const char c, const char* chars )
{
    return strchr( chars, c ) != NULL;
}
// Seek the iterator to the first occurrence of a character in |chars|.
// Returns true if it hit the end, false otherwise.
static inline bool SeekTo( std::string::const_iterator* it,
                           const std::string::const_iterator& end,
                           const char* chars )
{
    for ( ; *it != end && !CharIsA( **it, chars ); ++( *it ) );
    return *it == end;
}
// Seek the iterator to the first occurrence of a character not in |chars|.
// Returns true if it hit the end, false otherwise.
static inline bool SeekPast( std::string::const_iterator* it,
                             const std::string::const_iterator& end,
                             const char* chars )
{
    for ( ; *it != end && CharIsA( **it, chars ); ++( *it ) );
    return *it == end;
}
static inline bool SeekBackPast( std::string::const_iterator* it,
                                 const std::string::const_iterator& end,
                                 const char* chars )
{
    for ( ; *it != end && CharIsA( **it, chars ); --( *it ) );
    return *it == end;
}


template <class Char> inline Char ToLowerASCII( Char c ) 
{
  return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

template <class str> inline void StringToLowerASCII(str* s) {
  for (typename str::iterator i = s->begin(); i != s->end(); ++i)
    *i = ToLowerASCII(*i);
}

template <class str> inline str StringToLowerASCII(const str& s) {
  // for std::string and std::wstring
  str output(s);
  StringToLowerASCII(&output);
  return output;
}

}


using namespace httpheader_detail;
CHttpResponseHeaders::CHttpResponseHeaders()
{
	m_statusCode = 400;
	m_location = _T("");
    m_uFileSize = 0;
}

CHttpResponseHeaders::~CHttpResponseHeaders()
{

}

int CHttpResponseHeaders::GetStatusCode()
{
    return m_statusCode;
}

CUsableValue< THttpResponseCookie > CHttpResponseHeaders::GetCookie( const CString &name  )
{
	CUsableValue< THttpResponseCookie > ret;
	CString nametemp = name;
	nametemp.MakeLower();
	for( unsigned int i = 0 ; i < m_vCookies.size() ; i++ )
	{
		if( m_vCookies[i].name == nametemp )
		{
			ret = m_vCookies[i];
			break;
		}
	}
	return ret;
}

CUsableValue<unsigned __int64> CHttpResponseHeaders::GetContentLength()
{
	return m_contentLength;
}

CUsableValue<unsigned __int64> CHttpResponseHeaders::GetFileSize()
{
    return (m_uFileSize.GetValue() == 0 ? m_contentLength : m_uFileSize);
}

CUsableValue< CString > CHttpResponseHeaders::GetRawHeaders()
{
	return m_rawHeaders;

}
CString CHttpResponseHeaders::GetLocation()
{
	return m_location;

}

CUsableValue< std::pair< int , CString > > CHttpResponseHeaders::GetRefreshInfo()
{
	return m_pairRefresh;
}

CString CHttpResponseHeaders::GetContentType()
{
	return m_strContentType;
}

CString CHttpResponseHeaders::GetContentDisposition()
{
	return m_contentDisposition;
}

BOOL CHttpResponseHeaders::ParseCookie( std::string &cookie_line , THttpResponseCookie &ret )
{
    typedef std::pair< std::string , std::string >  TokenValuePair;
    std::vector< TokenValuePair > tpairs;

    // Ok, here we go.  We should be expecting to be starting somewhere
    // before the cookie line, not including any header name...
    std::string::const_iterator start = cookie_line.begin();
    std::string::const_iterator end = cookie_line.end();
    std::string::const_iterator it = start;

    // TODO Make sure we're stripping \r\n in the network code.  Then we
    // can log any unexpected terminators.
    std::string::size_type term_pos = cookie_line.find_first_of(
                                          std::string( kTerminator, kTerminatorLen ) );
    if ( term_pos != std::string::npos )
    {
        // We found a character we should treat as an end of string.
        end = start + term_pos;
    }

    for ( int pair_num = 0; pair_num < kMaxPairs && it != end; ++pair_num )
    {
        TokenValuePair pair;
        std::string::const_iterator token_start, token_real_end, token_end;

        // Seek past any whitespace before the "token" (the name).
        // token_start should point at the first character in the token
        if ( SeekPast( &it, end, kWhitespace ) )
            break;  // No token, whitespace or empty.
        token_start = it;

        // Seek over the token, to the token separator.
        // token_real_end should point at the token separator, i.e. '='.
        // If it == end after the seek, we probably have a token-value.
        SeekTo( &it, end, kTokenSeparator );
        token_real_end = it;

        // Ignore any whitespace between the token and the token separator.
        // token_end should point after the last interesting token character,
        // pointing at either whitespace, or at '=' (and equal to token_real_end).
        if ( it != token_start )  // We could have an empty token name.
        {
            --it;  // Go back before the token separator.
            // Skip over any whitespace to the first non-whitespace character.
            SeekBackPast( &it, token_start, kWhitespace );
            // Point after it.
            ++it;
        }
        token_end = it;

        // Seek us back to the end of the token.
        it = token_real_end;

        if ( it == end || *it != '=' )
        {
            // We have a token-value, we didn't have any token name.
            if ( pair_num == 0 )
            {
                // For the first time around, we want to treat single values
                // as a value with an empty name. (Mozilla bug 169091).
                // IE seems to also have this behavior, ex "AAA", and "AAA=10" will
                // set 2 different cookies, and setting "BBB" will then replace "AAA".
                pair.first = "";
                // Rewind to the beginning of what we thought was the token name,
                // and let it get parsed as a value.
                it = token_start;
            }
            else
            {
                // Any not-first attribute we want to treat a value as a
                // name with an empty value...  This is so something like
                // "secure;" will get parsed as a Token name, and not a value.
                pair.first = std::string( token_start, token_end );
            }
        }
        else
        {
            // We have a TOKEN=VALUE.
            pair.first = std::string( token_start, token_end );
            ++it;  // Skip past the '='.
        }

        // OK, now try to parse a value.
        std::string::const_iterator value_start, value_end;

        // Seek past any whitespace that might in-between the token and value.
        SeekPast( &it, end, kWhitespace );
        // value_start should point at the first character of the value.
        value_start = it;

        // The value is double quoted, process <quoted-string>.
        if ( it != end && *it == '"' )
        {
            // Skip over the first double quote, and parse until
            // a terminating double quote or the end.
            for ( ++it; it != end && !CharIsA( *it, kQuoteTerminator ); ++it )
            {
                // Allow an escaped \" in a double quoted string.
                if ( *it == '\\' )
                {
                    ++it;
                    if ( it == end )
                        break;
                }
            }

            SeekTo( &it, end, kValueSeparator );
            // We could seek to the end, that's ok.
            value_end = it;
        }
        else
        {
            // The value is non-quoted, process <token-value>.
            // Just look for ';' to terminate ('=' allowed).
            // We can hit the end, maybe they didn't terminate.
            SeekTo( &it, end, kValueSeparator );

            // Ignore any whitespace between the value and the value separator
            if ( it != value_start )  // Could have an empty value
            {
                --it;
                SeekBackPast( &it, value_start, kWhitespace );
                ++it;
            }

            value_end = it;
        }

        // OK, we're finished with a Token/Value.
        pair.second = std::string( value_start, value_end );
        // From RFC2109: "Attributes (names) (attr) are case-insensitive."
        if ( pair_num != 0 )
            StringToLowerASCII( &pair.first );
        tpairs.push_back( pair );

        // We've processed a token/value pair, we're either at the end of
        // the string or a ValueSeparator like ';', which we want to skip.
        if ( it != end )
            ++it;
    }
	if( tpairs.size() <= 0 ) return FALSE;

	// We skip over the first token/value, the user supplied one.
	ret.name = CA2T( tpairs[0].first.c_str() ).m_psz;
	ret.value = CA2T( tpairs[0].second.c_str() ).m_psz;
	for ( unsigned int i = 1; i < tpairs.size(); ++i )
	{
		if ( tpairs[i].first == std::string( "path" ) )
		{
			ret.path = CA2T( tpairs[i].second.c_str() ).m_psz;
		}
		else if ( tpairs[i].first == std::string( "domain" ) )
		{
			ret.domain = CA2T( tpairs[i].second.c_str() ).m_psz;
		}
		else if ( tpairs[i].first == std::string( "expires" ) )
		{
			ret.expires = CA2T( tpairs[i].second.c_str() ).m_psz;
		}
		else if ( tpairs[i].first == std::string( "max-age" ) )
		{
			ret.maxage = CA2T( tpairs[i].second.c_str() ).m_psz;
		}
		else if ( tpairs[i].first == std::string( "secure" ) )
		{
			ret.secure = CA2T( tpairs[i].second.c_str() ).m_psz;
		}
		else if ( tpairs[i].first == std::string("httponly") )
		{
			ret.httponly = CA2T( tpairs[i].second.c_str() ).m_psz;
		}
		else
		{ 
			/* some attribute we don't know or don't care about. */ 
		}
	}
	return TRUE;
}


void CHttpResponseHeaders::Parse( HINTERNET hRequest , const CString &rawheader )
{

	m_rawHeaders = rawheader;

    DWORD len = 4;
    DWORD index = 0;
	int nValue = 0;
    BOOL bQuery = HttpQueryInfo( hRequest , HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                                 &nValue , &len , &index );
    index = 0;
	if( bQuery )
	{
		m_statusCode = nValue;
	}
	
    int bufsize = 1024 * 16;
	len = bufsize - 1;
    CAutoBuffer buf;
    buf.AllocateBuffer( bufsize );
    bQuery = HttpQueryInfo( hRequest , HTTP_QUERY_CONTENT_LENGTH ,
							buf.GetBuffer() , &len , &index );
    index = 0;
	if( bQuery )
	{
		m_contentLength = _ttoi64( (TCHAR*)buf.GetBuffer() );
	}

    // Begin: Get Content-Range, files size.
    index = 0;
    len = bufsize - 1;
    ZeroMemory( buf.GetBuffer() , buf.GetBufferSize() )	;
    bQuery = HttpQueryInfo( hRequest , HTTP_QUERY_CONTENT_RANGE ,
							buf.GetBuffer() , &len , &index );
	if( bQuery )
	{
        CString str = (TCHAR*)buf.GetBuffer();
        int nIdx = str.ReverseFind(_T('/'));
        str = str.Mid(nIdx + 1);
        m_uFileSize = _ttoi64( (TCHAR*)str.GetBuffer() );
	}
    index = 0;
    // End 

    do
    {
        len = bufsize - 1;
	    ZeroMemory( buf.GetBuffer() , buf.GetBufferSize() )	;
		bQuery = HttpQueryInfo( hRequest ,  HTTP_QUERY_SET_COOKIE,
                                buf.GetBuffer() , &len , &index );
        if ( !bQuery )
		{
			break;
		}
		std::string cookie = CT2A( (TCHAR*)buf.GetBuffer() ).m_psz;
        THttpResponseCookie ret ;
        if ( ParseCookie( cookie , ret ) )
        {
            m_vCookies.push_back( ret );
        }
        if ( index == ERROR_HTTP_HEADER_NOT_FOUND )
        {
            break;
        }
    }while ( TRUE );
    index = 0;
	len = bufsize - 1;
	ZeroMemory( buf.GetBuffer() , buf.GetBufferSize() )	;
	bQuery = HttpQueryInfo( hRequest ,  HTTP_QUERY_LOCATION,
		buf.GetBuffer() , &len , &index );
	if ( bQuery )
	{
		m_location = (LPCTSTR)buf.GetBuffer();
	}

	bQuery = HttpQueryInfo( hRequest ,  HTTP_QUERY_CONTENT_ENCODING,
		buf.GetBuffer() , &len , &index );
	if ( bQuery )
	{
		m_contentEncoding = (LPCTSTR)buf.GetBuffer();
	}

    index = 0;
	len = bufsize - 1;
	ZeroMemory( buf.GetBuffer() , buf.GetBufferSize() )	;
	bQuery = HttpQueryInfo( hRequest ,  HTTP_QUERY_REFRESH,
		buf.GetBuffer() , &len , &index );
	if ( bQuery )
	{
		CString strRefreshContent = (LPCTSTR)buf.GetBuffer();
		strRefreshContent.Trim();
		if( !strRefreshContent.IsEmpty() )
		{
			std::vector< CString > vRefreshItems;
			/*CommonHelper*/CStringHelper::SplitCStringToArray( strRefreshContent , _T(";") , vRefreshItems );
			if( vRefreshItems.size() > 0 )
			{
				m_pairRefresh.SetUsable( TRUE );
				m_pairRefresh.GetValue().first = _ttoi( vRefreshItems[0].Trim() );
				if( vRefreshItems.size() > 1 ) m_pairRefresh.GetValue().second = vRefreshItems[1];
			}
		}
	}

    index = 0;
	len = bufsize - 1;
	ZeroMemory( buf.GetBuffer() , buf.GetBufferSize() )	;
	bQuery = HttpQueryInfo( hRequest ,  HTTP_QUERY_CONTENT_TYPE,
		buf.GetBuffer() , &len , &index );
	if ( bQuery )
	{
		m_strContentType = (LPCTSTR)buf.GetBuffer();
		m_strContentType.Trim();
		m_strContentType.MakeLower();
	}	

	index = 0;
	len = bufsize - 1;
	ZeroMemory(buf.GetBuffer(), buf.GetBufferSize());
	bQuery = HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_DISPOSITION,
		buf.GetBuffer(), &len, &index);
	if (bQuery)
	{
		m_contentDisposition = (LPCTSTR)buf.GetBuffer();
		m_contentDisposition.Trim();
	}

}

CString CHttpResponseHeaders::GetContentEncoding()
{
	return m_contentEncoding;
}








}
