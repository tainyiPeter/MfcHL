//#include "Stdafx.h"
#include "HttpWebRequest.h"
#include <cassert>
#include <atlpath.h>

#include "threadpool.h"
//#include "io\FileHelper.h"
#include "GlobalTimer.h"
#include "LibThread.h"
//#include "LibBiz.h"
#include "FileHelper.h"
#include "log.h"

//#include "../../plugin/IPlugin.h"
//#include "../../plugin/IHostContext.h"
#include "CStringHelper.h"
#include "CSyncWindow.h"

#pragma comment( lib , "wininet.lib" )
//InternetGetConnectedState

//默认的http超时时间为20秒
#define WEAVER_HTTP_TIMEOUT 15*1000

bool g_enable_http_log = true;
CSharedPtr<CHttpWebRequest> CHttpWebRequest::Create(CString url, CHttpClientSession::RequestMethod method)
{
	CSharedPtr<CHttpWebRequest> request(new CHttpWebRequest(url));
	request->m_own = request;
	request->SetRequestMethod(method);
	return request;
}

CHttpWebRequest::CHttpWebRequest(CString url)
{
	m_timeout = WEAVER_HTTP_TIMEOUT;
	m_callback = NULL;
	m_strRequestURL = url;
	m_hasTimeOut = FALSE;
	m_finished = FALSE;
	m_downloadFile = NULL;
	m_progressCallback = NULL;
	//this->SetProxyType(CCoreConfigGetter::GetInstance()->GetProxyType());
	InitializeCriticalSection(&_CS);
}


void CHttpWebRequest::SetResponseCategory(EWebResponseCategory category)
{
	m_emRequestCategory = category;
}

void CHttpWebRequest::SetRequestMethod(CHttpClientSession::RequestMethod method)
{
	m_emRequestMethod = method;
}

void CHttpWebRequest::AsyncRequest(IAsyncCallback* callback)
{
	if (callback == NULL)
	{
		assert(0);
	}
	if (m_strRequestURL.IsEmpty())
	{
		CAsyncResult* asyncResult = new CAsyncResult(1);
		THttpResponseResult result;
		result.m_httpCode = 1;
		result.m_result = HTTP_RESULT_ERRORED;
		asyncResult->callbackResult = result;
		EXECUTE_CALL_BACK(callback, asyncResult);
		return;
	}
	m_callback = callback;

	//不再使用线程池，可能会导致线程池占用满了之后，无法继续http的请求
	CLibThread::Start(pcutil::bindAsyncFunc(&CHttpWebRequest::SyncStartRequest, this), true);
	//CBinderThreadPool::GetInstance()->AddTask( pcutil::bindAsyncFunc( &CHttpWebRequest::ProcessThreadRequest, this ) );

	//添加超时的定时器， CGlobalTimer保证线程安全
	//下载文件、断点续传，都不需要添加定时器
	if (!m_breakPoint.IsUsable())
		CGlobalTimer::GetInstance()->AddTimer(pcutil::bindAsyncFunc(&CHttpWebRequest::ProcessTimeOut, this), m_timeout, FALSE, FALSE);
}

void CHttpWebRequest::AsyncUploadFile(CString filePath, IAsyncCallback* callback, CString contentType /*=_T("image/jpeg") */)
{
	this->InitilizePostArguments();
	this->SetContentType(contentType);
	this->SetConnectionKeepAlive(TRUE);
	this->SetRequestMethod(CHttpClientSession::RequestMethod::RequestPostMethod);

	std::string content;
	char* pBuff;
	UINT64 length;

	if (CFileHelper::ReadAllBinary(filePath, pBuff, length))
	{
		content.assign(pBuff, length);
		this->AddPostRawText(content);
	}
	else
	{
		EXECUTE_STATUS_CODE_CALL_BACK(callback, 1);
	}

	this->AsyncRequest(callback);
}

BOOL CHttpWebRequest::DoSyncStartRequest()
{
	//初始化断点续传的信息
	if (m_breakPoint.IsUsable())
	{
		CString range;
		range.Format(_T("Range:bytes=%I64d-"), m_breakPoint.GetValue());
		this->AddOneRequestHeader(range);
	}

	//CString oemTag = CONFIGMANAGER->GetOEMTag();
	//if (!oemTag.IsEmpty())
	//{
	//	CString tag;
	//	tag.Format(_T("oem-tag:%s"),  oemTag );
	//	this->AddOneRequestHeader( tag );
	//}

	/*this->AddOneRequestHeader(_T("Client-Type:Win7"));
	this->AddOneRequestHeader(_T("Accept-Encoding:gzip"));

	CString version = pcutil::CFileHelper::GetProductVersion().ToString();
	if (!version.IsEmpty())
	{
	this->AddOneRequestHeader(_T("Client-Version:") + version);
	}

	CString macAddr;
	if (CommonHelper::GetMacAddress(macAddr))
	{
	this->AddOneRequestHeader(_T("Mac-addr:") + macAddr);
	}*/

	//CString userAgent = CONFIGMANAGER->GetUserAgent();
	//if(!userAgent.IsEmpty())
	//{
	//	this->SetAgentName(userAgent);
	//}

	//url里面如果带有单个的"%",打印日志就会崩溃,将该字符替换后输出
	//CString url1 = m_strRequestURL;
	//url1.Replace(L"%", L"%%");
	// 	WEAVERLOG( LOG_LEVEL_INFO, _T("开始HTTP请求:%s"), url1 );	//如启用该行代码，则debug模式下会弹出异常报警;
	if (g_enable_http_log)
	{
		LOG_INFO(_T("start HTTP request:%s"), m_strRequestURL);
	}
	BOOL requestOK = FALSE;
	try
	{
		DoRequestLog();
		requestOK = this->StartRequest();
		EnterCriticalSection(&_CS);

		m_finished = TRUE;

		//如果已经超时 则callback为NULL，此处处理为了析构this指针
		if (m_hasTimeOut/* || m_callback == NULL*/)
		{
			m_own.reset();
			LeaveCriticalSection(&_CS);
			if (g_enable_http_log)
			{
				LOG_INFO(_T("HTTP request:%s timeout."), m_strRequestURL);
			}
			return FALSE;
		}
		LeaveCriticalSection(&_CS);
		DoResponseLog();
	}
	catch (...)
	{
		requestOK = FALSE;
	}

	//if ( m_strRequestURL.Find(_T("http://ifaceshow.com/1.0/historyaction/list.json")) > -1 )
	//{
	//Sleep(1000);
	//}
	THttpResponseResult result;
	result.m_httpCode = this->GetResponseHeaderPtr()->GetStatusCode();
	result.m_request = m_own;
	////只有result持有m_request的指针，CHttpWebRequest会随着THttpResponseResult的析构而析构
	m_own.reset();

	result.m_result = requestOK ? HTTP_RESULT_SUCCEEDED : HTTP_RESULT_ERRORED;
	if (requestOK)
	{
		if (result.m_httpCode < 200 || result.m_httpCode >= 300)
		{
			result.m_result = HTTP_RESULT_FAILED;
			requestOK = FALSE;
			if (g_enable_http_log)
			{
				LOG_ERROR(_T("http request failed，address:%s,code：%d, error:%d"), m_strRequestURL, result.m_httpCode, m_errorCode);
			}
		}
		else
		{
			if (g_enable_http_log) {
				LOG_INFO(_T("http request successed:%s"), m_strRequestURL);
			}
		}
	}
	else
	{
		if (g_enable_http_log)
		{
			LOG_ERROR(_T("http request failled，address:%s,code：%d error:%d"), m_strRequestURL, result.m_httpCode, m_errorCode);
		}
	}
	if (m_callback)
	{
		SwitchToMainThread(pcutil::bindAsyncFunc(&CHttpWebRequest::ProcessThreadSwitched, this, result));
	}
	return requestOK;
}



BOOL CHttpWebRequest::SyncStartRequest()
{
	BOOL result = DoSyncStartRequest();
	//if (GetFrameHostContext() != NULL)
	//{
	//	//如果有网络的请求，则通知给框架，框架感知网络发生改变
	//	PluginNotify notifyInfo = { L"HttpRequestComplete" , result ? 1 : 0, NULL };
	//	GetFrameHostContext()->OnAppViewNotify(notifyInfo);
	//}
	return result;
}

void CHttpWebRequest::ProcessThreadSwitched(THttpResponseResult& result)
{
	//CComCritSecLock< CComAutoCriticalSection > guard(m_csLock);
	if (m_bClosed)
	{
		delete m_callback;
		return;
	}
	//WEAVERLOG( ELOGLEVEL::LOG_LEVEL_ERROR, _T("http switch thread callback:%s"),m_strRequestURL.GetString() );
	//此处调用 理论上会出现线程安全的问题 导致空指针访问，遗留问题、、
	CGlobalTimer::GetInstance()->DeleteTimer(this);

	CAsyncResult* asyncResult = new CAsyncResult();
	asyncResult->callbackResult = result;
	EXECUTE_CALL_BACK(m_callback, asyncResult);
	this->Close();
	m_callback = NULL;
}


void CHttpWebRequest::ProcessTimeOut()
{
	EnterCriticalSection(&_CS);

	if (m_finished)
	{
		LeaveCriticalSection(&_CS);
		return;
	}
	m_hasTimeOut = TRUE;

	//url里面如果带有单个的"%",打印日志就会崩溃,将该字符替换后输出
	CString url1 = m_strRequestURL;
	url1.Replace(L"%", L"%%");
	LOG_ERROR(_T("HTTP请求超时： %s"), url1.GetString());

	THttpResponseResult result;
	result.m_httpCode = 0;
	result.m_request = m_own;
	result.m_result = HTTP_RESULT_TIMEOUT;

	LeaveCriticalSection(&_CS);

	SwitchToMainThread(pcutil::bindAsyncFunc(&CHttpWebRequest::ProcessThreadSwitched, this, result));
}

CHttpWebRequest::~CHttpWebRequest()
{
	//如果this没有被析构 则会出现空指针访问
	CGlobalTimer::GetInstance()->DeleteTimer(this);
	if (m_downloadFile)
		delete(m_downloadFile);
}

void CHttpWebRequest::SetTimeout(const DWORD timeout)
{
	m_timeout = timeout;
}

DWORD CHttpWebRequest::GetTimeout()
{
	return m_timeout;
}

void CHttpWebRequest::InitBreakPoint(UINT64 start /*= 0 */)
{
	m_breakPoint = start;
}

BOOL CHttpWebRequest::Response(DWORD &dwResultSize)
{
	if (!m_breakPoint.IsUsable())
	{
		return CHttpClientSession::Response(dwResultSize);
	}
	else
	{
		QueryDataAvailable();
		return ReadBreakPointData();
	}
}


BOOL CHttpWebRequest::ReadBreakPointData()
{
	if (m_callback == NULL && m_downloadFile == NULL)
	{
		return FALSE;
	}
	int readSize = 0;
	int nRunLoop = 0;
	while (TRUE)
	{
		if (m_bClosed)
		{
			return FALSE;
		}
		std::string buffer;
		buffer.reserve(m_buffSize);
		readSize = CHttpClientSession::ReadResponse(buffer);
		if (readSize > 0)
		{
			int httpCode = this->GetResponseHeaderPtr()->GetStatusCode();
			if (httpCode < 200 || httpCode >= 300)
			{
				return FALSE;
			}

			if (m_callback)
			{
				CAsyncResult* asyncResult = new CAsyncResult(0);
				THttpResponseResult result;
				result.m_httpCode = 0;
				result.m_request = m_own;
				result.m_result = HTTP_RESULT_DOWNLOADING;

				asyncResult->callbackResult = result;
				asyncResult->callbackResult2 = buffer;
				//此处没有做线程切换，需要考虑做线程切换？
				EXECUTE_MULTI_CALL_BACK(m_callback, asyncResult);
			}
			else if (m_downloadFile) {
				m_downloadFile->Write(buffer.c_str(), buffer.size());
				if (m_progressCallback)
				{
					ULONGLONG size;
					m_downloadFile->GetSize(size);
					ULONGLONG contentLength = this->GetResponseHeaderPtr()->GetContentLength().GetValue() + m_breakPoint.GetValue();
					if (contentLength > size)
					{
						m_progressCallback(size, contentLength);
					}
				}
			}
		}
		else if (readSize == 0)
		{
			return TRUE;
		}
		else
		{
			//assert( 0 );
			return FALSE;
		}


		//下面的参数需要根据实际使用的效果再做调整
		//避免Cpu使用率高
	/*	if (((++nRunLoop) % 5))
			Sleep(50);*/
	}

}

DWORD CHttpWebRequest::TimeDiff(DWORD dwCur, DWORD dwStart)
{
	if (dwCur >= dwStart)
	{
		return dwCur - dwStart;
	}
	else
	{
		return INFINITE - dwStart + dwCur;
	}
	return 0;
}

void CHttpWebRequest::QueryDataAvailable()
{
	DWORD startTime = GetTickCount();
	BOOL queryFinish = FALSE;
	do
	{
		DWORD read = 0;
		queryFinish = ::InternetQueryDataAvailable(m_hHTTPRequest, &read, 0, 0);
		if (!queryFinish)
		{
			Sleep(100);
		}
		else if (TimeDiff(GetTickCount(), startTime) > 5 * 1000)
		{
			break;
		}

	} while (!m_bHttpClosed && !queryFinish);
}

void CHttpWebRequest::DoRequestLog()
{
	return;
#ifndef _DEBUG
	//static CString log = CLibUtils::GetLocalConfig(_T("loghttp"));
	//if (log != _T("1"))
	//{
	//	return;
	//}
#endif
	//url里面如果带有单个的"%",打印日志就会崩溃,将该字符替换后输出
	CString url1 = m_strRequestURL;
	url1.Replace(L"%", L"%%");
	std::string content;
	content = CStringHelper::ToString(url1);
	if (this->m_emRequestMethod == RequestMethod::RequestPostMethod)
	{
		content.append(" POST");
		//std::string postArgs;
		//this->GetPostArgumentsBufferForLog(postArgs);
		//content.append("\r\n");
		//content.append(postArgs.c_str(), postArgs.length());
	}
	else if (this->m_emRequestMethod == CHttpClientSession::RequestPostMethodMultiPartsFormData)
	{
		content.append("PostMulti");
		content.append("\r\n");
		//std::string cache;
		//AllocMultiPartsFormData(cache, m_strBoundaryName);
		//char buffer[10];
		//ZeroMemory(buffer, 10);
		//itoa(cache.size(), buffer, 10);
		//content.append(buffer);
	}
	else
	{
		content.append(" GET");
	}

	std::string postContent;
	GetPostArgumentsBuffer(postContent);
	content.append("\r\n");
	content.append(postContent.c_str(), postContent.length());

	content.append("\r\n\r\n");
	//HTTP_LOGGER(content);
}

void CHttpWebRequest::DoResponseLog()
{
	return;
#ifndef _DEBUG
	//static CString log = CLibUtils::GetLocalConfig(_T("loghttp"));
	//if (log != _T("1"))
	//{
	//	return;
	//}
#endif
	//url里面如果带有单个的"%",打印日志就会崩溃,将该字符替换后输出
	CString url1 = m_strRequestURL;
	url1.Replace(L"%", L"%%");

	std::string content;
	content.append("Response:");
	content.append(CStringHelper::ToString(url1) + "\r\n");
	content.append(CStringHelper::ToString(m_strResponseHeaders));
	content.append("\r\n");
	if (m_strResponseHeaders.Find(_T("Content-Type: image")) < 0)
	{
		content.append(m_bufResponseContent);
	}
	else
	{
		content.append("not log image type content!");
	}
	content.append("\r\n");
	//HTTP_LOGGER(content);
}

void CHttpWebRequest::AsyncDowndFile(CString downloadPath, IAsyncCallback* callback)
{
	m_downloadFile = new CAtlFile();
	HRESULT ret = m_downloadFile->Create(downloadPath, FILE_WRITE_DATA, FILE_SHARE_WRITE, OPEN_ALWAYS);

	if (!SUCCEEDED(ret))
	{
		EXECUTE_STATUS_CODE_CALL_BACK(callback, 1);
		return;
	}
	ULONGLONG size;
	m_downloadFile->GetSize(size);
	ret = m_downloadFile->Seek(0, FILE_END);

	m_downloadCallback = callback;
	InitBreakPoint(size);
	AsyncRequest(CreateAsyncBack(this, &CHttpWebRequest::ProcessDownloading));
}

void CHttpWebRequest::ProcessDownloading(CAsyncResult* result)
{
	THttpResponseResult& responseResult = any_cast<THttpResponseResult&>(result->callbackResult);

	if (!downloadInfo.get())
	{
		downloadInfo = new CDownloadInfo();
	}

	CAsyncResult* asyncResult = new CAsyncResult(0);
	asyncResult->callbackResult = downloadInfo;

	downloadInfo->status = responseResult.m_result;
	if (responseResult.m_result == HTTP_RESULT_SUCCEEDED)
	{
		m_downloadFile->Flush();
		m_downloadFile->Close();

		downloadInfo->totalSize = responseResult.m_request->GetResponseHeaderPtr()->GetContentLength();
		downloadInfo->downloadSize = downloadInfo->totalSize.GetValue();

		EXECUTE_CALL_BACK(m_downloadCallback, asyncResult);
	}
	else if (responseResult.m_result == HTTP_RESULT_DOWNLOADING)
	{
		string buffer = any_cast<string>(result->callbackResult2);
		m_downloadFile->Write(buffer.c_str(), buffer.size());

		downloadInfo->totalSize = responseResult.m_request->GetResponseHeaderPtr()->GetContentLength();
		
		downloadInfo->downloadSize += buffer.size();
		EXECUTE_MULTI_CALL_BACK(m_downloadCallback, asyncResult)
	}
	else
	{
		//修复下载失败文件被占用的问题
		m_downloadFile->Flush();
		m_downloadFile->Close();

		LOG_ERROR(_T("http下载失败:%d"), result->GetStatusCode());
		EXECUTE_CALL_BACK(m_downloadCallback, asyncResult);
	}
}

BOOL CHttpWebRequest::SyncDownloadFile(CString url, CString localPath, BOOL breakContinue, std::function<void(INT64 complete, INT64 totle)> progressCallback)
{
	CSharedPtr<CHttpWebRequest> request = CHttpWebRequest::Create(url, RequestGetMethod);
	request->m_progressCallback = progressCallback;
	return request->StartDownloadFile(localPath, breakContinue);
}

BOOL CHttpWebRequest::StartDownloadFile(CString localPath, BOOL breakContinue)
{
	m_downloadFile = new CAtlFile();

	CString fileName = ATLPath::FindFileName(localPath);
	CString dic = localPath.Left(localPath.GetLength() - fileName.GetLength());
	if (!::PathIsDirectory(dic))
	{
		::SHCreateDirectoryEx(NULL, dic, 0);
	}
	HRESULT ret = m_downloadFile->Create(localPath, FILE_WRITE_DATA, FILE_SHARE_WRITE, OPEN_ALWAYS);

	if (!SUCCEEDED(ret))
	{
		LOG_ERROR(_T("文件创建失败:%s result:%d error:%d"), localPath, ret, GetLastError());
		CFileHelper::Delete(localPath);
		return FALSE;
	}

	this->InitBreakPoint(0);
	if (breakContinue)
	{
		ULONGLONG size;
		if (m_downloadFile->GetSize(size) == S_OK)
		{
			m_downloadFile->Seek(size);
			this->InitBreakPoint(size);
		}
	}

	BOOL result = this->SyncStartRequest();
	m_downloadFile->Flush();
	m_downloadFile->Close();
	if ( !result && !breakContinue)
	{
		CFileHelper::Delete(localPath);
	}
	return result;
}

BOOL CHttpWebRequest::SyncRequest(CString url, string& response)
{
	CSharedPtr<CHttpWebRequest> request = CHttpWebRequest::Create(url);
	BOOL result = request->SyncStartRequest();
	if ( result)
	{
		response = request->GetResponseContent();
	}
	return  result;
}

BOOL CHttpWebRequest::SyncHttpRequest(CString url, string& response, CHttpClientSession::RequestMethod method /*= CHttpClientSession::RequestMethod::RequestGetMethod*/, string requestBody /*= ""*/, CString contentType /*= _T("application/oct-stream") */, DWORD* statusCode /*= NULL*/)
{
	CSharedPtr<CHttpWebRequest> request = CHttpWebRequest::Create(url, method);
	request->SetContentType(contentType);
	if (requestBody.length() > 0)
	{
		request->SetPostContent(requestBody);
	}
	BOOL result = request->SyncStartRequest();
	response = request->GetResponseContent();
	if (statusCode!= NULL && request->GetResponseHeaderPtr().get())
	{
		*statusCode = request->GetResponseHeaderPtr()->GetStatusCode();
	}
	return  result;
}

void CHttpWebRequest::GetPostArgumentsBuffer(std::string &cache)
{
	if (m_postContent.length()>0)
	{
		cache.assign(m_postContent);
	}
	else
	{
		__super::GetPostArgumentsBuffer(cache);
	}
}

void CHttpWebRequest::SetPostContent(std::string content)
{
	m_postContent = content;
}


void SetEnableLogHttp(bool enable)
{
	g_enable_http_log = enable;
}
