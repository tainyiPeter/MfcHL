#pragma once
#include <Windows.h>
#include <wininet.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlfile.h>
#include <functional>
#include <vector>
#include <string>
#include "WebRequest.h"
#include "UsableValue.h"
#include "shared_ptr.h"

#include "EventHandler.h"

using namespace std;
using namespace pcutil;

enum EHTTP_RESULT
{
	HTTP_RESULT_NONE,
	HTTP_RESULT_SUCCEEDED,
	HTTP_RESULT_TIMEOUT,
	HTTP_RESULT_FAILED,
	HTTP_RESULT_ERRORED,
	HTTP_RESULT_DOWNLOADING
};

class CHttpWebRequest;
struct THttpResponseResult
{
	EHTTP_RESULT m_result = HTTP_RESULT_NONE;
	int m_httpCode;
	CSharedPtr<CHttpWebRequest> m_request;
	~THttpResponseResult()
	{
	}
};

class CDownloadInfo
{
public:
	EHTTP_RESULT status = HTTP_RESULT_NONE;
	unsigned __int64 downloadSize;
	CUsableValue<unsigned __int64>  totalSize;
protected:
private:
};

class WEAVERLIB_API_LIB CHttpWebRequest
	:public pcutil::CWebRequest,public CEventObserver
{
public:
	virtual ~CHttpWebRequest();
	//支持大文件下载，需要在业务线程执行
	static BOOL SyncDownloadFile(CString url, CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle)> progressCallback = NULL);
	static BOOL SyncRequest(CString url, string& response);
	static BOOL SyncHttpRequest(CString url,string& response, CHttpClientSession::RequestMethod method = CHttpClientSession::RequestMethod::RequestGetMethod, string requestBody = "", CString contentType = _T("application/oct-stream") , DWORD* statusCode = NULL);
	//通过静态方法来创建实例,参考.net的实现模式（今后可以考虑实现工厂类）
	static CSharedPtr<CHttpWebRequest> Create( CString url, CHttpClientSession::RequestMethod method = CHttpClientSession::RequestMethod::RequestGetMethod);
	void AsyncRequest( IAsyncCallback* callback );
	//此方法只支持小文件上传
	void AsyncUploadFile( CString filePath, IAsyncCallback* callback, CString contentType =_T("image/jpeg") );
	//下载文件，支持大文件下载
	void AsyncDowndFile( CString downloadPath, IAsyncCallback* callback );
	//设置断点续传的起始点，如果为0 表示从头，如果需要断点续传 则必须调用此函数初始化
	void InitBreakPoint( UINT64 start = 0 );
	void SetTimeout( const DWORD timeout );
	DWORD GetTimeout();
	void SetResponseCategory( EWebResponseCategory category );
	void SetRequestMethod( CHttpClientSession::RequestMethod method );

	void SetPostContent(std::string content);
	BOOL SyncStartRequest();

	BOOL StartDownloadFile(CString localPath, BOOL breakContinue);
protected:
	void ProcessThreadSwitched( THttpResponseResult& result );
	void ProcessTimeOut();
	CHttpWebRequest( CString url );
	BOOL Response( DWORD &dwResultSize );
	void QueryDataAvailable();
	BOOL ReadBreakPointData();
	DWORD TimeDiff(DWORD dwCur, DWORD dwStart);
	void DoRequestLog();
	void DoResponseLog();
	BOOL DoSyncStartRequest();
private:
	//精度为秒
	DWORD m_timeout;
	//参考.net的Nullable，如果没有初始化 则认为是默认下载
	pcutil::CUsableValue<INT64> m_breakPoint;
	//自己持有自己的指针，因为是异步请求，封装在内部做析构，保证在请求的过程中不会被析构
	//更好的实现方式应该用工厂方式实现，由工厂持有指针
	CSharedPtr<CHttpWebRequest> m_own;
	IAsyncCallback* m_callback;

	//添加下面两个标记，用于超时逻辑，保证线程安全
	BOOL m_hasTimeOut;
	BOOL m_finished;
	string m_postContent;

public:
	//同步调用 回调函数
	std::function<void(INT64 complete, INT64 totle)> m_progressCallback;
private:
	//断点续传文件下载
	void ProcessDownloading( CAsyncResult* result );

	virtual void GetPostArgumentsBuffer(std::string &cache) override;

	IAsyncCallback* m_downloadCallback;
	CSharedPtr<CDownloadInfo> downloadInfo;
	CAtlFile* m_downloadFile;
	CRITICAL_SECTION	_CS;
};

void SetEnableLogHttp(bool enable);

