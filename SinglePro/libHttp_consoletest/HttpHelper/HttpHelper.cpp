#include <atlstr.h>
#include <string>  

#include "HttpHelper.h"
#include "HttpWebRequest.h"
#include "FileHelper.h"
#include "LibThread.h"

#pragma comment(lib, "Rpcrt4.lib")

BOOL CLibHttpHelper::HttpRequest(CString url, string& response, bool httpGet /*= true*/, string requestBody /*= ""*/, CString contentType /*= _T("application/oct-stream")*/, DWORD* statusCode /*= NULL*/)
{
	return CHttpWebRequest::SyncHttpRequest(url, response, httpGet ? CHttpClientSession::RequestMethod::RequestGetMethod : CHttpClientSession::RequestMethod::RequestPostMethod, requestBody, contentType, statusCode);


}


BOOL CLibHttpHelper::HttpUploadFiles(CString url, string& response, std::map<CString, CString>& args , CString file)
{
	if (!CFileHelper::IsFileExists(file))
	{
		return FALSE;
	}
	CSharedPtr<CHttpWebRequest> request = CHttpWebRequest::Create(url);

	for (auto arg : args)
	{
		request->AddPostArguments(arg.first, arg.second);
	}
	//·Ç±ØÑ¡;
	request->SetContentType(_T("multipart/form-data"));
	request->SetRequestMethod(CHttpClientSession::RequestPostMethodMultiPartsFormData);

	char* fileBuffer = NULL;
	UINT64 length = 0;
	if (CFileHelper::ReadAllBinary(file, fileBuffer, length)) {
		CString filename = CFileHelper::GetFileName(file);
		request->AddPostBinaryRefArgument(_T("file"), filename, fileBuffer, length, _T("text/plain"));
	}
	delete fileBuffer;

	BOOL result = request->SyncStartRequest();
	response = request->GetBinaryResponse();
	return result;
}

BOOL CLibHttpHelper::HttpPost(CString url, string& response, string requestBody)
{
	return CLibHttpHelper::HttpRequest(url, response, false, requestBody);
}

BOOL CLibHttpHelper::HttpPost(CString url, string& response, std::map<CString, CString>& args)
{
	CSharedPtr<CHttpWebRequest> request = CHttpWebRequest::Create(url, CHttpClientSession::RequestMethod::RequestPostMethod);
	request->SetContentType(_T("application/x-www-form-urlencoded"));
	for (auto arg : args)
	{
		request->AddPostArguments(arg.first, arg.second);
	}
	BOOL result = request->SyncStartRequest();
	response = request->GetBinaryResponse();
	
	return result;
}

BOOL CLibHttpHelper::HttpGet(CString url, string& response)
{
	return CLibHttpHelper::HttpRequest(url, response, true);
}

BOOL CLibHttpHelper::HttpRequest(CString url, string& response, bool httpGet, CString contentType, vector<CString>& headers, string& requestBody)
{
	CSharedPtr<CHttpWebRequest> request = CHttpWebRequest::Create(url, httpGet ? CHttpClientSession::RequestMethod::RequestGetMethod : CHttpClientSession::RequestMethod::RequestPostMethod);
	request->SetContentType(contentType);
	if (requestBody.length() > 0)
	{
		request->SetPostContent(requestBody);
	}
	for (auto& header : headers)
	{
		request->AddOneRequestHeader(header);
	}
	BOOL result = request->SyncStartRequest();
	response = request->GetBinaryResponse();
	return  result;
}

BOOL CLibHttpHelper::DownloadFile(CString url, CString localPath, BOOL breakContinue, std::function<void(INT64 complete, INT64 totle)> progressCallback)
{
	return CHttpWebRequest::SyncDownloadFile(url, localPath, breakContinue, progressCallback);
}

class CHttpDownloaderImpl :public CHttpDownloader {
public:
	CHttpDownloaderImpl():m_resultCallback(NULL)
	{

	}
private:
	virtual BOOL DownloadFile(CString url, CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle) > progressCallback = NULL) override
	{
		m_request = CHttpWebRequest::Create(url);
		m_request->m_progressCallback = progressCallback;
		BOOL result = m_request->StartDownloadFile(localPath, breakContinue);
		if (m_resultCallback)
		{
			m_resultCallback(result);
		}
		
		return result;
	}
	virtual int GetHttpResult() {
		if(m_request.get() && m_request->GetResponseHeaderPtr().get())
			return m_request->GetResponseHeaderPtr()->GetStatusCode();
		return 0;
	}
	virtual void AsyncDownloadFile(CString url, CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle) > progressCallback = NULL, std::function<void(BOOL result)> resultCallback = NULL) override
	{
		m_resultCallback = resultCallback;
		CLibThread::Start(pcutil::bindAsyncFunc(&CHttpDownloaderImpl::DownloadFile, this,url, localPath, breakContinue, progressCallback));
	}

	virtual void Close() override
	{
		if (m_request.get())
			m_request->Close();
	}

	virtual ~CHttpDownloaderImpl()
	{
		CHttpWebRequest* a  = m_request.get();
	}
private:
	CSharedPtr<CHttpWebRequest> m_request;
	std::function<void(BOOL result)> m_resultCallback;
};

pcutil::CSharedPtr<CHttpDownloader> CHttpDownloader::CreateDownloader()
{
	return new CHttpDownloaderImpl();
}
