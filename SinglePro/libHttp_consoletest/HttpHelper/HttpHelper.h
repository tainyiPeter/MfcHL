#pragma once
#include <atlstr.h>
#include <string>  
#include <vector>
#include <map>
#include <functional>
#include "shared_ptr.h"

//using namespace pcutil;
using namespace std;
class CLibHttpHelper {
public:
	static BOOL HttpPost(CString url, string& response,std::map<CString,CString>& args);
	static BOOL HttpPost(CString url, string& response, string requestBody);
	static BOOL HttpGet(CString url, string& response);
	static BOOL HttpRequest(CString url, string& response, bool httpGet, string requestBody = "", CString contentType = _T("application/oct-stream"), DWORD* statusCode = NULL);

	static BOOL HttpUploadFiles(CString url,string& response, std::map<CString, CString>& args,CString file);

	static BOOL HttpRequest(CString url, string& response, bool httpGet, CString contentType, std::vector<CString>& headers, string& requestBody);

	/*breakContinue：如果本地文件存在的情况下 是否断点续传 调用方需要保证文件的唯一性
	sampleCode:
		int index = 0;
	CLibHttpHelper::DownloadFile(_T("https://guanjia.lenovo.com.cn/download/lenovopcmanager_apps.exe"), _T("c:\\test.exe"), TRUE,
		[&](INT64 complete,INT64 totle)
	{index++;});
	*/
	static BOOL DownloadFile(CString url, CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete,INT64 totle)> progressCallback = NULL);
};

class CHttpDownloader {
public:
	static pcutil::CSharedPtr<CHttpDownloader> CreateDownloader();
	virtual BOOL DownloadFile(CString url, CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle)> progressCallback = NULL) = 0;
	virtual void AsyncDownloadFile(CString url, CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle)> progressCallback = NULL, std::function<void(BOOL result)> resultCallback = NULL) = 0;
	virtual void Close() = 0;
	virtual ~CHttpDownloader() {};
	virtual int GetHttpResult() = 0;
};