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
	//֧�ִ��ļ����أ���Ҫ��ҵ���߳�ִ��
	static BOOL SyncDownloadFile(CString url, CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle)> progressCallback = NULL);
	static BOOL SyncRequest(CString url, string& response);
	static BOOL SyncHttpRequest(CString url,string& response, CHttpClientSession::RequestMethod method = CHttpClientSession::RequestMethod::RequestGetMethod, string requestBody = "", CString contentType = _T("application/oct-stream") , DWORD* statusCode = NULL);
	//ͨ����̬����������ʵ��,�ο�.net��ʵ��ģʽ�������Կ���ʵ�ֹ����ࣩ
	static CSharedPtr<CHttpWebRequest> Create( CString url, CHttpClientSession::RequestMethod method = CHttpClientSession::RequestMethod::RequestGetMethod);
	void AsyncRequest( IAsyncCallback* callback );
	//�˷���ֻ֧��С�ļ��ϴ�
	void AsyncUploadFile( CString filePath, IAsyncCallback* callback, CString contentType =_T("image/jpeg") );
	//�����ļ���֧�ִ��ļ�����
	void AsyncDowndFile( CString downloadPath, IAsyncCallback* callback );
	//���öϵ���������ʼ�㣬���Ϊ0 ��ʾ��ͷ�������Ҫ�ϵ����� �������ô˺�����ʼ��
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
	//����Ϊ��
	DWORD m_timeout;
	//�ο�.net��Nullable�����û�г�ʼ�� ����Ϊ��Ĭ������
	pcutil::CUsableValue<INT64> m_breakPoint;
	//�Լ������Լ���ָ�룬��Ϊ���첽���󣬷�װ���ڲ�����������֤������Ĺ����в��ᱻ����
	//���õ�ʵ�ַ�ʽӦ���ù�����ʽʵ�֣��ɹ�������ָ��
	CSharedPtr<CHttpWebRequest> m_own;
	IAsyncCallback* m_callback;

	//�������������ǣ����ڳ�ʱ�߼�����֤�̰߳�ȫ
	BOOL m_hasTimeOut;
	BOOL m_finished;
	string m_postContent;

public:
	//ͬ������ �ص�����
	std::function<void(INT64 complete, INT64 totle)> m_progressCallback;
private:
	//�ϵ������ļ�����
	void ProcessDownloading( CAsyncResult* result );

	virtual void GetPostArgumentsBuffer(std::string &cache) override;

	IAsyncCallback* m_downloadCallback;
	CSharedPtr<CDownloadInfo> downloadInfo;
	CAtlFile* m_downloadFile;
	CRITICAL_SECTION	_CS;
};

void SetEnableLogHttp(bool enable);

