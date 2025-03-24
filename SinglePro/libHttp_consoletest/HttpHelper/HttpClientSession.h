#pragma once
#include <Windows.h>
#include "AutoBuffer.h"
#include "ClientConfig.h"
#include "HttpPostArgument.h"
#include "HttpResponseHeaders.h"
#include <vector>
namespace pcutil
{


	class WEAVERLIB_API_UTILS CHttpClientSession
	{
	public:
		CHttpClientSession();
		virtual ~CHttpClientSession();

	public:
		static CString GetContentType( const CString &strExtName );

	public:
		enum RequestMethod		// REQUEST METHOD
		{
			RequestUnknown = 0,
			RequestGetMethod = 1,
			RequestPostMethod = 2,
			RequestPostMethodMultiPartsFormData = 3
		};

	public:
		virtual void InitilizePostArguments();
		virtual void AddPostArguments( LPCTSTR lpName, DWORD nValue );
		virtual void AddPostArguments( LPCTSTR lpName, LPCTSTR lpValue );
		virtual void AddPostBinaryRefArgument( LPCTSTR argName, const CString& fileName, LPCSTR pBuffer, DWORD len, LPCTSTR contentType = _T( "application/octet-stream" )  );

		virtual void AddPostRawText(  const std::string& szValue );
		virtual void AddPostCStringBody( const CString& strBody );
		virtual BOOL AddOneRequestHeader( const CString &strHeader, DWORD dwFlag = HTTP_ADDREQ_FLAG_ADD_IF_NEW );
		virtual void RemoveAllRequestHeader();

		virtual BOOL Request( LPCTSTR szURL, CHttpClientSession::RequestMethod Method = CHttpClientSession::RequestGetMethod , BOOL bAutoClose = TRUE );
		virtual BOOL Response( DWORD &dwResultSize );
		virtual int ReadResponse( std::string &data );				// >0

		BOOL Close();

	protected:
		void ParseURL(	const CString &strURL, 
			CString &strProtocol, 
			CString &strAddress, 
			unsigned short &nPort, 
			CString &strURI 
			); 

		BOOL RequestOfURI( const CString &strURI , CHttpClientSession::RequestMethod Method );
		BOOL Connect( 
			const CString &strAddress, 
			unsigned short nPort = INTERNET_DEFAULT_HTTP_PORT, 
			LPCTSTR szUserAccount = NULL, 
			LPCTSTR szPassword = NULL 
			);


		//property
	public:
		void SetMaxResponseContentSize( DWORD dwMaxResponseContentSize );
		DWORD GetMaxResponseContentSize();
		void SetAgentName( const CString &strAgentName );
		CString GetAgentName();
		void SetReferrerName( const CString &strReferrerName );
		CString GetReferrerName();
		void SetBoundaryName( const CString &strBoundaryName );
		CString GetBoundaryName();

		void SetBufferSize( DWORD bufferSize );
		void SetProxy( DWORD dwType ,const CString &strProxyNames , 
			const CString &strProxyUser , const CString &strProxyPwd );
		//void SetProxyType( DWORD dwType  ){}

		void SetContentType( const CString &strContentType );

		void SetConnectionKeepAlive( BOOL bConnectionKeepAlive );

		virtual std::string& GetResponseContent();
		virtual CString& GetResponseHeader();
		virtual CString GetResponseTextContent();

		void SetTwoStepRequest( BOOL bTwoStepRequest );

		CHttpResponseHeadersPtr GetResponseHeaderPtr();
		void SetProxyType(DWORD proxyType );
		DWORD GetProxyType();

		/*
		-1 ÍøÂçÎ´Á¬½Ó 
		-2 Response false
		-3 Request error 
		-4 http conn error
		*/
		int m_errorCode = 0;
	protected:
		virtual BOOL OpenRequest( BOOL bPost, const CString& strURI,  LPCTSTR* szAcceptType );
		virtual BOOL RequestPost( const CString& strURI );
		virtual BOOL RequestPostMultiParts( const CString& szURI );
		virtual BOOL RequestGet( const CString& strURI );
		virtual BOOL RequestPostMultiPartsFormData( LPCTSTR szURI );

		virtual void GetPostArgumentsBuffer( std::string &cache );
		virtual void GetPostArgumentsBufferForLog(std::string& cache);
		virtual void AllocMultiPartsFormData( std::string &cache, LPCTSTR szBoundary );

		void ResetRequestHandle();

		//WinInet Handles
	protected:
		volatile HINTERNET m_hHTTPOpen;
		volatile HINTERNET m_hHTTPConnect;
		volatile HINTERNET m_hHTTPRequest;

		//Property
	protected:
		//CString m_strServerAddress;
		//unsigned short m_nServerPort; 
		CString m_strAgentName;
		CString m_strReferrerName;
		CString m_strBoundaryName;
		CString m_strContentType;

		DWORD m_dwMaxResponseContentSize;
		//	DWORD m_dwMaxResponseHeaderSize;

		std::string m_bufResponseContent;
		CString m_strResponseHeaders;

		std::vector< CHttpPostArgument > m_vArguments;
		std::vector<CHttpPostBinaryRefArgument> m_vBinaryRefArguments;

		volatile BOOL m_bHttps;

		DWORD m_dwProxyType;
		CString m_strProxyNames;
		CString m_strProxyUser;
		CString m_strProxyPwd;


		BOOL m_bTwoStepRequest;
		BOOL m_bConnectionKeepAlive;

		CHttpResponseHeadersPtr m_ptrResponseHeaders;
		CAutoBuffer m_cachePartResponse;

		volatile BOOL m_bHttpClosed;
		DWORD m_buffSize;

		CString m_strRequestURL;

		BOOL m_onlyReadHeader;

	private:
		const CString m_strAcceptType;
		std::vector< std::pair< CString , DWORD > > m_vRequestHeader;

#ifdef _DEBUG
		RequestMethod m_emRequestMethod;
#endif
		BOOL m_requestWithDody;

	};

}

void SetHttpTimeoutValue(int value);
void SetDefaultProxyType(int value);
void SetHttpProxyRetry(bool value);
void SetHttpRedirectGet(bool value);
