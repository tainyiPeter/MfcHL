//#include "StdAfx.h"
#include <Windows.h>
#include <wininet.h>
#include <sstream>
#include "HttpClientSession.h"
#include "CStringHelper.h"
//#include "CommonHelper.h"
//#include "zip/ZlibStream.h"
//#include "MiscHelper.h"
//#include "ClientLogger.h"
//
DWORD g_default_dwProxyType = INTERNET_OPEN_TYPE_PRECONFIG;
int g_http_timeout_value = 30000;
bool g_http_proxy_retry = true;
bool g_redirect_method_get = false;

namespace pcutil
{
//#ifdef _DEBUG
//volatile long g_nHttpClientSessionCount = 0;
//CComAutoCriticalSection g_csHttpClientSessionCount;
//#endif
CHttpClientSession::CHttpClientSession()
        : m_dwMaxResponseContentSize( 1024 * 100 ),
        m_hHTTPOpen( NULL ),
        m_hHTTPConnect( NULL ),
        m_hHTTPRequest( NULL ),
        m_strAgentName( _T( "PC-WIN7" ) ),
        m_strAcceptType( _T( "Accept: */*\r\n" ) ),
        m_strBoundaryName( _T( "-----xz4BBcV-5C1C9B2B-95AC-4752-AF91-10B280A68D3B---" ) ),
        m_bHttps( FALSE ),
        m_bTwoStepRequest( FALSE ),
		m_onlyReadHeader(FALSE),
		m_errorCode(0),
		m_requestWithDody(TRUE)
{
	m_strContentType = _T( "Content-Type: application/oct-stream\r\n" );
    m_bufResponseContent.reserve( m_dwMaxResponseContentSize );
    m_dwProxyType = g_default_dwProxyType;
	//m_dwProxyType = INTERNET_OPEN_TYPE_DIRECT;
	m_ptrResponseHeaders.reset( new CHttpResponseHeaders() );
	m_bConnectionKeepAlive = FALSE;
	m_bHttpClosed = FALSE;
	m_buffSize = 1024 * 128;

//#ifdef _DEBUG
//	g_csHttpClientSessionCount.Lock();
//	ATLTRACE( _T( "\r\nHttp Request Count = %d increasement" ) , ++g_nHttpClientSessionCount );
//	g_csHttpClientSessionCount.Unlock();
//#endif
}

CHttpClientSession::~CHttpClientSession()
{
    Close();
//#ifdef _DEBUG
//	g_csHttpClientSessionCount.Lock();
//	ATLTRACE( _T( "\r\nHttp Request Count = %d decreasement" ) , --g_nHttpClientSessionCount );
//	g_csHttpClientSessionCount.Unlock();
//#endif
}

void CHttpClientSession::SetProxy( DWORD dwType ,const CString &strProxyNames ,
								  const CString &strProxyUser , const CString &strProxyPwd )
{
    m_dwProxyType = dwType;
	m_strProxyNames = strProxyNames;
	m_strProxyUser = strProxyUser;
	m_strProxyPwd = strProxyPwd;
}

void CHttpClientSession::SetConnectionKeepAlive( BOOL bConnectionKeepAlive )
{
	m_bConnectionKeepAlive = bConnectionKeepAlive;
}

void CHttpClientSession::SetMaxResponseContentSize( DWORD dwMaxResponseContentSize )
{
    ATLASSERT( m_hHTTPOpen == NULL );
    if ( dwMaxResponseContentSize < m_dwMaxResponseContentSize )
        return ;
    m_dwMaxResponseContentSize = dwMaxResponseContentSize;
    m_bufResponseContent.reserve( dwMaxResponseContentSize );
}

DWORD CHttpClientSession::GetMaxResponseContentSize()
{
    return m_dwMaxResponseContentSize;
}

void CHttpClientSession::SetAgentName( const CString &strAgentName )
{
    ATLASSERT( !strAgentName.IsEmpty() );
    m_strAgentName = strAgentName;
}

CString CHttpClientSession::GetAgentName()
{
    return m_strAgentName;
}

void CHttpClientSession::SetReferrerName( const CString &strReferrerName )
{
    m_strReferrerName = strReferrerName;
}

CString CHttpClientSession::GetReferrerName()
{
    return m_strReferrerName;
}

std::string& CHttpClientSession::GetResponseContent()
{
    return m_bufResponseContent;
}

CString CHttpClientSession::GetResponseTextContent()
{
	//return CommonHelper::ConvertUtf8ToLocal(m_bufResponseContent);
	return CStringHelper::ConvertUtf8ToLocal( m_bufResponseContent );
}

CString& CHttpClientSession::GetResponseHeader()
{
    return m_strResponseHeaders;
}

void CHttpClientSession::SetBoundaryName( const CString &strBoundaryName )
{
    m_strBoundaryName = strBoundaryName;
}

CString CHttpClientSession::GetBoundaryName()
{
    return m_strBoundaryName;
}

void CHttpClientSession::SetContentType( const CString &strContentType )
{
	m_strContentType.Format( _T("Content-Type: %s\r\n") , strContentType ); 
}



CHttpResponseHeadersPtr CHttpClientSession::GetResponseHeaderPtr()
{
	return m_ptrResponseHeaders;
}


void CHttpClientSession::SetTwoStepRequest( BOOL bTwoStepRequest )
{
    m_bTwoStepRequest = bTwoStepRequest;
}


BOOL CHttpClientSession::Connect( const CString &strAddress,
                                  unsigned short nPort,
                                  LPCTSTR szUserAccount,
                                  LPCTSTR szPassword
                                )
{
	if( m_bHttpClosed ) return FALSE;
	if( m_hHTTPOpen == NULL )
	{
		if( INTERNET_OPEN_TYPE_PRECONFIG == m_dwProxyType || INTERNET_OPEN_TYPE_DIRECT == m_dwProxyType )
		{
			m_hHTTPOpen =::InternetOpen( m_strAgentName,					// agent name
									 m_dwProxyType,							// proxy option
									 NULL,								// proxy
									 NULL,								// proxy bypass
									 0 );								// flags
		}
		else if( INTERNET_OPEN_TYPE_PROXY == m_dwProxyType )
		{
			m_hHTTPOpen =::InternetOpen( m_strAgentName,					// agent name
									 m_dwProxyType,							// proxy option
									 m_strProxyNames,						// proxy
									 NULL,//_T("<local>"),									// proxy bypass
									 0 );									// flags
		
		}
		if ( m_hHTTPOpen == NULL )
		{
			return FALSE;
		}

		DWORD timeout = g_http_timeout_value;

		BOOL result  = ::InternetSetOption(
			m_hHTTPOpen,
			INTERNET_OPTION_CONNECT_TIMEOUT,
			(LPVOID)&timeout,
			sizeof(timeout)
		);

		::InternetSetOption(
			m_hHTTPOpen,
			INTERNET_OPTION_SEND_TIMEOUT,
			(LPVOID)&timeout,
			sizeof(timeout)
		);
		
		::InternetSetOption(
			m_hHTTPOpen,
			INTERNET_OPTION_RECEIVE_TIMEOUT,
			(LPVOID)&timeout,
			sizeof(timeout)
		);
		

		DWORD dwConnInfoSize = sizeof( INTERNET_CONNECTED_INFO ) ;//解决IE设置成脱机工作的bug
		INTERNET_CONNECTED_INFO InetConnInfo = {0};
		if( InternetQueryOption( m_hHTTPOpen , INTERNET_OPTION_CONNECTED_STATE , (LPBYTE)&InetConnInfo , &dwConnInfoSize ) )
		{
			if( ( InetConnInfo.dwConnectedState & INTERNET_STATE_CONNECTED ) == 0 )
			{
				InetConnInfo.dwConnectedState = INTERNET_STATE_CONNECTED;
				InternetSetOption( m_hHTTPOpen , INTERNET_OPTION_CONNECTED_STATE , (LPBYTE)&InetConnInfo , sizeof( InetConnInfo ) );
			}
		}
	}

    DWORD dwFlag = INTERNET_FLAG_NO_UI;//( INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE );
    if( m_hHTTPConnect == NULL )
	{
		m_hHTTPConnect =::InternetConnect(
							m_hHTTPOpen,			// internet opened handle
							strAddress,				// server name
							nPort,					// ports
							szUserAccount,			// user name
							szPassword,				// password
							INTERNET_SERVICE_HTTP,	// service type
							dwFlag,	// service option
							0 );					// context call-back option
		if ( m_hHTTPConnect == NULL )
		{
			return FALSE;
		}


		if( INTERNET_OPEN_TYPE_PROXY == m_dwProxyType )
		{
			if( !m_strProxyUser.IsEmpty() )
			{
				InternetSetOption( m_hHTTPConnect , INTERNET_OPTION_PROXY_USERNAME ,
					m_strProxyUser.GetBuffer(), m_strProxyUser.GetLength() );
				m_strProxyUser.ReleaseBuffer();
			}

			if( !m_strProxyPwd.IsEmpty() )
			{
				InternetSetOption( m_hHTTPConnect , INTERNET_OPTION_PROXY_PASSWORD  ,
					m_strProxyPwd.GetBuffer(), m_strProxyPwd.GetLength() );
				m_strProxyPwd.ReleaseBuffer();
			}
		}

		if( !m_bHttps )
		{
			m_bConnectionKeepAlive = TRUE;
			//AddOneRequestHeader( ( m_bConnectionKeepAlive  ) ? _T("Connection: Keep-Alive") : _T("Connection: Close") );
		}

		//测试连接可用?
		if ( ::InternetAttemptConnect( NULL ) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		DWORD dwFlags = INTERNET_CONNECTION_OFFLINE ;
		CString strConnectionName;
		BOOL bOK = InternetGetConnectedStateEx( &dwFlags , strConnectionName.GetBuffer( 1024 ), 1023 , 0 );
		strConnectionName.ReleaseBuffer();

		if ( !bOK)
		{
			if ((dwFlags & INTERNET_CONNECTION_OFFLINE) == INTERNET_CONNECTION_OFFLINE)
			{
				//LOG_INFO(_T("(dwFlags & INTERNET_CONNECTION_OFFLINE) == INTERNET_CONNECTION_OFFLINE"));
				//没有网络连接
				return TRUE;//不能返回FALSE,IE设置成脱机状态时造成BUG
			}
			else
			{
				m_errorCode = -1;
				//LOG_INFO(_T("InternetGetConnectedStateEx not connect"));
				return FALSE;
			}
		}
	}

    return TRUE;
}

BOOL CHttpClientSession::Close()
{
	m_bHttpClosed = TRUE;
	SetLastError( 0 );
	ResetRequestHandle();
	RemoveAllRequestHeader();

#ifdef _DEBUG
	if( GetLastError() == WSAEWOULDBLOCK )
	{
		CString strLog;
		strLog.Format( _T("m_strRequestURL = %s | m_emRequestMethod = %u") , m_strRequestURL , m_emRequestMethod );
		//ClientLogger::CommInfo( _T("wininet.http") , _T("") , strLog );
	}
#endif

    return TRUE;
}

void CHttpClientSession::InitilizePostArguments()
{
    m_vArguments.clear();
}

void CHttpClientSession::AddPostArguments( LPCTSTR lpName, DWORD nValue )
{
    ATLASSERT( m_hHTTPOpen == NULL );
	std::string szName = /*CommonHelper*/CStringHelper::ConvertLocalToUtf8( lpName );
    m_vArguments.push_back( CHttpPostArgument( szName , 
											   CT2A( /*CommonHelper*/CStringHelper::UInt32ToString( nValue ) ).m_psz ,
											   CHttpPostArgument::TypeNormal )
						  );
}

void CHttpClientSession::AddPostArguments( LPCTSTR lpName, LPCTSTR lpValue )
{
	CString encodeString = /*CommonHelper*/CStringHelper::URLEncode( lpValue );
	ATLASSERT( m_hHTTPOpen == NULL );
	std::string szName = /*CommonHelper*/CStringHelper::ConvertLocalToUtf8( lpName );
	std::string szValue = /*CommonHelper*/CStringHelper::ConvertLocalToUtf8( encodeString );
	m_vArguments.push_back( CHttpPostArgument( szName , szValue, CHttpPostArgument::TypeNormal ) );
}

void CHttpClientSession::AddPostBinaryRefArgument( LPCTSTR argName, const CString& fileName, LPCSTR pBuffer, DWORD len, LPCTSTR contentType /*= _T( "application/octet-stream" ) */ )
{
	ATLASSERT( m_hHTTPOpen == NULL );
	m_vBinaryRefArguments.push_back( CHttpPostBinaryRefArgument( argName, fileName, pBuffer, len, contentType ) );
}

void CHttpClientSession::AddPostRawText(  const std::string& szValue )
{
    DWORD dwRandom = GetTickCount();
    std::ostringstream os;
    os << dwRandom;
    m_vArguments.push_back( CHttpPostArgument( os.str() , szValue , CHttpPostArgument::TypeRawText ) );
}

void CHttpClientSession::AddPostCStringBody( const CString& strBody )
{
    AddPostRawText( /*CommonHelper*/CStringHelper::ConvertLocalToUtf8( strBody ) );
}

BOOL CHttpClientSession::AddOneRequestHeader( const CString &strHeader, DWORD dwFlag  )
{
    CString strTemp = strHeader;
    strTemp.Trim();
    if ( strTemp.Right( 2 ) != _T( "\r\n" ) )
    {
        strTemp += _T( "\r\n" );
    }
    m_vRequestHeader.push_back( std::pair< CString , DWORD >( strTemp , dwFlag ) );
    return TRUE;
}

void CHttpClientSession::RemoveAllRequestHeader()
{
	m_vRequestHeader.clear();
}

BOOL CHttpClientSession::Request( LPCTSTR szURL1,
                                  CHttpClientSession::RequestMethod Method,
								  BOOL bAutoClose
                                )
{
	CHttpClientSession::RequestMethod currentMethod = Method;
	m_strRequestURL = szURL1;
#ifdef _DEBUG
	m_emRequestMethod = currentMethod;
	ATLTRACE( _T("\r\nHttp Request Start : URL = %s | RequestMethod = %d。 \r\n") , m_strRequestURL , m_emRequestMethod  );
#endif
	int nJumpRetryCount = 1 , nMaxJumpRetry = 5;
	bool requestTryed = false;
    BOOL bReturn = FALSE;
    try
    {
		CString strRequestUrl = szURL1;
        CString strProtocol;
        CString strAddress;
        CString strURI;
        unsigned short nPort = 0;

	JUMPRETRY:
		auto backup = m_vRequestHeader;
		m_vRequestHeader.clear();
		if( m_bHttpClosed ) return FALSE;
		if( nJumpRetryCount > nMaxJumpRetry )
		{
			return FALSE;
		}
        strProtocol = strAddress = strURI = _T("");
        nPort = 0;
        m_bufResponseContent.clear();
        m_strResponseHeaders.Empty();

        this->ParseURL( strRequestUrl, strProtocol, strAddress, nPort, strURI );

        if ( strProtocol.CompareNoCase( _T( "https" ) ) == 0 )
        {
            m_bHttps = TRUE;
        }
        if ( strProtocol.CompareNoCase( _T( "https" ) ) != 0 && strProtocol.CompareNoCase( _T( "http" ) ) != 0  )
        {
            return FALSE;
        }

        CString strHostHeader;
		//if ( nPort == 0 )
		//{
		strHostHeader.Format( _T( "Host: %s\r\n" ) , strAddress );
		//}
		//else
		//{
		//	strHostHeader.Format( _T( "Host: %s:%d\r\n" ) , strAddress, nPort );
		//}
       
        AddOneRequestHeader( strHostHeader , HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD );
		if (backup.size()) {
			for (auto itr = backup.begin(); itr != backup.end(); ++itr) {
				m_vRequestHeader.push_back(*itr);
				//AddOneRequestHeader(itr->first, itr->second);
			}
		}

		CString strTrace;
		if ( Connect( strAddress, nPort ) )
        {
            if ( RequestOfURI( strURI, currentMethod) )
            {
                DWORD dwRealResponseSize = 0 ;
                if ( Response(  dwRealResponseSize ) )
                {
					if( m_ptrResponseHeaders->GetStatusCode() == HTTP_STATUS_MOVED 
						|| m_ptrResponseHeaders->GetStatusCode() == HTTP_STATUS_REDIRECT 
						|| m_ptrResponseHeaders->GetStatusCode() == HTTP_STATUS_REDIRECT_METHOD
						 )
					{
						strRequestUrl = m_ptrResponseHeaders->GetLocation();
						
						ResetRequestHandle();
						nJumpRetryCount++;

						/*if (m_emRequestMethod == RequestGetMethod)
						{*/
						m_requestWithDody = FALSE;
						//}
						if (g_redirect_method_get)
						{
							currentMethod = RequestGetMethod;
						}
						goto JUMPRETRY;
					}
                    bReturn = TRUE;
                }
				else
				{
					m_errorCode = GetLastError();
					//LOG_ERROR(_T("Response false"));
				}
            }
			else
			{
				//在此处做一次重试
				//如果是直连的，则使用代理重试，如果是代理则使用直连重试
				if (false&& g_http_proxy_retry && !requestTryed ) {
					requestTryed = true;

					if (m_dwProxyType == INTERNET_OPEN_TYPE_PRECONFIG) {
						m_dwProxyType = INTERNET_OPEN_TYPE_DIRECT;
					}
					else if (m_dwProxyType == INTERNET_OPEN_TYPE_DIRECT) {
						m_dwProxyType = INTERNET_OPEN_TYPE_PRECONFIG;
					}
					ResetRequestHandle();
					//LOG_ERROR(L"try again :%d", m_dwProxyType);
					goto JUMPRETRY;
				}
				m_errorCode = GetLastError();
				//LOG_ERROR(_T(" Request error "));
			}
        }
		else
		{
			m_errorCode = -4;
			//LOG_ERROR(_T("http conn error"));
		}
		if( bAutoClose )
		{
			Close();
		}
    }
    catch ( ... )
    {
    }
	//下一次请求的代理方式使用上一次成功的方式
	if (bReturn && m_dwProxyType != g_default_dwProxyType)
	{
		g_default_dwProxyType = bReturn;
	}
    return bReturn;

}

CString CHttpClientSession::GetContentType( const CString &strExtName )
{
    CString strReturn = _T( "application/octet-stream" );
    DWORD dwDot = strExtName.ReverseFind( _T( '.' ) );
    if ( dwDot <= 0 )
    {
        return strReturn;
    }
    CString strWord = strExtName.Mid( dwDot );
    HKEY	hKEY;
    if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, strWord, 0, KEY_QUERY_VALUE, &hKEY ) == ERROR_SUCCESS )
    {
        DWORD dwLen = 1024 ;
        CString strRegValue ;
        LONG lRet =  RegQueryValueEx(	hKEY,
                                      _T( "Content Type" ),
                                      NULL,
                                      NULL,
                                      ( LPBYTE )strRegValue.GetBuffer( dwLen ) ,
                                      ( LPDWORD ) & dwLen );
        strRegValue.ReleaseBuffer();
        if ( lRet == ERROR_SUCCESS )
        {
            strReturn = strRegValue;
        }
        RegCloseKey( hKEY );
    }
    return strReturn;
}

void CHttpClientSession::ParseURL( const CString &strURL, CString &strProtocol, CString &strAddress,
                                   unsigned short &nPort, CString &strURI )
{
    CString strRealURL = strURL;
    int pos = strRealURL.Find( _T( ":/" ) );
    if ( pos > 0 )
    {
        strProtocol = strRealURL.Left( pos );
        strRealURL = strRealURL.Mid( pos + 3 );
    }
    else
    {
        strProtocol = _T( "http" );
    }
    strAddress = strRealURL;
    pos = strRealURL.Find( _T( "/" ) );
    if ( pos >= 0 )
    {
        strAddress = strRealURL.Left( pos );
        strURI = strRealURL.Mid( pos  );
    }
    strRealURL.Empty();

    pos = strAddress.Find( _T( ":" ) ) ;
    if ( pos >= 0 )
    {
        nPort = ( unsigned short )_ttol( strAddress.Mid( pos + 1 ) );
        strAddress = strAddress.Left( pos );
    }
    else
    {
		if ( strProtocol.CompareNoCase( _T( "https" ) ) == 0 )
        {
            nPort = INTERNET_DEFAULT_HTTPS_PORT;
        }
        else if ( strProtocol.CompareNoCase( _T( "http" ) )== 0 )
        {
            nPort = INTERNET_DEFAULT_HTTP_PORT;
        }
    }
}

BOOL CHttpClientSession::RequestOfURI( const CString &strURI , CHttpClientSession::RequestMethod Method )
{
    BOOL bReturn = TRUE;
    try
    {
        switch ( Method )
        {
            case CHttpClientSession::RequestPostMethod:
                bReturn = RequestPost( strURI );
                break;
            case CHttpClientSession::RequestPostMethodMultiPartsFormData:
                bReturn = RequestPostMultiParts( strURI );
                break;
            case CHttpClientSession::RequestGetMethod:
            default:
                bReturn = RequestGet( strURI );
                break;
        }
    }
    catch ( ... )
    {
    }
    return bReturn;
}

BOOL CHttpClientSession::RequestGet( const CString& strURI )
{
	CString strTrace;
    LPCTSTR ppszAcceptTypes[2];
    ppszAcceptTypes[0] = _T( "*/*" );  //We support accepting any mime file type since this is a simple download of a file
    ppszAcceptTypes[1] = NULL;

    if( !OpenRequest( FALSE, strURI, ppszAcceptTypes ) )
    {
        return FALSE;
    }

    // REPLACE HEADER
    if ( !::HttpAddRequestHeaders( m_hHTTPRequest, m_strAcceptType, m_strAcceptType.GetLength(), HTTP_ADDREQ_FLAG_REPLACE ) )
    {
		strTrace.Format( _T("In CHttpClientSession::RequestGet: HttpAddRequestHeaders false , m_strAcceptType = %s ") , m_strAcceptType  );
		//pcutil::ClientLogger::BizInfo( _T("CHttpClientSession::Request") , strTrace );
        return FALSE;
    }

    //add user header
    for ( unsigned int i = 0 ; i < m_vRequestHeader.size() ; i++ )
    {
        if ( !::HttpAddRequestHeaders( m_hHTTPRequest,
                                       m_vRequestHeader[i].first,
                                       m_vRequestHeader[i].first.GetLength(),
                                       m_vRequestHeader[i].second ) )
        {
			strTrace.Format( _T("In CHttpClientSession::RequestGet: HttpAddRequestHeaders false , m_vRequestHeader[i].first = %s ") , m_vRequestHeader[i].first  );
			//pcutil::ClientLogger::BizInfo( _T("CHttpClientSession::Request") , strTrace );
            return FALSE;
        }
    }

	CString strContentType = m_strContentType;//_T( "Content-Type: application/oct-stream\r\n" );
	std::string cache;
	if (m_requestWithDody)
	{
		GetPostArgumentsBuffer(cache);
	}
	

	int nRetryCount = 0;
    do
    {
		if( ++nRetryCount > 2 )
		{
			return FALSE;
		}
        SetLastError( 0 );
       if ( !::HttpSendRequest(
					m_hHTTPRequest,				// handle by returned HttpOpenRequest
					strContentType,				// additional HTTP header
					strContentType.GetLength(),	// additional HTTP header length
					( void* )cache.c_str(),					// additional data in HTTP Post or HTTP Put
					cache.size()		// additional data length
				)
		   )
        {
            DWORD dwError = GetLastError();

            //if ( ERROR_INTERNET_INVALID_CA == dwError )
			strTrace.Format( _T("In CHttpClientSession::RequestGet: HttpSendRequest false , dwError = %d ") , dwError  );
			//pcutil::ClientLogger::BizInfo( _T("CHttpClientSession::Request") , strTrace );
			//LOG_ERROR(strTrace);
			if( m_bHttps )
            {
                DWORD dwFlags = 0;
                DWORD dwBuffLen = sizeof( dwFlags );
                InternetQueryOption ( m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                      ( LPVOID )&dwFlags, &dwBuffLen );
                //dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ; 
				//dwFlags |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_WRONG_USAGE ;
                //dwFlags |= SECURITY_FLAG_IGNORE_REVOCATION ;
				InternetSetOption ( m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                    &dwFlags, sizeof ( dwFlags ) );
				strTrace.Format( _T("In CHttpClientSession::RequestGet: HttpSendRequest false , Sleep( %d ) sec!") , nRetryCount  );
				//pcutil::ClientLogger::BizInfo( _T("CHttpClientSession::Request") , strTrace );

				//int nSleepTimes = 1;
				//do
				//{
				//	Sleep(nSleepTimes * 500);
				//	nSleepTimes++;
				//	if (m_bHttpClosed)
				//	{
				//		return FALSE;
				//	}
				//} while (nSleepTimes * 500 <= nRetryCount * 1000);

				Sleep(100);
				if (m_bHttpClosed)
				{
					return FALSE;
				}
                continue;
            }
            return FALSE;
        }
		break;
    }while ( true );
    return TRUE;
}

BOOL CHttpClientSession::RequestPost( const CString& strURI )
{
	CString strTrace;
    LPCTSTR ppszAcceptTypes[2];
    ppszAcceptTypes[0] = _T( "*/*" );  //We support accepting any mime file type since this is a simple download of a file
    ppszAcceptTypes[1] = NULL;
    if( !OpenRequest( TRUE, strURI, ppszAcceptTypes ) )
    {
        return FALSE;
    }


    CString strContentType = m_strContentType;//_T( "Content-Type: application/oct-stream\r\n" );
    std::string cache;
    GetPostArgumentsBuffer( cache );
    //if ( cache.empty() )
    //{
    //    return FALSE;
    //}

	CString strContentLength;
	strContentLength.Format( _T("Content-Length: %u\r\n") , cache.size() ); 
    AddOneRequestHeader( strContentLength  );

    if ( !::HttpAddRequestHeaders( m_hHTTPRequest, m_strAcceptType, m_strAcceptType.GetLength(), HTTP_ADDREQ_FLAG_REPLACE ) )
    {
		strTrace.Format( _T("In CHttpClientSession::RequestPost: HttpAddRequestHeaders false , m_strAcceptType = %s GetLastError = %d") , m_strAcceptType, GetLastError() );
		///pcutil::ClientLogger::BizInfo( _T("CHttpClientSession::Request") , strTrace );
		//LOG_ERROR(strTrace);
    }

   // add user header
    for ( unsigned int i = 0 ; i < m_vRequestHeader.size() ; i++ )
    {
        if ( !::HttpAddRequestHeaders( m_hHTTPRequest,
                                       m_vRequestHeader[i].first,
                                       m_vRequestHeader[i].first.GetLength(),
                                       m_vRequestHeader[i].second ) )
        {
			strTrace.Format( _T("In CHttpClientSession::RequestPost: HttpAddRequestHeaders false , m_vRequestHeader[i].first = %s  second:%d GetLastError = %d") , m_vRequestHeader[i].first, m_vRequestHeader[i].second , GetLastError() );
			//pcutil::ClientLogger::BizInfo( _T("CHttpClientSession::Request") , strTrace );
			//LOG_ERROR(strTrace);
            continue;
        }
    }

	int nRetryCount = 0;
    do
    {
		if( ++nRetryCount > 2 )
		{
			return FALSE;
		}
        SetLastError( 0 );
		if ( !::HttpSendRequest(
					m_hHTTPRequest,				// handle by returned HttpOpenRequest
					strContentType,				// additional HTTP header
					strContentType.GetLength(),	// additional HTTP header length
					( void* )cache.c_str(),					// additional data in HTTP Post or HTTP Put
					cache.size()		// additional data length
				)
		   )
		{
			DWORD dwError = GetLastError();
			strTrace.Format( _T("In CHttpClientSession::RequestPost: HttpSendRequest false , dwError = %d ") , dwError  );
			//pcutil::ClientLogger::BizInfo( _T("CHttpClientSession::Request") , strTrace );
			//if ( ERROR_INTERNET_INVALID_CA == dwError )
			if( m_bHttps )
			{
                DWORD dwFlags = 0;
                DWORD dwBuffLen = sizeof( dwFlags );
                InternetQueryOption ( m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                      ( LPVOID )&dwFlags, &dwBuffLen );
                //dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ; 
				//dwFlags |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ;
                //dwFlags |= SECURITY_FLAG_IGNORE_REVOCATION ;
                InternetSetOption ( m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                    &dwFlags, sizeof ( dwFlags ) );
				
				//int nSleepTimes = 1;
				//do
				//{
				//	Sleep(nSleepTimes * 500);
				//	nSleepTimes++;
				//	if (m_bHttpClosed)
				//	{
				//		return FALSE;
				//	}
				//} while (nSleepTimes * 500 <= nRetryCount * 1000);


				Sleep(100);
				if (m_bHttpClosed)
				{
					return FALSE;
				}
                continue;
			
			}
			return FALSE;
		}
		break;
	}while( true );
    return TRUE;
}

BOOL CHttpClientSession::RequestPostMultiParts( const CString& szURI )
{
    CString strContentType;
    strContentType.Format( _T( "Content-Type: multipart/form-data; boundary=%s\r\n" ) , m_strBoundaryName ) ;

    //ALLOCATE POST MULTI-PARTS FORM DATA ARGUMENTS
    std::string cache;
    AllocMultiPartsFormData( cache , m_strBoundaryName );

    LPCTSTR ppszAcceptTypes[2];
    ppszAcceptTypes[0] = _T( "*/*" );  //We support accepting any mime file type since this is a simple download of a file
    ppszAcceptTypes[1] = NULL;
    if( !OpenRequest( TRUE, szURI, ppszAcceptTypes ) )
    {
        return FALSE;
    }

    // REPLACE HEADER
    if ( !::HttpAddRequestHeaders( m_hHTTPRequest, m_strAcceptType, m_strAcceptType.GetLength(), HTTP_ADDREQ_FLAG_REPLACE ) )
    {
        return FALSE;
    }

    if ( !::HttpAddRequestHeaders( m_hHTTPRequest, strContentType, strContentType.GetLength(), HTTP_ADDREQ_FLAG_ADD_IF_NEW ) )
    {
        return FALSE;
    }

    CString strCL;
    strCL.Format( _T( "Content-Length: %u\r\n" ) , cache.size() );
    if ( !::HttpAddRequestHeaders( m_hHTTPRequest, strCL, strCL.GetLength(), HTTP_ADDREQ_FLAG_ADD_IF_NEW ) )
    {
        return FALSE;
    }

    //add user header
    for ( unsigned int i = 0 ; i < m_vRequestHeader.size() ; i++ )
    {
        if ( !::HttpAddRequestHeaders( m_hHTTPRequest,
                                       m_vRequestHeader[i].first,
                                       m_vRequestHeader[i].first.GetLength(),
                                       m_vRequestHeader[i].second ) )
        {
            return FALSE;
        }
    }

	int nRetryCount = 0;
    // SEND REQUEST WITH HttpSendRequestEx and InternetWriteFile
    INTERNET_BUFFERS InternetBufferIn = {0};
    InternetBufferIn.dwStructSize = sizeof( INTERNET_BUFFERS );
    InternetBufferIn.Next = NULL;
    do
    {
		if( ++nRetryCount > 2 )
		{
			return FALSE;
		}
		SetLastError( 0 );
		if ( !::HttpSendRequestEx( m_hHTTPRequest, &InternetBufferIn, NULL, HSR_INITIATE, 0 ) )
		{
			DWORD dwError = GetLastError();
			//if ( ERROR_INTERNET_INVALID_CA == dwError )
			if( m_bHttps )
			{
	            DWORD dwFlags = 0;
                DWORD dwBuffLen = sizeof( dwFlags );
                InternetQueryOption ( m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                      ( LPVOID )&dwFlags, &dwBuffLen );
                //dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ; 
				//dwFlags |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ;
                //dwFlags |= SECURITY_FLAG_IGNORE_REVOCATION ;
                InternetSetOption ( m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                    &dwFlags, sizeof ( dwFlags ) );
				//int nSleepTimes = 1;
				//do
				//{
				//	Sleep( nSleepTimes * 500 );
				//	nSleepTimes++;
				//	if( m_bHttpClosed )
				//	{
				//		return FALSE;
				//	}
				//}while( nSleepTimes * 500 <= nRetryCount * 1000 );

				Sleep(100);
				if (m_bHttpClosed)
				{
					return FALSE;
				}
                continue;
			}
			return FALSE;
		}
		break;
	}while( false );

    DWORD dwOutPostBufferLength = 0;
    if ( !::InternetWriteFile( m_hHTTPRequest, cache.data(), cache.size(), &dwOutPostBufferLength ) )
    {
        return FALSE;
    }

    if ( !::HttpEndRequest( m_hHTTPRequest, NULL, HSR_INITIATE, 0 ) )
    {
        return FALSE;
    }

    return TRUE;
}

int CHttpClientSession::ReadResponse( std::string &data )				// >0
{
    //ATLASSERT( m_bTwoStepRequest );
    CAutoBuffer &abuf = m_cachePartResponse;
    unsigned int abufsize = m_buffSize;
	if( abuf.GetBufferSize() <= 0 )
	{
		if ( !abuf.AllocateBuffer( abufsize ) )
		{
			return -1;
		}
	}


	DWORD dwNumOfBytesToRead = 0;
	ZeroMemory( abuf.GetBuffer() , abuf.GetBufferSize() );
    BOOL bReadOK = ::InternetReadFile(	m_hHTTPRequest,
                                       abuf.GetBuffer(),
                                       abuf.GetBufferSize(),
                                       &dwNumOfBytesToRead
                                     );
    if ( bReadOK )
    {
        data.append( abuf.GetBuffer() , dwNumOfBytesToRead );
    }
    else
    {
        return -1;
    }

	ZeroMemory( abuf.GetBuffer() , abufsize );

	if ( m_strResponseHeaders.IsEmpty() )
    {
        ZeroMemory( abuf.GetBuffer() , abufsize );
        DWORD dwHeaderBufferLength = abufsize - 1;
        if ( !::HttpQueryInfo( m_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, abuf.GetBuffer(), &dwHeaderBufferLength, NULL ) )
        {
            return -1;
        }
        m_strResponseHeaders = ( TCHAR* )abuf.GetBuffer() ;
		m_ptrResponseHeaders->Parse( m_hHTTPRequest , m_strResponseHeaders );
		if( m_ptrResponseHeaders->GetStatusCode() == HTTP_STATUS_MOVED 
						|| m_ptrResponseHeaders->GetStatusCode() == HTTP_STATUS_REDIRECT 
						|| m_ptrResponseHeaders->GetStatusCode() == HTTP_STATUS_REDIRECT_METHOD
						 )
		{
			data.clear();
			return 0;
		}

    }

    return dwNumOfBytesToRead;
}

BOOL CHttpClientSession::Response( DWORD &dwResultSize )
{
	DWORD dwNumOfBytesToRead = 0;
	DWORD dwStartResponseTimeStamp = GetTickCount();
	BOOL bOK = FALSE;
	do
	{
		dwNumOfBytesToRead = 0;
		bOK = InternetQueryDataAvailable( m_hHTTPRequest , &dwNumOfBytesToRead , 0 , 0 ) ;
		if( !bOK )
		{
			Sleep( 100 );
		}
		else if(  /*CommonHelper*/CStringHelper::TimeDiff( GetTickCount() , dwStartResponseTimeStamp ) > 5 * 1000 )
		{
			break;
		}

	}while( !m_bHttpClosed && !bOK );

    if ( m_bTwoStepRequest )
	{
		return TRUE;
	}

    CAutoBuffer abuf;
    unsigned int abufsize = 1024 * 100;
    if ( !abuf.AllocateBuffer( abufsize ) )
    {
        return FALSE;
    }


    ZeroMemory( abuf.GetBuffer() , abufsize );
    dwNumOfBytesToRead = 0;
    m_bufResponseContent.clear();

	if (!m_onlyReadHeader)
	{
		BOOL bReadOK = FALSE;
		do
		{
			dwNumOfBytesToRead = 0;
			bReadOK = ::InternetReadFile(m_hHTTPRequest,
				abuf.GetBuffer(),
				abufsize,
				&dwNumOfBytesToRead
			);
			if (bReadOK)
			{
				m_bufResponseContent.append(abuf.GetBuffer(), dwNumOfBytesToRead);
			}
			ZeroMemory(abuf.GetBuffer(), abufsize);
		} while (bReadOK && dwNumOfBytesToRead > 0);
		if (!bReadOK)
		{
			return FALSE;
		}
	}

	ZeroMemory( abuf.GetBuffer() , abufsize );
	DWORD dwHeaderBufferLength = abufsize - 1;
	if ( !::HttpQueryInfo( m_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, abuf.GetBuffer(), &dwHeaderBufferLength, NULL ) )
	{
		return FALSE;
	}
	m_strResponseHeaders = ( TCHAR* )abuf.GetBuffer();
	m_ptrResponseHeaders->Parse( m_hHTTPRequest , m_strResponseHeaders );

	//if (m_ptrResponseHeaders->GetContentEncoding().MakeLower() == _T("gzip") )
	//{
	//	string content = m_bufResponseContent;
	//	CZlibStream gzipstream;
	//	gzipstream.DecompressGZip( content , m_bufResponseContent );
	//}
	
    return TRUE;
}

void CHttpClientSession::GetPostArgumentsBuffer( std::string &cache )
{
    cache.clear();
    std::vector<CHttpPostArgument>::iterator itArg;
    for ( itArg = m_vArguments.begin(); itArg < m_vArguments.end(); ++itArg )
    {
        if ( ( *itArg ).GetType() == CHttpPostArgument::TypeNormal )
        {
            cache = cache + ( *itArg ).GetName() + std::string( "=" ) + ( *itArg ).GetValue();
            if ( itArg + 1 != m_vArguments.end() )
            {
                cache +=  "&" ;
            }
        }
        else if ( ( *itArg ).GetType() == CHttpPostArgument::TypeRawText )
        {
            cache = cache + ( *itArg ).GetValue();
            if ( m_vArguments.size() != 1 )
            {
                ATLASSERT( FALSE );
            }
        }
    }
}

void CHttpClientSession::GetPostArgumentsBufferForLog(std::string& cache)
{
	cache.clear();
	std::vector<CHttpPostArgument>::iterator itArg;
	for ( itArg = m_vArguments.begin(); itArg < m_vArguments.end(); ++itArg )
	{
		bool isSecret = false;
		if(( *itArg ).GetName() == "password" || ( *itArg ).GetName() == "newPassword" || ( *itArg ).GetName() == "loginPWD" ) isSecret = true;

		if ( ( *itArg ).GetType() == CHttpPostArgument::TypeNormal )
		{
			cache = cache + ( *itArg ).GetName() + std::string( "=" ) + (isSecret ? "******" : ( *itArg ).GetValue());
			if ( itArg + 1 != m_vArguments.end() )
			{
				cache +=  "&" ;
			}
		}
		else if ( ( *itArg ).GetType() == CHttpPostArgument::TypeRawText )
		{
			cache = cache + (isSecret ? "******" : ( *itArg ).GetValue());
			if ( m_vArguments.size() != 1 )
			{
				ATLASSERT( FALSE );
			}
		}
	}
}

void CHttpClientSession::AllocMultiPartsFormData( std::string &cache, LPCTSTR szBoundary )
{
    cache.clear();
    std::vector<CHttpPostArgument>::iterator itArgv;
    for ( itArgv = m_vArguments.begin(); itArgv < m_vArguments.end(); ++itArgv )
    {
        // SET MULTI_PRATS FORM DATA BUFFER
        // BUG：超过4096则崩溃
        // char strFormItem[4096] = {'\0'};

        switch ( ( *itArgv ).GetType() )
        {
            case CHttpPostArgument::TypeRawText:
                ATLASSERT( FALSE );
                break;

            case CHttpPostArgument::TypeNormal:
                //sprintf( strFormItem,
                //         "--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n"
                //         , CT2A( szBoundary ).m_psz , ( *itArgv ).GetName().c_str() , ( *itArgv ).GetValue().c_str()
                //       );
                //cache.append(  strFormItem );

                cache.append( "--");
                cache.append(CT2A( szBoundary ).m_psz);
                cache.append("\r\nContent-Disposition: form-data; name=\"");
                cache.append(( *itArgv ).GetName().c_str());
                cache.append("\"\r\n\r\n");
                cache.append(( *itArgv ).GetValue().c_str());
                cache.append("\r\n" );
				break;
				//////////////////////////////////////////////////////////////////////////
				// Post 一个磁盘文件的方法是没有用的，不允许干这种事情！
				// 另实现为直接 Post 二进制流参数
				//case CHttpPostArgument::TypeBinary:
				//HANDLE hFile =::CreateFile( CA2T( ( *itArgv ).GetValue().c_str() ).m_psz,	//File Name
				//                            GENERIC_READ,
				//                            FILE_SHARE_READ,
				//                            NULL,
				//                            OPEN_EXISTING,
				//                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, // flag and attributes
				//                            NULL
				//                          );
				//if ( hFile == NULL ) return ;
				//LONGLONG dwSize =::GetFileSize( hFile, NULL );
				//BOOL bNotFirstpackage = TRUE;
				//BYTE	pBytes[4096] = {'\0'};
				//DWORD	dwNumOfBytesToRead = 0;
				//while ( ::ReadFile( hFile, pBytes, 4096, &dwNumOfBytesToRead, NULL ) && dwNumOfBytesToRead > 0 )
				//{
				//    if ( bNotFirstpackage )
				//    {
				//        std::string strContextType = "application/octet-stream";
				//        LPWSTR	lMime = NULL;
				//        HRESULT hr = ::FindMimeFromData( NULL, NULL, pBytes, dwNumOfBytesToRead, NULL, 0, &lMime, 0 );
				//        if ( NOERROR == hr )
				//        {
				//            strContextType = CW2A( lMime ).m_psz;
				//        }
				//        sprintf( strFormItem, "--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n" ,
				//                 CT2A( szBoundary ).m_psz , ( *itArgv ).GetName().c_str() ,
				//                 ( *itArgv ).GetValue().c_str() , strContextType.c_str() );

				//        cache.append(  strFormItem  );
				//        bNotFirstpackage = FALSE;
				//    }
				//    cache.append( ( const char* )pBytes , dwNumOfBytesToRead );
				//}
				//::CloseHandle( hFile );
				//cache.append( "\r\n" );
				//break;
		}
	}
	std::vector<CHttpPostBinaryRefArgument>::iterator it;
	for ( it = m_vBinaryRefArguments.begin(); it < m_vBinaryRefArguments.end(); ++it )
	{
		CString header;
		header.Format( _T( "--%s\r\nContent-Disposition: form-data; name=\"%s\";filename=\"%s\"\r\nContent-Type: %s\r\n\r\n" ),
			szBoundary, it->ArgName, it->FileName, it->ContentType );
		cache.append( /*CommonHelper*/CStringHelper::ConvertLocalToUtf8( header ) );
		cache.append( it->BinaryCache.data(), it->Length );
		cache.append( "\r\n" );
	}
    cache.append( "--" );
    cache.append( CT2A( szBoundary ).m_psz );
	cache.append( "--" );
    cache.append( "\r\n" );
    return ;
}


void CHttpClientSession::ResetRequestHandle()
{
	if (m_hHTTPRequest != NULL) {
		::InternetCloseHandle(m_hHTTPRequest);
		m_hHTTPRequest = NULL;
	}
	if (m_hHTTPConnect != NULL) {
		::InternetCloseHandle(m_hHTTPConnect);
		m_hHTTPConnect = NULL;
	}
	if (m_hHTTPOpen != NULL) {
		::InternetCloseHandle(m_hHTTPOpen);
		m_hHTTPOpen = NULL;
	}
}

BOOL CHttpClientSession::OpenRequest( BOOL bPost, const CString &strURI,  LPCTSTR* szAcceptType )
{
	if( m_bHttpClosed ) return FALSE;
    DWORD dwFlag = INTERNET_FLAG_KEEP_CONNECTION |  INTERNET_FLAG_NO_CACHE_WRITE 
					|INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_COOKIES | 
					INTERNET_FLAG_NO_UI | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_PRAGMA_NOCACHE;
    CString strPut = _T( "GET" );
    if ( bPost )
    {
        strPut = _T( "POST" );
    }
	dwFlag |= INTERNET_FLAG_FORMS_SUBMIT;

    if ( m_bHttps )
    {
        dwFlag |= INTERNET_FLAG_SECURE;
        dwFlag |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
        dwFlag |= INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        dwFlag |= INTERNET_FLAG_EXISTING_CONNECT;
		//dwFlag |= ~INTERNET_FLAG_NO_COOKIES;
	}

	if( m_hHTTPRequest != NULL )
	{
		::InternetCloseHandle( m_hHTTPRequest );
		m_hHTTPRequest = NULL;
	}

    m_hHTTPRequest = HttpOpenRequest( m_hHTTPConnect,
                            strPut, // HTTP Verb
                            strURI, // Object Name
                            _T( "HTTP/1.1" ), // Version
                            m_strReferrerName.IsEmpty() ? NULL : ( LPCTSTR )m_strReferrerName, // Reference
                            szAcceptType, // Accept Type
                            dwFlag,
                            0 ); // context call-back point
	
	return m_hHTTPRequest != NULL ;
	
}


BOOL CHttpClientSession::RequestPostMultiPartsFormData( LPCTSTR szURI )
{

    return TRUE;
}

void CHttpClientSession::SetBufferSize( DWORD bufferSize )
{
	m_buffSize = bufferSize;
}

void CHttpClientSession::SetProxyType( DWORD proxyType )
{
	m_dwProxyType = proxyType;
}

DWORD CHttpClientSession::GetProxyType()
{
	return m_dwProxyType;
}

}

void SetHttpTimeoutValue(int value)
{
	g_http_timeout_value = value;
}

void SetDefaultProxyType(int value)
{
	g_default_dwProxyType = value;
}

void SetHttpProxyRetry(bool value)
{
	g_http_proxy_retry = value;
}

void SetHttpRedirectGet(bool value)
{
	g_redirect_method_get = value;
}

//CHttpClientSession httpClient;
//httpClient.AddPostArguments("UserID", "liuweigang");
//httpClient.AddPostArguments("Sign", "liuweigang");
//httpClient.AddPostArguments("File", "c:\\test.gif", true);
////Multipart
//BOOL bReturn = httpClient.Request("http://q.sohu.com/test.jsp", GenericHTTPClient::RequestPostMethodMultiPartsFormData);
////Post





//
//int nRet = 1;
//LPCTSTR lpszAgent = "WinInetGet/0.1";
//HINTERNET hInternet = InternetOpen(lpszAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
//if (!hInternet) {
//	return 1;
//}
//LPCTSTR lpszServerName = "192.168.22.199"; //设置server  
//INTERNET_PORT nServerPort = INTERNET_DEFAULT_HTTPS_PORT; // HTTPS端口443  
//LPCTSTR lpszUserName = NULL; //无登录用户名  
//LPCTSTR lpszPassword = NULL; //无登录密码  
//DWORD dwConnectFlags = 0;
//DWORD dwConnectContext = 0;
//HINTERNET hConnect = InternetConnect(hInternet,
//	lpszServerName, nServerPort,
//	lpszUserName, lpszPassword,
//	INTERNET_SERVICE_HTTP,
//	dwConnectFlags, dwConnectContext);
//if (!hConnect) {
//	return 1;
//}
//LPCTSTR lpszVerb = "GET";
//LPCTSTR lpszObjectName = "/";
//LPCTSTR lpszVersion = NULL;    // 默认.  
//LPCTSTR lpszReferrer = NULL;   // 没有引用页  
//LPCTSTR *lplpszAcceptTypes = NULL; // Accpet所有类型.  
//DWORD dwOpenRequestFlags = INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
//INTERNET_FLAG_KEEP_CONNECTION |
//INTERNET_FLAG_NO_AUTH |
//INTERNET_FLAG_NO_COOKIES |
//INTERNET_FLAG_NO_UI |
////设置启用HTTPS  
//INTERNET_FLAG_SECURE |
//INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
//INTERNET_FLAG_RELOAD;
//DWORD dwOpenRequestContext = 0;
////初始化Request  
//HINTERNET hRequest = HttpOpenRequest(hConnect, lpszVerb, lpszObjectName, lpszVersion,
//	lpszReferrer, lplpszAcceptTypes,
//	dwOpenRequestFlags, dwOpenRequestContext);
//if (!hRequest) {
//	goto GOTO_EXIT;
//}
////发送Request  
//BOOL bResult = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
//if (!bResult && GetLastError() == ERROR_INTERNET_INVALID_CA)
//{
//	DWORD dwFlags;
//	DWORD dwBuffLen = sizeof(dwFlags);
//	InternetQueryOption(hRequest, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
//	dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
//	InternetSetOption(hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
//	bResult = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
//}
//if (!bResult) {
//	goto GOTO_EXIT;
//}
////获得HTTP Response Header信息  
//char szBuff[TRANSFER_SIZE];
//DWORD dwReadSize = TRANSFER_SIZE;
//bResult = HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, szBuff, &dwReadSize, NULL);
//if (!bResult) {
//	goto GOTO_EXIT;
//}
//szBuff[dwReadSize] = '/0';
//printf("%s/n", szBuff);
////HTTP Response 的 Body  
//DWORD dwBytesAvailable;
//bResult = InternetQueryDataAvailable(hRequest, &dwBytesAvailable, 0, 0);
//if (!bResult) {
//	goto GOTO_EXIT;
//}
//if (dwBytesAvailable > TRANSFER_SIZE)
//{
//	printf("tool long %d /b", GetLastError(), dwBytesAvailable);
//	goto GOTO_EXIT;
//}
//DWORD dwBytesRead;
//bResult = InternetReadFile(hRequest, szBuff, dwBytesAvailable, &dwBytesRead);
//if (!bResult) {
//	goto GOTO_EXIT;
//}
//szBuff[dwBytesRead] = '/0';
//printf("%s/n", szBuff);
//nRet = 0;
//GOTO_EXIT:
//if (hRequest) {
//	InternetCloseHandle(hRequest);
//}
//if (hConnect) {
//	InternetCloseHandle(hConnect);
//}
//if (hInternet) {
//	InternetCloseHandle(hInternet);
//}