// HttpLib.cpp: 定义 DLL 的初始化例程。
//

#include "pch.h"
#include "framework.h"

#include "HttpLib\HttpLib.h"
#include "HttpLib\HttpRequest.h"
#include "HttpLib\std_macro.h"
#include "HttpLib\mem_macro.h"

//#include <LoggerManager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SET_CURL_OPT(opt, value)    curl_easy_setopt(curl_handle, opt, value)
#define GET_CURL_INFO(info, value)  curl_easy_getinfo(curl_handle, info, value);

#define CURL_PROGRESSFUNC_CONTINUE 0

static const DWORD THREAD_WAITTIME_INTERVAL = 50;

#define USE_WINDOW_FILE_FUNCTION

typedef struct CURL_OBJ
{
	CURL* curl_handle;
	CURLM* multi_handle;
	char err_buffer[CURL_ERROR_SIZE];
	curl_socket_t s;
	HTTP_REQUEST_CB http_request_cb;
	IHttpSupport* interface_cb;
	RequestPtr request;
	ResponsePtr response;

#ifdef USE_WINDOW_FILE_FUNCTION
	HANDLE file_handle;
#else
	FILE* file;
#endif

	CURL_OBJ()
	{
		curl_handle = NULL;
		multi_handle = NULL;
		SIMPLE_ZEROMEMORY(err_buffer);
		s = NULL;
		http_request_cb = NULL;
		interface_cb = NULL;

#ifdef USE_WINDOW_FILE_FUNCTION
		file_handle = INVALID_HANDLE_VALUE;
#else
		file = NULL;
#endif
	}
} CURL_OBJ;

/*
 * Write data callback function (called within the context of
 * curl_easy_perform.
 */
static size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp)
{
	int segsize = size * nmemb;

	CURL_OBJ* pCurlObj = (CURL_OBJ*)userp;

	if (pCurlObj == NULL)
	{
		return 0;
	}

	if (pCurlObj->request->OpType() == HTTP_OP_TYPE::DATA_REQUEST)
	{
		pCurlObj->response->AppendData((const char*)buffer, segsize);
	}
	else
	{
#ifdef USE_WINDOW_FILE_FUNCTION
		ASSERT(pCurlObj->file_handle != INVALID_HANDLE_VALUE);

		if (pCurlObj->file_handle != INVALID_HANDLE_VALUE)
		{
			DWORD dwBytesWritten = 0;
			::WriteFile(pCurlObj->file_handle, buffer, segsize, &dwBytesWritten, NULL);
		}
#else
		ASSERT(pCurlObj->file != NULL);

		if (pCurlObj->file)
		{
			size_t nWrite = fwrite(buffer, size, nmemb, pCurlObj->file);
		}
#endif
	}
	return segsize;
}

static size_t write_header(void* buffer, size_t size, size_t nmemb, void* userp)
{
	int segsize = size * nmemb;

	CURL_OBJ* pCurlObj = (CURL_OBJ*)userp;

	if (pCurlObj == NULL)
	{
		return 0;
	}

	pCurlObj->response->AppendHeader((const char*)buffer, segsize);

	return segsize;
}

static int progress_callback(void* userp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CURL_OBJ* pCurlObj = (CURL_OBJ*)userp;

	if (pCurlObj == NULL)
	{
		return CURL_PROGRESSFUNC_CONTINUE;
	}

	if (dltotal > -0.1 && dltotal < 0.1)
	{
		return CURL_PROGRESSFUNC_CONTINUE;
	}

	if (pCurlObj->interface_cb)
	{
		pCurlObj->interface_cb->RequestProgress(pCurlObj->request, dltotal, dlnow);
	}

	return CURL_PROGRESSFUNC_CONTINUE;
}

HttpLib::HttpLib(HTTP_LIB_MODE enumRunMode /*= HTTP_LIB_MODE::ASYNC*/) : m_enumRunMode(enumRunMode)
{
	m_curl_handle = curl_easy_init();

	if (m_enumRunMode == HTTP_LIB_MODE::ASYNC)
	{
		m_multi_handle = curl_multi_init();
		m_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hRequest = CreateEvent(NULL, FALSE, FALSE, NULL);

		StartAsyncThread();
	}
}

HttpLib::~HttpLib(void)
{
	curl_easy_cleanup(m_curl_handle);

	if (m_enumRunMode == HTTP_LIB_MODE::ASYNC)
	{
		StopAsyncThread();

		CLOSE_HANDLE(m_hExit);
		CLOSE_HANDLE(m_hRequest);

		curl_multi_cleanup(m_multi_handle);
	}
}

void HttpLib::StartAsyncThread()
{
	SafeStartThread(&HttpLib::AsyncRequestWorkThread);
}

void HttpLib::StopAsyncThread()
{
	if (m_hExit == NULL)
	{
		return;
	}

	SetEvent(m_hExit);

#ifdef _DEBUG
	JoinAllThread();
#else
	JoinAllThread(1000 * 30, TRUE);
#endif
}

int HttpLib::curl_multi_perform_ex()
{
	int running_handle_count = 0;

	while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(m_multi_handle, &running_handle_count))
	{
		if (running_handle_count == 0)
		{
			break;
		}
	}

	return running_handle_count;
}

UINT HttpLib::AsyncRequestWorkThread()
{
	//WARN_LOG("httplib 异步线程启动");

	HANDLE wait_handle_array[] = { m_hExit, m_hRequest };

	BOOL bContinueRun = TRUE;

	while (bContinueRun)
	{
		DWORD dwWaitRet = WaitForMultipleObjects(
			_countof(wait_handle_array),
			wait_handle_array,
			FALSE,
			INFINITE
		);

		if (dwWaitRet == 0)
		{
			bContinueRun = FALSE;
			break;
		}

		while (curl_multi_perform_ex() > 0)
		{
			CURLMcode wait_code = curl_multi_wait(m_multi_handle, NULL, 0, INFINITE, NULL);

			if (wait_code != CURLM_OK)
			{
				//ERROR_LOG(_T("curl_multi_wait 返回值 [%d] 异常"), wait_code);
				break;
			}

			check_curl_msg_queue();
		}

		check_curl_msg_queue();
	}

	//WARN_LOG("httplib 异步线程退出");

	return 0;
}

BOOL HttpLib::check_curl_msg_queue()
{
	int msgs_left = 0;
	CURLMsg* msg = NULL;

	while (msg = curl_multi_info_read(m_multi_handle, &msgs_left))
	{
		CURL* curl_handle = msg->easy_handle;

		if (msg->msg == CURLMSG_DONE)
		{
			handle_request_finished_event(curl_handle, msg->data.result);
		}
		else
		{
			//ERROR_LOG(_T("httplib curl_multi_info_read 状态异常，返回值 CURLMsg = [%s]"), msg->msg);
		}
	}

	return TRUE;
}

BOOL HttpLib::handle_request_finished_event(CURL* curl_handle, CURLcode perform_code)
{
	CURL_OBJ* pCurlObj = NULL;
	int http_status_code = 0;
	curl_off_t total_t;

	GET_CURL_INFO(CURLINFO_PRIVATE, &pCurlObj);
	GET_CURL_INFO(CURLINFO_RESPONSE_CODE, &http_status_code);

	GET_CURL_INFO(CURLINFO_TOTAL_TIME_T, &total_t);
	ASSERT(pCurlObj != NULL);

	RequestPtr pRequest = pCurlObj->request;
	ResponsePtr pResponse = pCurlObj->response;

	pResponse->SetCurlCode(perform_code);
	pResponse->SetTotalTime(total_t);
	pResponse->SetHttpStatusCode(http_status_code);

	if (perform_code != CURLE_OK)
	{
		//ERROR_LOG("http请求失败\n请求地址 [%s]\n错误代码 [%d],原因 [%s]", pResponse->RequestString(), pResponse->CurlCode(), pCurlObj->err_buffer);
	}

	if (pRequest->Debug())
	{
		pResponse->DebugPrintSummary();
		//pResponse->DebugPrintHeader();
	}

	if (pCurlObj->http_request_cb != NULL)
	{
		pCurlObj->http_request_cb(pRequest, pResponse);
	}

	if (pCurlObj->interface_cb != NULL)
	{
		pCurlObj->interface_cb->RequestComplete(pRequest, pResponse);
	}

	curl_object_release(pCurlObj);

	return TRUE;
}

std::string HttpLib::s_strProxy;

long HttpLib::s_lTimeout = 0;

void HttpLib::SyncRequsetImpl(RequestPtr pRequest, ResponsePtr pResponse, IHttpSupport* interface_cb /*=NULL*/)
{
	ASSERT(pRequest != NULL && pResponse != NULL);

	CURL_OBJ* pCurlObj = curl_object_prepare(pRequest, pResponse);

	pCurlObj->interface_cb = interface_cb;

	CURLcode perform_code = curl_easy_perform(pCurlObj->curl_handle);

	handle_request_finished_event(pCurlObj->curl_handle, perform_code);
}

void HttpLib::AsyncRequestImpl(RequestPtr pRequest, HTTP_REQUEST_CB http_request_cb, IHttpSupport* interface_cb)
{
	if (m_enumRunMode == HTTP_LIB_MODE::SYNC)
	{
		ASSERT(FALSE);
		return;
	}

	ResponsePtr pResponse = HttpResponse::Create();

	CURL_OBJ* pCurlObj = curl_object_prepare(pRequest, pResponse);

	pCurlObj->multi_handle = m_multi_handle;
	pCurlObj->http_request_cb = http_request_cb;
	pCurlObj->interface_cb = interface_cb;

	curl_multi_add_handle(m_multi_handle, pCurlObj->curl_handle);
	SetEvent(m_hRequest);
}

ResponsePtr HttpLib::SyncRequset(RequestPtr pRequest, IHttpSupport* interface_cb /*=NULL*/)
{
	ResponsePtr pResponse = HttpResponse::Create();

	SyncRequsetImpl(pRequest, pResponse, interface_cb);

	return pResponse;
}

void HttpLib::AsyncRequest(RequestPtr pRequest, HTTP_REQUEST_CB http_request_cb)
{
	AsyncRequestImpl(pRequest, http_request_cb, NULL);
}

void HttpLib::AsyncRequest(RequestPtr pRequest, IHttpSupport* interface_cb)
{
	AsyncRequestImpl(pRequest, NULL, interface_cb);
}

BOOL HttpLib::SyncDownloadFile(const char* file_url, const char* save_as, IHttpSupport* interface_cb /*= NULL*/)
{
	RequestPtr pRequest = HttpRequest::Create();

	pRequest->SetUrl(file_url);
	pRequest->SetMethod(HTTP_METHOD::GET);
	pRequest->SetOpType(HTTP_OP_TYPE::FILE_DOWNLOAD);
	pRequest->SaveAs(save_as);

	ResponsePtr pResponse = SyncRequset(pRequest, interface_cb);

	return pResponse->Succeed();
}

void HttpLib::AsyncDownLoadFile(const char* file_url, const char* save_as, HTTP_REQUEST_CB http_request_cb)
{
	RequestPtr pRequest = HttpRequest::Create();

	pRequest->SetUrl(file_url);
	pRequest->SetMethod(HTTP_METHOD::GET);
	pRequest->SetOpType(HTTP_OP_TYPE::FILE_DOWNLOAD);
	pRequest->SaveAs(save_as);

	AsyncRequest(pRequest, http_request_cb);
}

void HttpLib::AsyncDownLoadFile(const char* file_url, const char* save_as, IHttpSupport* interface_cb)
{
	RequestPtr pRequest = HttpRequest::Create();

	pRequest->SetUrl(file_url);
	pRequest->SetMethod(HTTP_METHOD::GET);
	pRequest->SetOpType(HTTP_OP_TYPE::FILE_DOWNLOAD);
	pRequest->SaveAs(save_as);

	AsyncRequest(pRequest, interface_cb);
}

HStringPtr HttpLib::UrlEncode(const char* url)
{
	return UrlEncode(url, strlen(url));
}

HStringPtr HttpLib::UrlEncode(const char* url, size_t nLen)
{
	ASSERT(m_curl_handle != NULL);

	char* url_encode = curl_easy_escape(m_curl_handle, url, (int)nLen);

	HStringPtr urlString(new StaticString(url_encode, SCP_ANSI));

	curl_free(url_encode);

	return urlString;
}

HStringPtr HttpLib::UrlDecode(const char* url)
{
	return UrlDecode(url, strlen(url));
}

HStringPtr HttpLib::UrlDecode(const char* url, size_t nLen)
{
	ASSERT(m_curl_handle != NULL);

	int out_len = 0;

	char* url_decode = curl_easy_unescape(m_curl_handle, url, (int)nLen, &out_len);

	HStringPtr urlString(new StaticString(url_decode, SCP_ANSI));

	curl_free(url_decode);

	return urlString;
}

void HttpLib::SetProxy(const char* proxy)
{
	s_strProxy = proxy;
}

void HttpLib::SetTimeout(long timeout)
{
	s_lTimeout = timeout;
}

HStringPtr HttpLib::Get(const char* lpUrl)
{
	RequestPtr pRequest = HttpRequest::Create(lpUrl, HTTP_METHOD::GET);

	ResponsePtr pResponse = SyncRequset(pRequest);

	return pResponse->ResponseString();
}

HStringPtr HttpLib::Post(const char* lpUrl, const char* data, const char* content_type /*= NULL*/)
{
	RequestPtr pRequest = HttpRequest::Create(lpUrl, data, content_type);

	ResponsePtr pResponse = SyncRequset(pRequest);

	return pResponse->ResponseString();
}

void HttpLib::AsyncGet(const char* lpUrl, IHttpSupport* interface_cb)
{
	RequestPtr pRequest = HttpRequest::Create(lpUrl, HTTP_METHOD::GET);
	AsyncRequest(pRequest, interface_cb);
}

void HttpLib::AsyncPost(const char* lpUrl, const char* data, IHttpSupport* interface_cb, const char* content_type /*= NULL*/)
{
	RequestPtr pRequest = HttpRequest::Create(lpUrl, data, content_type);
	AsyncRequest(pRequest, interface_cb);
}

void HttpLib::curl_object_release(CURL_OBJ* pCurlObj)
{
	if (pCurlObj->request->OpType() == HTTP_OP_TYPE::FILE_DOWNLOAD)
	{
		// TRACE_FUNC_TIME("fclose");

#ifdef USE_WINDOW_FILE_FUNCTION
		ASSERT(pCurlObj->file_handle != INVALID_HANDLE_VALUE);
		CLOSE_FILE_HANDLE(pCurlObj->file_handle);
#else
		ASSERT(pCurlObj->file != NULL);
		fclose(pCurlObj->file);
		pCurlObj->file = NULL;
#endif
	}

	if (pCurlObj->multi_handle != NULL)
	{
		CURLMcode mcode = curl_multi_remove_handle(pCurlObj->multi_handle, pCurlObj->curl_handle);
		ASSERT(mcode == CURLM_OK);
	}

	curl_easy_cleanup(pCurlObj->curl_handle);

	DELETE_PTR(pCurlObj);
}

CURL_OBJ* HttpLib::curl_object_prepare(RequestPtr pRequest, ResponsePtr pResponse)
{
	CURL* curl_handle = curl_easy_init();
	ASSERT(curl_handle != NULL);

	CURL_OBJ* pCurlObj = new CURL_OBJ;

	pCurlObj->curl_handle = curl_handle;
	pCurlObj->request = pRequest;
	pCurlObj->response = pResponse;

	if (pRequest->OpType() == HTTP_OP_TYPE::FILE_DOWNLOAD)
	{
#ifdef USE_WINDOW_FILE_FUNCTION
		pCurlObj->file_handle = ::CreateFileA(
			pRequest->LocalSaveFileName().c_str(),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		ASSERT(pCurlObj->file_handle != INVALID_HANDLE_VALUE);
#else
		fopen_s(&pCurlObj->file, pRequest->LocalSaveFileName().c_str(), "wb");
		ASSERT(pCurlObj->file != NULL);
#endif
	}

	pResponse->SetRequestString(pRequest->RequestString(curl_handle).c_str());

	// Url
	SET_CURL_OPT(CURLOPT_URL, (LPCSTR)pResponse->RequestString());

	// Error Message
	SET_CURL_OPT(CURLOPT_ERRORBUFFER, pCurlObj->err_buffer);

	// body recv function
	SET_CURL_OPT(CURLOPT_WRITEFUNCTION, write_data);
	SET_CURL_OPT(CURLOPT_WRITEDATA, (void*)pCurlObj);

	// header recv function
	SET_CURL_OPT(CURLOPT_HEADERFUNCTION, write_header);
	SET_CURL_OPT(CURLOPT_HEADERDATA, (void*)pCurlObj);

	// progress callback
	SET_CURL_OPT(CURLOPT_NOPROGRESS, 0);
	SET_CURL_OPT(CURLOPT_PROGRESSFUNCTION, progress_callback);
	SET_CURL_OPT(CURLOPT_PROGRESSDATA, (void*)pCurlObj);

	// ssl, not check
	if (pRequest->SSL())
	{
		SET_CURL_OPT(CURLOPT_SSL_VERIFYPEER, 0);
		SET_CURL_OPT(CURLOPT_SSL_VERIFYHOST, 0);
	}

	// post 
	if (pRequest->Method() == HTTP_METHOD::POST)
	{
		SET_CURL_OPT(CURLOPT_POST, 1L);
		SET_CURL_OPT(CURLOPT_POSTFIELDS, pRequest->PostData());
		SET_CURL_OPT(CURLOPT_POSTFIELDSIZE, pRequest->PostDataLen());
	}

	// http header
	SET_CURL_OPT(CURLOPT_HTTPHEADER, pRequest->HeaderList());

	// Referer
	if (!pRequest->Referer().empty())
	{
		SET_CURL_OPT(CURLOPT_REFERER, pRequest->Referer().c_str());
	}

	// timeout
	if (s_lTimeout > 0)
	{
		SET_CURL_OPT(CURLOPT_TIMEOUT, s_lTimeout);
	}

	// 支持压缩
	SET_CURL_OPT(CURLOPT_ACCEPT_ENCODING, "gzip");

	// 支持重定向
	if (pRequest->Redirect())
	{
		SET_CURL_OPT(CURLOPT_FOLLOWLOCATION, 1L);
	}

	// proxy
	if (!s_strProxy.empty())
	{
		SET_CURL_OPT(CURLOPT_PROXY, s_strProxy.c_str());
		//SET_CURL_OPT(CURLOPT_PROXY_SSLVERSION, CURL_SSLVERSION_DEFAULT);
	}

	// 建立关联
	SET_CURL_OPT(CURLOPT_PRIVATE, pCurlObj);

	return pCurlObj;
}


