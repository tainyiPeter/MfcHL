#include "stdafx.h"
#include "CurlClient.h"
#include <io.h>
#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <atlconv.h>

#include "UtilsString.h"

using namespace std;

CCurlClient::CCurlClient(CString url) :m_connectTimeout(10)
{
	m_url = CW2A(url);
}

CCurlClient::~CCurlClient()
{
	int index = 0;
}

uint32_t CCurlClient::AddHeader(const std::string& strName, const std::string& strValue)
{
	m_vecHeaders.push_back(std::make_pair(strName, strValue));
	return m_vecHeaders.size();
}

uint32_t CCurlClient::AddParam(const std::string& strName, const std::string& strValue)
{
	m_vecParams.push_back(std::make_pair(strName, strValue));
	return m_vecParams.size();
}

//续传的时间如果超过5秒还没有返回，则尝试重新下载
#define CURL_DOWNLOAD_RESUME_TIME	5
//下载过程中断网，10秒钟后返回失败
#define CURL_DOWNLOAD_FAILED_TIME	10

#ifdef _WIN32
typedef _off_t p_off_t;
#else
typedef off_t p_off_t;
#endif

typedef enum
{
	HTTP_REQUEST_OK = CURLE_OK,

	HTTP_REQUEST_ERROR = -999,
}Http_Client_Response;

typedef void(*progress_info_callback)(void* userdata, double downloadSpeed, double remainingTime, double progressPercentage);

p_off_t getLocalFileLength(string path)
{
	p_off_t ret;
	struct stat fileStat = { 0 };

	ret = stat(path.c_str(), &fileStat);
	if (ret == 0)
	{
		return fileStat.st_size;
	}

	return 0;
}

size_t lib_cur_write_callback(void* ptr, size_t size, size_t nmemb, void* userdata)
{
	FILE* fp = static_cast<FILE*>(userdata);
	size_t length = fwrite(ptr, size, nmemb, fp);
	if (length != nmemb)
	{
		return length;
	}

	return size * nmemb;
}

int lib_curl_progress_callback(void* userdata, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	Curl_Progress_User_Data* data = static_cast<Curl_Progress_User_Data*>(userdata);
	if (data->client->m_canceled) {
		SetLastError(HTTP_ERROR_CANELED);
		return 1;
	}

	time_t now = time(NULL);
	if (now - data->lasttime < 1)
	{
		return 0;
	}

	if (dltotal == 0) {
		//如果五秒钟 还是没有任何返回，则取消
		//兼容腾讯302跳转的软件，断点续传无返回的问题
		if (now - data->lasttime > CURL_DOWNLOAD_RESUME_TIME) {
			data->errorCode = CURL_CANCEL_NODATA;
			return 1;
		}
		return 0;
	}
	else
	{
		//如果持续10秒都没有新的数据反馈，则返回失败
		//解决下载过程中断网，curl无返回的问题
		if (data->lastdl != dlnow) {
			data->lastdl = dlnow;
			data->lastdltime = now;
		}
		if (data->lastdl == dlnow && now - data->lastdltime > CURL_DOWNLOAD_FAILED_TIME)
		{
			data->errorCode = CURL_CANCEL_NET_NODATA;
			return 2;
		}
	}
	data->lasttime = now;

	if (!data->client->m_processCallback) {
		return 0;
	}

	CURL* easy_handle = data->handle;

	double speed = 0;
	curl_easy_getinfo(easy_handle, CURLINFO_SPEED_DOWNLOAD, &speed);

	double leftTime = 0;
	auto progress = dlnow + data->breakPoint;

	if (data->client->m_processCallback) {
		data->client->m_processCallback(progress, data->length, speed);
	}
	return 0;
}

CString CCurlClient::GetDownloadFileName()
{
	CString fileName;
	if (m_dispositionFileName.length() > 0)
	{
		fileName = CA2T(m_dispositionFileName.c_str(), CP_UTF8).m_psz;
	}
	else
	{
		if (m_lastRedirectUrl.length() > 0) {
			fileName = GetFileNameByUrl(m_lastRedirectUrl);
		}
		if (fileName.GetLength() == 0) {
			fileName = GetFileNameByUrl(m_url);
		}
	}
	return  fileName.Trim();
}

CString CCurlClient::GetFileNameByUrl(const std::string& url)
{
	CString strUrl;
	strUrl = CA2T(url.c_str(), CP_UTF8); 
	CString strFileName = strUrl.Mid(strUrl.ReverseFind(L'/') + 1);
	int pos = strFileName.Find(L"?");
	if (pos > 0) {
		strFileName = strFileName.Mid(0, pos);
	}
	return strFileName;
}

UINT64 CCurlClient::GetFileSize(const CString& FilePath)
{
	UINT64 filesize = 0;

	HANDLE hFile = ::CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER size;
		if (GetFileSizeEx(hFile, &size))
			filesize = size.QuadPart;
		CloseHandle(hFile);
	}
	return filesize;
}

BOOL CCurlClient::DownloadFile(CString localPath, BOOL breakContinue /*= FALSE*/, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback /*= NULL*/)
{
	auto downloadFileLength = GetDownloadFileLength();
	if (downloadFileLength <= 0)
	{
		SetLastError(HTTP_ERROR_FILELENGTH);
		return FALSE;
	}

	if (localPath.GetLength() > 0)
	{
		CString fileName = GetDownloadFileName();
		m_downloadFilePath = localPath + L"\\" + fileName;
	}
	else
	{
		if (!InitDownloadPath())
		{
			return FALSE;
		}
	}
	if (::PathFileExists(m_downloadFilePath))
	{
		auto FileSize = GetFileSize(m_downloadFilePath);
		if ((FileSize >= 0) && (FileSize == downloadFileLength))
			return TRUE;
	}
	string saveTo;
	saveTo = CT2A(m_downloadFilePath);

	string partPath = saveTo + ".part";

	BOOL resumeDownload = breakContinue;

	CURL* easy_handle = NULL;
	FILE* fp = NULL;

	int ret = HTTP_REQUEST_ERROR;

	do
	{
		m_processCallback = progressCallback;

	tryDownloadAgain:
		easy_handle = curl_easy_init();

		curl_off_t resumeByte = 0;
		if (resumeDownload) {
			resumeByte = getLocalFileLength(partPath);
		}
		else
		{
			//todo:考虑重命名
			remove(saveTo.c_str());
		}

		Curl_Progress_User_Data data;
		data.client = this;
		data.breakPoint = resumeByte;
		data.handle = easy_handle;
		data.length = (curl_off_t)downloadFileLength;
		data.lasttime = time(NULL);
		data.errorCode = 0;
		data.lastdltime = data.lasttime;

		if (!easy_handle) {
			SetLastError(102);
			break;
		}

		fopen_s(&fp, partPath.c_str(), "ab+");
		if (fp == NULL) {
			int error = GetLastError();
			SetLastError(HTTP_ERROR_OPENFILE);
			break;
		}
		ret = curl_easy_setopt(easy_handle, CURLOPT_URL, m_url.c_str());
		//ret |= curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 0L);
		ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &lib_cur_write_callback);
		ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
		ret |= curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, 0L);
		ret |= curl_easy_setopt(easy_handle, CURLOPT_XFERINFOFUNCTION, &lib_curl_progress_callback);
		ret |= curl_easy_setopt(easy_handle, CURLOPT_XFERINFODATA, &data);
		ret |= curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1);
		ret |= curl_easy_setopt(easy_handle, CURLOPT_FAILONERROR, 1L);
		//ret |= curl_easy_setopt(easy_handle, CURLOPT_CONNECTTIMEOUT, m_connectTimeout);
		//ret |= curl_easy_setopt(easy_handle, CURLOPT_CONNECTTIMEOUT, easy_handle);

		if (resumeByte > 0)
		{
			ret |= curl_easy_setopt(easy_handle, CURLOPT_RESUME_FROM_LARGE, resumeByte);
		}

		if (ret != CURLE_OK)
		{
			ret = HTTP_REQUEST_ERROR;
			SetLastError(HTTP_ERROR_CURSET_FAILED);
			break;
		}

		try
		{
			ret = curl_easy_perform(easy_handle);
		}
		catch (...)
		{
			//crash...
			SetLastError(8888);
			return FALSE;
		}

		if (ret != CURLE_OK) {
			if (GetLastError() == 0) {
				SetLastError(ret);
			}
			//断线续传出错 则直接从0开始下载， 兼容腾讯的文件续传失败的问题
			if (resumeDownload && ret == CURLE_ABORTED_BY_CALLBACK)
			{
				if (data.errorCode == CURL_CANCEL_NODATA) {
					resumeDownload = FALSE;
					if (fp != NULL) {
						fclose(fp);
						fp = NULL;
					}
					curl_easy_cleanup(easy_handle);
					remove(partPath.c_str());
					easy_handle = NULL;
					goto tryDownloadAgain;
				}
				else if (data.errorCode == CURL_CANCEL_NET_NODATA)
				{
					SetLastError(CURL_CANCEL_NET_NODATA);
				}
			}

			switch (ret)
			{
			case CURLE_HTTP_RETURNED_ERROR:
			{
				int code = 0;
				curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &code);
				if (code != 0) {
					SetLastError(code);
				}
				break;
			}
			}
		}
	} while (0);

	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}

	curl_easy_cleanup(easy_handle);
	easy_handle = NULL;

	if (ret == CURLE_OK)
	{
		remove(saveTo.c_str());
		rename(partPath.c_str(), saveTo.c_str());
		return TRUE;
	}
	else
	{
		//未知错误
		if (GetLastError() == 0) {
			SetLastError(HTTP_ERROR_UNKNOW);
		}
	}
	return FALSE;
}

BOOL CCurlClient::DownloadFile(CString localDictory, CString& filePath, BOOL breakContinue /*= FALSE*/, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback /*= NULL*/)
{
	m_downloadDictory = localDictory;
	if (!::PathIsDirectory(m_downloadDictory))
	{
		::SHCreateDirectoryEx(NULL, m_downloadDictory, 0);
	}
	BOOL result = DownloadFile(L"", breakContinue, progressCallback);
	filePath = m_downloadFilePath;
	return result;
}

BOOL CCurlClient::InitDownloadPath()
{
	CString fileName = GetDownloadFileName();
	if (fileName.GetLength() == 0)
	{
		SetLastError(HTTP_ERROR_FILENAME_NONE);
		return FALSE;
	}
	if (m_downloadDictory.GetLength() == 0) {
		TCHAR tchPath[MAX_PATH + 1] = { 0 };
		ExpandEnvironmentStrings(L"%USERPROFILE%\\Downloads\\", tchPath, MAX_PATH);
		m_downloadDictory = tchPath;
	}
	m_downloadFilePath = m_downloadDictory + fileName;
	return TRUE;
}

BOOL CCurlClient::CancelDownload()
{
	m_canceled = true;
	return TRUE;
}

size_t curl_dl_header_parse(void* hdr, size_t size, size_t nmemb, void* userdata)
{
	CCurlClient* client = (CCurlClient*)userdata;
	const   size_t  cb = size * nmemb;

#ifdef DEBUG
	char* testchar = (char*)hdr;
#endif // DEBUG

	string strHeader;
	strHeader.assign((char*)hdr, cb);

	std::transform(strHeader.begin(), strHeader.end(), strHeader.begin(), ::tolower);
	//此处可能会重定向多次!!
	static string locationtag = "location:";
	auto locationPos = strHeader.find(locationtag);
	if (locationPos != std::string::npos) {
		string origin;
		origin.assign((char*)hdr, cb);
		client->m_lastRedirectUrl = origin.substr(locationPos + locationtag.length());
	}

	//如果有content-disposition，优先使用
	static string cdtag = "content-disposition:";
	auto dispositionPos = strHeader.find(cdtag);
	if (dispositionPos != std::string::npos) {
		string filetag = "filename=";
		string fileName;
		auto pos = strHeader.find("filename=");
		if (pos != std::string::npos) {
			string origin;
			origin.assign((char*)hdr, cb);

			auto dimpos = strHeader.find(";", pos + filetag.length());
			if (dimpos != std::string::npos) {
				fileName = origin.substr(pos + filetag.length(), dimpos - (pos + filetag.length()));
			}
			else
			{
				fileName = origin.substr(pos + filetag.length());
			}
		}
		client->m_dispositionFileName = fileName;
	}
	return cb;
}

size_t lib_cur_nouse_callback(char* buffer, size_t x, size_t y, void* userdata)
{
	(void)buffer;
	(void)userdata;
	return x * y;
}

long CCurlClient::GetDownloadFileLength()
{
	CURL* easy_handle = NULL;
	int ret = CURLE_OK;
	double size = 0;

	do
	{
		easy_handle = curl_easy_init();
		if (!easy_handle) {
			SetLastError(102);
			break;
		}
		this->m_curHandle = easy_handle;

		ret = curl_easy_setopt(easy_handle, CURLOPT_URL, m_url.c_str());
		ret |= curl_easy_setopt(easy_handle, CURLOPT_NOBODY, 1L);

		ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, lib_cur_nouse_callback);	// libcurl_a.lib will return error code 23 without this sentence on windows
		ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, this);

		ret |= curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1);

		ret |= curl_easy_setopt(easy_handle, CURLOPT_HEADER, 0L);
		ret |= curl_easy_setopt(easy_handle, CURLOPT_HEADERDATA, this);
		ret |= curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, curl_dl_header_parse);

		if (ret != CURLE_OK) {
			SetLastError(HTTP_ERROR_SETTINGS);
			break;
		}

		ret = curl_easy_perform(easy_handle);
		if (ret != CURLE_OK) {
			::SetLastError(HTTP_ERROR_GET_LENGTH);
			break;
		}

		ret = curl_easy_getinfo(easy_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
		if (ret != CURLE_OK) {
			size = 0;
			SetLastError(HTTP_ERROR_GET_INFO_FAILED);
			break;
		}
	} while (0);

	curl_easy_cleanup(easy_handle);
	return (long)size;
}

size_t lib_cur_receive_data_callback(void* contents, size_t size, size_t nmemb, void* stream) 
{
	string* str = (string*)stream;
	(*str).append((char*)contents, size * nmemb);
	return size * nmemb;
}

int CCurlClient::HttpGet(std::string& response, long& statusCode, int timeout)
{
	int ret = CURLE_OK;
	CURL* easy_handle = curl_easy_init();

	if (easy_handle == NULL) {
		SetLastError(HTTP_ERROR_CURINIT_FAILED);
		return CURLE_FAILED_INIT;
	}

	ret |= curl_easy_setopt(easy_handle, CURLOPT_URL, m_url.c_str());
	ret |= curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_NOSIGNAL, 1L);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, timeout);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, 1L);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, lib_cur_receive_data_callback);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, (void*)&response);
	if ((ret != CURLE_OK))
	{
		SetLastError(HTTP_ERROR_CURSET_FAILED);
		return ret;
	}
	ret = curl_easy_perform(easy_handle);
	curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &statusCode);
	curl_easy_cleanup(easy_handle);
	return ret;
}

int CCurlClient::HttpGet(long& statusCode, int timeout)
{
	int ret = CURLE_OK;
	CURL* easy_handle = curl_easy_init();

	if (easy_handle == NULL) {
		SetLastError(HTTP_ERROR_CURINIT_FAILED);
		return CURLE_FAILED_INIT;
	}

	ret |= curl_easy_setopt(easy_handle, CURLOPT_URL, m_url.c_str());
	ret |= curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 0);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_NOSIGNAL, 1L);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, timeout);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, 1L);

	if ((ret != CURLE_OK))
	{
		SetLastError(HTTP_ERROR_CURSET_FAILED);
		return ret;
	}
	ret = curl_easy_perform(easy_handle);
	curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &statusCode);
	curl_easy_cleanup(easy_handle);
	return ret;
}

// 定义一个结构体来存储响应数据
struct ResponseData {
    char* data;   // 存储响应数据的指针
    size_t size;  // 响应数据的大小
};

int CCurlClient::HttpGetTest()
{
	std::string url = "https://gms-test.service.lenovo.com/game/api/getClassifyList";
	//url = "https://www.google.com";
	//url = "https://www.baidu.com";

    CURL* curl;
    CURLcode res;

    // 初始化 libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // 创建一个 CURL 句柄
    curl = curl_easy_init();
    if (curl) {
		int ret = curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:7897");
        //curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);

		//int ret = curl_easy_setopt(curl, CURLOPT_PROXY, "");

        // 设置请求的 URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // 创建一个自定义的 Header 列表
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "devAppId: 6686088440bb48cd19f3f245");
		headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/oct-stream");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		//int ret = curl_easy_setopt(curl, CURLOPT_PROXY, "https://127.0.0.1:7897");

        //curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:7897");
        //curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);



		


		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);//不验证ssl证书。
        //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);//设置libcurl不验证host。忽略host验证必须同时忽略ssl证书验证。


		//{"Content-Type", "application/oct-stream"}
        //// 初始化响应数据结构
        //struct ResponseData response;
        //response.data = (char*) malloc(1); // 初始分配 1 字节
        //response.size = 0;

        // 设置回调函数以捕获响应数据
        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, lib_cur_receive_data_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, lib_cur_receive_data_callback);
        //curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

        // 执行请求
        res = curl_easy_perform(curl);

        // 检查请求是否成功
        if (res != CURLE_OK)		
		{
			cout << "--------------------------------------------------------------" << endl;
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else 
		{
            // 打印响应数据
			cout << "===============================================================" << endl;
			cout << response.c_str() << endl;
           // printf("Response data:\n%s\n", response.c_str());
        }

        // 清理响应数据
        //free(response.data);

        // 清理自定义 Header 列表
        curl_slist_free_all(headers);

        // 清理 CURL 句柄
        curl_easy_cleanup(curl);
    }

    // 清理 libcurl
    curl_global_cleanup();

    return 0;
}

int CCurlClient::HttpPost(std::string& response, long& statusCode, const std::string& requestHeader, const std::string& requestBody, int timeout)
{
	int ret = CURLE_OK;

	CURL* easy_handle = curl_easy_init();
	if (easy_handle == NULL) 
	{
		SetLastError(HTTP_ERROR_CURINIT_FAILED);
		return CURLE_FAILED_INIT;
	}

	//添加header
	struct curl_slist* headers = NULL;
	if (!m_vecHeaders.empty())
	{
		//优先使用 m_vecHeaders
		for (auto& it : m_vecHeaders)
		{
			std::string strHeader = UtilsString::FormatString("%s:%s", std::get<0>(it), std::get<1>(it));
			headers = curl_slist_append(headers, requestHeader.c_str());
		}
	}
	else
	{
		headers = curl_slist_append(headers, requestHeader.c_str());
	}

	//添加params
	std::string strBody = requestBody;
	if (!m_vecParams.empty())
	{
		//优先使用 m_vecParams
		strBody = UtilsString::BuildHttpBody(m_vecParams);
	}

	ret |= curl_easy_setopt(easy_handle, CURLOPT_URL, m_url.c_str());
	ret |= curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_POST, 1L);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, strBody.c_str());
	ret |= curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, timeout);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, lib_cur_receive_data_callback);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, (void*)&response);

	ret = curl_easy_perform(easy_handle);
	curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &statusCode);
	curl_easy_cleanup(easy_handle);
	return ret;
}

std::string CalcCheckData(std::vector<std::tuple<std::string, std::string>>& vecData)
{
	std::string strContent;
	int idx = 0;
	for (auto iter : vecData)
	{
		strContent += std::get<1>(iter);
		++idx;
		if (idx < vecData.size())
			strContent += "&";
	}

	return strContent;

}

int32_t  CCurlClient::TestPost()
{
	int ret = CURLE_OK;
	std::vector<std::tuple<std::string, std::string>> vecData = 
	{
		std::make_tuple("grant_type", "client_credentials"),
		std::make_tuple("client_id", "1593389727517312"),
		std::make_tuple("client_secret", "d248f803532a4d009c4bdecfb5bcd5cc")
	};

	int statusCode = 0;
	int32_t timeout = 30;
	std::string requestBody = CalcCheckData(vecData);
	const char* postData = "grant_type=client_credentials&client_id=1593389727517312&client_secret=d248f803532a4d009c4bdecfb5bcd5cc";
	std::string response;
	CURL* easy_handle = curl_easy_init();
	if (easy_handle == NULL)
	{
		SetLastError(HTTP_ERROR_CURINIT_FAILED);
		return CURLE_FAILED_INIT;
	}

	std::string url = "https://cloud-pay.mbgtest.lenovomm.com/cloud-auth/oauth/token";

	ret |= curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());

	// 设置请求头
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
	//ret |= curl_easy_setopt(easy_handle, CURLOPT_CAINFO, "D:/curl_sdk/cacert.pem");
	ret |= curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_POST, 1L);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, postData);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, timeout);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, lib_cur_receive_data_callback);
	ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, (void*)&response);

	ret = curl_easy_perform(easy_handle);
	curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &statusCode);
	curl_easy_cleanup(easy_handle);
	return ret;
}


size_t write_callback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	FILE* fp = (FILE*)stream;
	size_t written = fwrite(ptr, size, nmemb, fp);
	return written;
}

int progress_callback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (dltotal > 0)
	{
		int nPersent = (int)(100.0 * dlnow / dltotal);
		printf("[%f/%f]下载进度:%d\r", dlnow, dltotal, nPersent);
	}
	return 0;
}

int32_t CCurlClient::TestDLFile(CString localPath)
{
	CURL* curl = curl_easy_init();
	if (curl == NULL)
	{
		SetLastError(HTTP_ERROR_CURINIT_FAILED);
		return CURLE_FAILED_INIT;
	}

	std::string	url = "https://guanjia.lenovo.com.cn/download/lenovopcmanager_apps.exe";
	int ret = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	// 显示重定向次数
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	// 是否验证证书
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	// 是否验证主机名
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	FILE* file = NULL;

	std::string fileName = "d:\\tmp123\\a.dat";
	fopen_s(&file, fileName.c_str(), "wb");
	if (!file)
	{
		//cout << "文件创建失败！" << endl;
	}
	// 设置数据回调的句柄
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

	// 设置文件下载进度回调
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
	// 设置进度回调函数
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);

	// 开始执行请求
	CURLcode res = curl_easy_perform(curl);
	// 判断错误
	if (res != CURLE_OK)
	{
		cout << "下载失败:" << curl_easy_strerror(res) << endl;
	}

	fclose(file);
	curl_easy_cleanup(curl);

	return 0;
}
