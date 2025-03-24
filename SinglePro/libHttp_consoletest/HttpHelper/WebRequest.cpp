//#include "StdAfx.h"
#include "WebRequest.h"
#include "CStringHelper.h"
#include "CUUID.h"
//#include "CommonHelper.h"

namespace pcutil
{

CWebRequest::CWebRequest()
{
	m_emRequestMethod = CHttpClientSession::RequestGetMethod;
	m_emRequestCategory = RESPONSEMEMORY;
	m_bClosed = FALSE;
	m_bUserAbort = FALSE;
	m_dwDownloadBytes = 0;
	m_bDisabledUserProxySetting = FALSE;
}

CWebRequest::CWebRequest( EWebResponseCategory em )
{
	m_emRequestMethod = CHttpClientSession::RequestGetMethod;
	m_emRequestCategory = em;
	m_bClosed = FALSE;
	m_bUserAbort = FALSE;
	m_dwDownloadBytes = 0;
	m_bDisabledUserProxySetting = FALSE;
}

CWebRequest::~CWebRequest()
{
	//if( m_emRequestCategory == RESPONSEFILE && 
	//	!m_strDownloadFileName.IsEmpty() && 
	//	pcutil::FileSysHelper::IsFileExist(m_strDownloadFileName) )
	//{
	//	pcutil::FileSysHelper::DeleteFiles( m_strDownloadFileName );
	//}
	//if( m_emRequestCategory == RESPONSEFILE )
	//{
	//	pcutil::CAsyncFuncWndDispatcher::GetInstance()->RemoveAsyncFuncs( this );
	//}

}

BOOL CWebRequest::Request( LPCTSTR szURL, CHttpClientSession::RequestMethod Method  )
{
	m_strRequestURL = szURL;
	m_emRequestMethod = Method ;
	//m_strDownloadFileName = pcutil::FileSysHelper::GetTemporaryPath() + _T("\\") + m_strIdentityID + _T(".fxf");
	//return m_pFactory->AsyncRequest( GetIdentityID() );
	return StartRequest();
}

BOOL CWebRequest::StartRequest()
{
	if( m_bClosed ) return FALSE;
	if( m_emRequestCategory == RESPONSEFILE )
	{
		m_cachePartFileData.reserve( s_cachePartFileDataMaxSize );
	}
	SetLastError(0);
	BOOL bRequestOK = CHttpClientSession::Request( m_strRequestURL , m_emRequestMethod );
	if( !bRequestOK )
	{
		CString strInfo;
		strInfo.Format( _T("CHttpClientSession::Request Failed: m_strRequestURL = %s , GetLastError() = %d") ,
			m_strRequestURL , GetLastError() );
		//ClientLogger::BizInfo( _T("Biz.Util.WebRequest") , strInfo );
	}
	return bRequestOK;

}

BOOL CWebRequest::Response( DWORD &dwResultSize )
{
	BOOL bRet = TRUE;
	try
	{
		if( m_emRequestCategory == RESPONSEMEMORY || m_emRequestCategory == RESPONSETEXT || m_emRequestCategory == RESPONSEXML )
		{
			return CHttpClientSession::Response( dwResultSize );
		}
		else if( m_emRequestCategory == RESPONSEFILE )
		{
			SetTwoStepRequest( TRUE );
			CHttpClientSession::Response( dwResultSize );
			std::string buf;
			buf.reserve( 1024 * 160 );
			if( m_bClosed ) return FALSE;
			int nRead = CHttpClientSession::ReadResponse( buf );
			if( buf.size() >= 0 )
			{
				OnRecvResponseData( buf , ( nRead != -1 ) );
				buf.clear();
			}
			if( nRead == -1 )
			{
				throw -1;
			}
			
			int nRunLoop = 0;
			while( nRead > 0 )
			{
				buf.clear();
				buf.reserve( 1024 * 160 );
				if( m_bClosed ) return FALSE;
				nRead = CHttpClientSession::ReadResponse( buf );
				if( buf.size() >= 0 )
				{
					OnRecvResponseData( buf , ( nRead != -1 ) );
				}
				else
				{
					throw -1;
				}
				if( nRead == -1 )
				{
					throw -1;
				}
				if( ( ( ++nRunLoop ) % 5 ) == 0 && nRead > 0 )
					Sleep( 30 ); //避免Cpu使用率高~~(只是感觉待测试^_^) 
			}
			
			//CAtlFile atlFileDst;
			//HRESULT hr = atlFileDst.Create( m_strDownloadFileName , GENERIC_WRITE , 0 , CREATE_ALWAYS );
			//if( hr != S_OK ) throw -1;
			//std::string buf;
			//buf.reserve( 1024 * 10 );
			//int nRead = CHttpClientSession::ReadResponse( buf );
			//if( nRead < 0 ) throw -1;
			//int nRunLoop = 0;
			//while( nRead >= 0 )
			//{
			//	DWORD dwWrite = 0;
			//	if( atlFileDst.Write( buf.data() , (DWORD)nRead , &dwWrite ) != S_OK )
			//	{
			//		throw -1;				
			//	}
			//	buf.clear();
			//	nRead = CHttpClientSession::ReadResponse( buf );
			//	if( nRead < 0 ) throw -1;
			//	if( ( ( ++nRunLoop ) % 5 ) == 0 )
			//		Sleep( 30 ); //避免Cpu使用率高~~(只是感觉待测试^_^) 
			//}
		}
	}
	catch(...)
	{
		bRet = FALSE;
	}
	return bRet;
}

void CWebRequest::OnRecvResponseData( std::string &data , BOOL bDownloadOK )
{
	m_dwDownloadBytes += data.size();
	m_cachePartFileData.append( data );
	if( m_cachePartFileData.size() < s_cachePartFileDataMaxSize )
	{
		CUsableValue<unsigned __int64> lengthFile = m_ptrResponseHeaders->GetContentLength();
		if( lengthFile.IsUsable() )
		{
			ULONGLONG dsize = lengthFile.GetValue();
			dsize /= 100;
			if( m_cachePartFileData.size() < dsize && data.size() > 0 )
			{
				return ;
			}
		}
	}
	std::string tmpData = m_cachePartFileData; 
	m_cachePartFileData.clear();
	m_cachePartFileData.reserve( s_cachePartFileDataMaxSize );
	//if( !m_bClosed )
	//{
	//	m_pFactory->AsyncResponseProgress( GetIdentityID() , tmpData , m_dwDownloadBytes );
	//}


}

BOOL CWebRequest::Close()
{
	if (m_bClosed)
		return TRUE;
	BOOL bRet = TRUE;
	m_bClosed = TRUE;
	if( TRUE )
	{
		CComCritSecLock< CComAutoCriticalSection > guard( m_csLock );
		bRet = CHttpClientSession::Close();
	}
	return bRet;
}

CString CWebRequest::GetIdentityID()
{
	if( m_strIdentityID.IsEmpty() )
	{
		//pcutil::CommonHelper/*CStringHelper*/::GenerateUUID( m_strIdentityID );
		SmartEngine::CUUID::GenerateUUID(m_strIdentityID);
	}
	return m_strIdentityID;
}


BOOL CWebRequest::IsDisabledUserProxySetting()
{
	return m_bDisabledUserProxySetting;
}

void CWebRequest::DisableUserProxySetting( BOOL bDisabled  )
{
	m_bDisabledUserProxySetting = bDisabled;
}


EWebResponseCategory CWebRequest::GetResponseCategory()
{
	return m_emRequestCategory;
}

CString CWebRequest::GetTextResponse()
{
	std::string &res = GetResponseContent();
	return pcutil::/*CommonHelper*/CStringHelper::ConvertUtf8ToLocal( res );
}

std::string& CWebRequest::GetBinaryResponse()
{
	return GetResponseContent();
}

CString CWebRequest::GetResponseHeaderText()
{
	return CHttpClientSession::GetResponseHeader();
	//return pcutil::StringHelper::ConvertUtf8ToLocal( res );

}

std::string& CWebRequest::GetResponseContent()
{
	return CHttpClientSession::GetResponseContent();
}

CString CWebRequest::GetDownloadFileName()
{
	return m_strDownloadFileName;
}

CString& CWebRequest::GetResponseHeader()
{
	return CHttpClientSession::GetResponseHeader();
}

void CWebRequest::UserAbort( BOOL bAbort )
{
	m_bUserAbort = bAbort;
}

BOOL CWebRequest::IsUserAbort()
{
	return m_bUserAbort;
}

BOOL CWebRequest::Connect( const CString &strAddress, unsigned short nPort, 
							LPCTSTR szUserAccount, 
							LPCTSTR szPassword  
						)
{
	CComCritSecLock< CComAutoCriticalSection > guard( m_csLock );
	return CHttpClientSession::Connect( strAddress , nPort , szUserAccount , szPassword );
}

BOOL CWebRequest::OpenRequest( BOOL bPost, const CString& strURI,  LPCTSTR* szAcceptType )
{
	CComCritSecLock< CComAutoCriticalSection > guard( m_csLock );
	return CHttpClientSession::OpenRequest( bPost, strURI, szAcceptType );

}

}
