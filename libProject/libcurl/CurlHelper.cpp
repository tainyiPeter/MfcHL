#include "stdafx.h"
#include "CurlHelper.h"
#include "CurlClient.h"



BOOL CCurlHelper::DownloadFile(CString url, CString localPath, BOOL breakContinue /*= FALSE*/, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback /*= NULL*/)
{
	CCurlClient client(url);
	return client.DownloadFile(localPath, breakContinue, progressCallback);
}

BOOL CCurlHelper::DownloadFile(CString url, CString localDictory, CString& filePath, BOOL breakContinue /*= FALSE*/, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback /*= NULL*/)
{
	CCurlClient client(url);
	return client.DownloadFile(localDictory, filePath, breakContinue, progressCallback);
}

int CCurlHelper::HttpGet(CString url, string& response, long& statusCode, int timeout)
{
	CCurlClient client(url);
	return client.HttpGet(response, statusCode, timeout);
}

int CCurlHelper::HttpGetTest()
{
	CCurlClient client("");
	return client.HttpGetTest();
}

int CCurlHelper::HttpGet(const CString& url, long& statusCode, int timeout)
{
	CCurlClient client(url);
	return client.HttpGet(statusCode, timeout);
}

int CCurlHelper::HttpPost(CString url, string& response, string requestBody, long& statusCode, string contenttype, int timeout)
{
	CCurlClient client(url);
	return client.HttpPost(response, statusCode, contenttype, requestBody, timeout);
}

BOOL CCurlHelper::HttpUploadFiles(CString url, string& response, std::map<CString, CString>& args, CString file)
{
	//ªπŒ¥ µœ÷
	return FALSE;
}

int32_t CCurlHelper::TestPost(CString url)
{
	CCurlClient client(url);
	return client.TestPost();
}

int32_t CCurlHelper::TestDLFile(CString url, CString localSaveFile)
{
	CCurlClient client(url);
	return client.TestDLFile(localSaveFile);
}