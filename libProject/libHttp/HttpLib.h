#pragma once

//#include "curl/curl.h"
#include "PrepareDef.h"
#include "HttpRequest.h"
#include "Utils\safethread.h"

#include <string>
#include <memory>
#include <mutex>
#include <vector>

typedef void (*HTTP_REQUEST_CB)(RequestPtr pRequest, ResponsePtr pResponse);

class IHttpSupport
{
public:
	virtual void RequestComplete(RequestPtr pRequest, ResponsePtr pResponse) = 0;
	virtual void RequestProgress(RequestPtr pRequest, double dbTotal, double dbNow) = 0;
};

struct CURL_OBJ;

enum class HTTP_LIB_MODE
{
	SYNC = 0,	// 简单同步模式，不支持异步请求，系统消耗比较小
	ASYNC		// 异步模式，支持异常请求

};

/** Http 全局初始化调用函数
*/
class EXTERN_CLASS HttpGlobalInit
{
public:

	/** 构造析构函数
	*/
	HttpGlobalInit();
	~HttpGlobalInit();

public:

	/** 初始化
	*/
	void Initilize();

	/** 反初始化
	*/
	void UnInitilize();
};

class EXTERN_CLASS HttpLib : public SafeThreadBase<HttpLib, 1>
{
public:
	HttpLib(HTTP_LIB_MODE enumRunMode = HTTP_LIB_MODE::ASYNC);
	~HttpLib(void);

	// 简单同步请求
	HStringPtr Get(const char* lpUrl);
	HStringPtr Post(const char* lpUrl, const char* data, const char* content_type = NULL);

	// 简单异步请求
	// 简单同步请求
	void AsyncGet(const char* lpUrl, IHttpSupport* interface_cb);
	void AsyncPost(const char* lpUrl, const char* data, IHttpSupport* interface_cb, const char* content_type = NULL);

	// 同步请求
	ResponsePtr SyncRequset(RequestPtr pRequest, IHttpSupport* interface_cb = NULL);

	// 异步请求
	void AsyncRequest(RequestPtr pRequest, HTTP_REQUEST_CB http_request_cb);
	void AsyncRequest(RequestPtr pRequest, IHttpSupport* interface_cb);

	BOOL SyncDownloadFile(const char* file_url, const std::wstring::value_type* save_as, IHttpSupport* interface_cb = NULL);
	void AsyncDownLoadFile(const char* file_url, const std::wstring::value_type* save_as, HTTP_REQUEST_CB http_request_cb);
	void AsyncDownLoadFile(const char* file_url, const std::wstring::value_type* save_as, IHttpSupport* interface_cb);

	// UrlEncode
	HStringPtr UrlEncode(const char* url);
	std::string UrlEncode(const std::string& url);
	HStringPtr UrlEncode(const char* url, size_t nLen);

	// UrlDecode
	HStringPtr UrlDecode(const char* url);
	HStringPtr UrlDecode(const char* url, size_t nLen);
public:
	// 代理设置
	static void SetProxy(curl_proxytype proxyType, const char* proxy, const char* szAuth = "");
	// 超时设置,单位秒  
	static void SetTimeout(long timeout);
protected:
	void SyncRequsetImpl(RequestPtr pRequest, ResponsePtr pResponse, IHttpSupport* interface_cb);
	void AsyncRequestImpl(RequestPtr pRequest, HTTP_REQUEST_CB http_request_cb, IHttpSupport* interface_cb);

	// 异步请求线程支持
	void StartAsyncThread();
	void StopAsyncThread();
	UINT AsyncRequestWorkThread();
	void Stop();

	// 异步执行支持函数
	int curl_multi_perform_ex();
	BOOL check_curl_msg_queue();

	// curl对象准备与销毁
	CURL_OBJ* curl_object_prepare(RequestPtr pRequest, ResponsePtr pResponse);
	void curl_object_release(CURL_OBJ* pCurlObj);

	// 请求完成信息收集函数
	BOOL handle_request_finished_event(CURL* curl_handle, CURLcode perform_code);
private:
	HTTP_LIB_MODE m_enumRunMode;
	CURL* m_curl_handle;

	CURLM* m_multi_handle;
	HANDLE m_hExit;
	HANDLE m_hRequest;

	std::recursive_mutex m_dataMutex;
	std::vector<CURL_OBJ*> m_vecCurlObj;

	static std::string s_strProxy;
	static std::string s_strProxyAuth;
	static curl_proxytype s_proxyType;

	static long s_lTimeout;
};