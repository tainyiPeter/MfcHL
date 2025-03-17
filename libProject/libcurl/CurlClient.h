#pragma once

#include <string>
#include <functional>
#include <atlstr.h>
#include <curl/curl.h>

using namespace std;

class CCurlClient 
{
public:
	CCurlClient(CString url);

	~CCurlClient();

	int HttpGet(string& response, long& statusCode, int timeout);

	int HttpGet(long& statusCode, int timeout);

	int HttpPost(string& response, string requestBody, long& statusCode, string contenttype, int timeout);

	BOOL DownloadFile(CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback = NULL);

	int32_t TestDLFile(CString localPath);

	//localpath是指定下载的目录， filePath 返回下载的文件路径，文件名根据http的协议来定义
	//如果没有计算出文件名，则随机使用一个exe的文件名 （待商榷）
	BOOL DownloadFile(CString localDictory, CString& filePath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback = NULL);

	BOOL InitDownloadPath();

	BOOL CancelDownload();

	/** only test
	*/
	int32_t TestPost();

private:
	long GetDownloadFileLength();
	CString GetDownloadFileName();
	CString GetFileNameByUrl(string url);
	UINT64 GetFileSize(const CString& FilePath);
public:
	BOOL m_canceled = FALSE;
	std::string m_url;
	std::function<void(INT64 complete, INT64 totle, double speed)> m_processCallback;
	int m_connectTimeout;

	void* m_curHandle;
	CString m_downloadDictory;
	CString m_downloadFilePath;

	//重定向的URL地址,从httpheader 读取
	string m_lastRedirectUrl;
	//header content-disposition: 定义的文件名
	string m_dispositionFileName;
};

struct Curl_Progress_User_Data
{
	CCurlClient* client;
	CURL* handle;
	curl_off_t breakPoint = 0;
	curl_off_t length = 0;
	time_t lasttime = 0;
	int errorCode = 0;

	curl_off_t lastdl = 0;
	time_t lastdltime = 0;
};


#define HTTP_ERROR_UNKNOW			99
#define HTTP_ERROR_FILELENGTH		100		//100 获取不到长度
#define HTTP_ERROR_OPENFILE			101		//文件无法打开
#define HTTP_ERROR_CURINIT_FAILED	102		//102 curl 初始化失败
#define HTTP_ERROR_CANELED			103		//取消下载
#define HTTP_ERROR_SETTINGS			104		//获取文件大小的时候 设置失败
#define HTTP_ERROR_GET_LENGTH		105		//获取文件大小的时候 执行失败
#define HTTP_ERROR_GET_INFO_FAILED	106		//curl_easy_getinfo 获取大小失败
#define HTTP_ERROR_CURSET_FAILED	107		//107 设置失败
#define HTTP_ERROR_DWONLOAD_ERROR	108
#define HTTP_ERROR_FILENAME_NONE	109

#define CURL_CANCEL_NODATA			9998	//兼容腾讯续传失败的情况 5秒尝试
#define CURL_CANCEL_NET_NODATA		9999	//兼容下载过程中，网络终端 10秒尝试
