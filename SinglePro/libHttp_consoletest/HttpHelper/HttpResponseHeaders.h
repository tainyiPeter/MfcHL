#pragma once
#include "ClientConfig.h"
#include <atlstr.h>
#include <vector>
#include <string>
#include "auxtools.h"
#include "UsableValue.h"
#include "shared_ptr.h"

namespace pcutil
{

struct THttpResponseCookie
{
	CString name;
	CString value;
	CString path;
	CString domain;
	CString expires;
	CString maxage;
	CString secure;
	CString httponly;
};
	
	
class WEAVERLIB_API_UTILS CHttpResponseHeaders
{

public:
	friend class CHttpClientSession;
	virtual ~CHttpResponseHeaders();


private:
	CHttpResponseHeaders();
	void Parse( HINTERNET hRequest , const CString &rawheader );
	BOOL ParseCookie( std::string &cookie_line , THttpResponseCookie &ret );



public:
	int GetStatusCode();
	CUsableValue< THttpResponseCookie > GetCookie( const CString &name );
	CUsableValue<unsigned __int64> GetContentLength();
	CUsableValue<unsigned __int64> GetFileSize();
	CUsableValue< CString > GetRawHeaders();
	CString GetLocation();
	CString GetContentEncoding();
	CUsableValue< std::pair< int , CString > > GetRefreshInfo();
	CString GetContentType();
	CString GetContentDisposition();
private:
	int m_statusCode;
	CUsableValue<unsigned __int64> m_contentLength;
	std::vector< THttpResponseCookie > m_vCookies;
	CUsableValue< CString > m_rawHeaders;
	CString m_location;
	CUsableValue< std::pair< int , CString > > m_pairRefresh;
	CString m_strContentType; 
	CString m_contentEncoding;
	CString m_contentDisposition;
	CUsableValue<unsigned __int64> m_uFileSize;

	DISALLOW_COPY_AND_ASSIGN( CHttpResponseHeaders );
};

typedef CSharedPtr< CHttpResponseHeaders > CHttpResponseHeadersPtr;

}
