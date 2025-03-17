#pragma once

#define CUL_REQUEST_DEFAULT_TIMEOUT 30


#include <string>
#include <functional>
#include <atlstr.h>
#include <map>

//#include "./curl_sdk/include/curl/curl.h"

using namespace std;



class CCurlHelper 
{
public:
	static BOOL DownloadFile(CString url, CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback = NULL);

	static BOOL DownloadFile(CString url, CString localDictory, CString& filePath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback = NULL);

	static int HttpGet(CString url, string& response, long& statusCode, int timeout = CUL_REQUEST_DEFAULT_TIMEOUT);

	//// 禁用302的get请求,仅用于ASE报数
	//static int HttpGet(const CString& url, long& statusCode, int timeout = CUL_REQUEST_DEFAULT_TIMEOUT);

	//static int HttpPost(CString url, string& response, string requestBody, long& statusCode, string contenttype = "Content-Type:application/json; charset=utf-8", int timeout = CUL_REQUEST_DEFAULT_TIMEOUT);

	//static BOOL HttpUploadFiles(CString url, string& response, std::map<CString, CString>& args, CString file);

	/** my test function
	*/
	//static int TestPost(CString url, const std::string& strHeader, const std::string& strRequestBody, string& response, string requestBody, long& statusCode, string contenttype = "Content-Type:application/json; charset=utf-8", int timeout = CUL_REQUEST_DEFAULT_TIMEOUT);
	static int32_t Test(CString url);

	static int32_t TestDLFile(CString url, CString localPath);
};