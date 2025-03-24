#pragma once
#include <Windows.h>
#include <wininet.h>
#include "ClientConfig.h"
#include "HttpClientSession.h"
#include "auxtools.h"

namespace pcutil
{

enum EWebResponseCategory
{
	RESPONSETEXT,
	RESPONSEMEMORY,
	RESPONSEFILE,
	RESPONSEXML
};


class WEAVERLIB_API_UTILS CWebRequest
	:public pcutil::CHttpClientSession
{
public:
	virtual ~CWebRequest();

protected:
	CWebRequest();
	explicit CWebRequest(  EWebResponseCategory em );


public:
	virtual BOOL Request( LPCTSTR szURL, CHttpClientSession::RequestMethod Method = CHttpClientSession::RequestGetMethod );
	void SetTimeout( const DWORD dwTimeout );
	CString GetIdentityID();
	void DisableUserProxySetting( BOOL bDisabled  );
	void UserAbort(  BOOL bAbort = TRUE );
	BOOL IsUserAbort();

public:
	void OnRecvResponseData( std::string &data , BOOL bDownloadOK );

	virtual BOOL StartRequest();
    virtual BOOL Response( DWORD &dwResultSize );
	virtual BOOL Connect( const CString &strAddress, unsigned short nPort = INTERNET_DEFAULT_HTTP_PORT, 
				  LPCTSTR szUserAccount = NULL, 
				  LPCTSTR szPassword = NULL 
				 );
    virtual BOOL OpenRequest( BOOL bPost, const CString& strURI,  LPCTSTR* szAcceptType );
	virtual BOOL Close();

	EWebResponseCategory GetResponseCategory();
	CString GetTextResponse();
	std::string& GetBinaryResponse();
	CString GetResponseHeaderText();

	DWORD GetTimeout(); 

	volatile BOOL m_bClosed;
	volatile BOOL m_bUserAbort;
	BOOL m_bDisabledUserProxySetting;

protected:
	CString GetDownloadFileName();
	BOOL IsDisabledUserProxySetting();

protected:
	CString m_strIdentityID;
	RequestMethod m_emRequestMethod;
	CString m_strRequestURL;
	EWebResponseCategory m_emRequestCategory;
	CString m_strDownloadFileName;
	DWORD m_dwTimeout; //∫¡√Îº∂
	
	DWORD m_dwDownloadBytes;
	std::string m_cachePartFileData;
	static const DWORD s_cachePartFileDataMaxSize = 1024 * 200;

	CComAutoCriticalSection m_csLock;
protected:
	virtual std::string& GetResponseContent();
    virtual CString& GetResponseHeader();




private:
	DISALLOW_COPY_AND_ASSIGN( CWebRequest );

};



}