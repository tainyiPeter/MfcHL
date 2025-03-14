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
	SYNC = 0,	// ��ͬ��ģʽ����֧���첽����ϵͳ���ıȽ�С
	ASYNC		// �첽ģʽ��֧���쳣����

};

/** Http ȫ�ֳ�ʼ�����ú���
*/
class EXTERN_CLASS HttpGlobalInit
{
public:

	/** ������������
	*/
	HttpGlobalInit();
	~HttpGlobalInit();

public:

	/** ��ʼ��
	*/
	void Initilize();

	/** ����ʼ��
	*/
	void UnInitilize();
};

class EXTERN_CLASS HttpLib : public SafeThreadBase<HttpLib, 1>
{
public:
	HttpLib(HTTP_LIB_MODE enumRunMode = HTTP_LIB_MODE::ASYNC);
	~HttpLib(void);

	// ��ͬ������
	HStringPtr Get(const char* lpUrl);
	HStringPtr Post(const char* lpUrl, const char* data, const char* content_type = NULL);

	// ���첽����
	// ��ͬ������
	void AsyncGet(const char* lpUrl, IHttpSupport* interface_cb);
	void AsyncPost(const char* lpUrl, const char* data, IHttpSupport* interface_cb, const char* content_type = NULL);

	// ͬ������
	ResponsePtr SyncRequset(RequestPtr pRequest, IHttpSupport* interface_cb = NULL);

	// �첽����
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
	// ��������
	static void SetProxy(curl_proxytype proxyType, const char* proxy, const char* szAuth = "");
	// ��ʱ����,��λ��  
	static void SetTimeout(long timeout);
protected:
	void SyncRequsetImpl(RequestPtr pRequest, ResponsePtr pResponse, IHttpSupport* interface_cb);
	void AsyncRequestImpl(RequestPtr pRequest, HTTP_REQUEST_CB http_request_cb, IHttpSupport* interface_cb);

	// �첽�����߳�֧��
	void StartAsyncThread();
	void StopAsyncThread();
	UINT AsyncRequestWorkThread();
	void Stop();

	// �첽ִ��֧�ֺ���
	int curl_multi_perform_ex();
	BOOL check_curl_msg_queue();

	// curl����׼��������
	CURL_OBJ* curl_object_prepare(RequestPtr pRequest, ResponsePtr pResponse);
	void curl_object_release(CURL_OBJ* pCurlObj);

	// ���������Ϣ�ռ�����
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